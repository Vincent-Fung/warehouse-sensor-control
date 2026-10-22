#include <arpa/inet.h>
#include <iostream>
#include <memory>
#include <netinet/in.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;

struct SslCtxDeleter { void operator()(SSL_CTX* c) const { SSL_CTX_free(c); } };
struct SslDeleter    { void operator()(SSL* s) const { SSL_free(s); } };
using SslCtxPtr = unique_ptr<SSL_CTX, SslCtxDeleter>;
using SslPtr    = unique_ptr<SSL, SslDeleter>;

int main() {
    SslCtxPtr ctx(SSL_CTX_new(TLS_server_method()));
    if (!ctx) { ERR_print_errors_fp(stderr); return 1; }

    SSL_CTX_set_min_proto_version(ctx.get(), TLS1_2_VERSION);   // keine alten, unsicheren Versionen

    if (SSL_CTX_use_certificate_file(ctx.get(), "cert.pem", SSL_FILETYPE_PEM) != 1 ||
        SSL_CTX_use_PrivateKey_file(ctx.get(), "key.pem", SSL_FILETYPE_PEM) != 1) {
        ERR_print_errors_fp(stderr);
        return 1;
    }

    int listenFd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(9443);

    if (bind(listenFd, (sockaddr*)&addr, sizeof(addr)) < 0 || listen(listenFd, 5) < 0) {
        cerr << "bind/listen fehlgeschlagen" << endl;
        return 1;
    }
    cout << "TLS-Server lauscht auf Port 9443" << endl;

    while (true) {
        int clientFd = accept(listenFd, nullptr, nullptr);
        if (clientFd < 0) continue;

        {
            SslPtr ssl(SSL_new(ctx.get()));
            SSL_set_fd(ssl.get(), clientFd);

            if (SSL_accept(ssl.get()) <= 0) {                   // TLS-Handshake
                ERR_print_errors_fp(stderr);
            } else {
                cout << "Handshake ok: " << SSL_get_version(ssl.get())
                     << ", " << SSL_get_cipher(ssl.get()) << endl;

                char buf[1024];
                int n = SSL_read(ssl.get(), buf, sizeof(buf));
                if (n > 0) {
                    cout << "Empfangen: " << string(buf, n) << endl;
                    SSL_write(ssl.get(), buf, n);               // Echo
                }
                SSL_shutdown(ssl.get());
            }
        }   // SSL wird hier freigegeben
        close(clientFd);
    }
}