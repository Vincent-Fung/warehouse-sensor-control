FROM ubuntu:24.04 AS build

RUN apt-get update && apt-get install -y \
    cmake g++ python3-dev python3-pip libssl-dev \
    && pip install pybind11 --break-system-packages --no-cache-dir

WORKDIR /app
COPY . .

RUN rm -rf build && mkdir build && cd build && cmake .. && cmake --build . --target warehouse_test tcp_server tcp_client

FROM ubuntu:24.04
COPY --from=build /app/build/warehouse_test /app/warehouse_test
COPY --from=build /app/build/tcp_server /app/tcp_server
COPY --from=build /app/build/tcp_client /app/tcp_client
WORKDIR /app
CMD ["./warehouse_test"]