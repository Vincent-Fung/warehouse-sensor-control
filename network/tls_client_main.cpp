#include <arpa/inet.h>
#include <iostream>
#include <memory>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;

struct SslCtxDeleter { void operator()(SSL_CTX* c) const { SSL_CTX_free(c); } };
struct SslDeleter    { void operator()(SSL* s) const { SSL_free(s); } };
using SslCtxPtr = unique_ptr<SSL_CTX, SslCtxDeleter>;
using SslPtr    = unique_ptr<SSL, SslDeleter>;

int main(int argc, char* argv[]) {
    string message = argc > 1 ? argv[1] : "hallo tls";
    string caFile  = argc > 2 ? argv[2] : "cert.pem";

    SslCtxPtr ctx(SSL_CTX_new(TLS_client_method()));
    if (!ctx) { ERR_print_errors_fp(stderr); return 1; }
    SSL_CTX_set_min_proto_version(ctx.get(), TLS1_2_VERSION);

    // Welchen Zertifikaten vertraue ich? Nur dem, was ich hier angebe.
    if (SSL_CTX_load_verify_locations(ctx.get(), caFile.c_str(), nullptr) != 1) {
        ERR_print_errors_fp(stderr);
        return 1;
    }
    SSL_CTX_set_verify(ctx.get(), SSL_VERIFY_PEER, nullptr);     // Server-Zertifikat prüfen

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9443);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
    if (connect(sock, (sockaddr*)&addr, sizeof(addr)) < 0) {
        cerr << "TCP-Verbindung fehlgeschlagen" << endl;
        return 1;
    }

    SslPtr ssl(SSL_new(ctx.get()));
    SSL_set_fd(ssl.get(), sock);
    SSL_set_tlsext_host_name(ssl.get(), "localhost");            // SNI
    SSL_set1_host(ssl.get(), "localhost");                       // Hostname muss zum Zertifikat passen

    if (SSL_connect(ssl.get()) != 1) {                           // Handshake + Zertifikatsprüfung
        cerr << "TLS-Handshake fehlgeschlagen: "
             << X509_verify_cert_error_string(SSL_get_verify_result(ssl.get())) << endl;
        ERR_print_errors_fp(stderr);
        close(sock);
        return 1;
    }
    cout << "Verbunden: " << SSL_get_version(ssl.get())
         << ", " << SSL_get_cipher(ssl.get()) << endl;

    SSL_write(ssl.get(), message.data(), static_cast<int>(message.size()));

    char buf[1024];
    int n = SSL_read(ssl.get(), buf, sizeof(buf));
    if (n > 0) cout << "Antwort: " << string(buf, n) << endl;

    SSL_shutdown(ssl.get());
    close(sock);
    return 0;
}