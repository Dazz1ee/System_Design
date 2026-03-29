FROM ghcr.io/userver-framework/ubuntu-24.04-userver:latest AS builder
WORKDIR /app

COPY . .

ARG BUILD_TYPE=release
RUN cmake --preset ${BUILD_TYPE} \
    && cmake --build build-${BUILD_TYPE} -j$(nproc) --target auth_service

FROM ubuntu:24.04
RUN apt-get update && apt-get install -y \
        curl \
        libssl3t64 \
        libcurl4t64 \
        libev4t64 \
        libnghttp2-14 \
        libc-ares2 \
        libboost-iostreams1.83.0 \
        libboost-filesystem1.83.0 \
        libboost-locale1.83.0 \
        libboost-program-options1.83.0 \
        libboost-regex1.83.0 \
        libboost-stacktrace1.83.0 \
        libcrypto++8t64 \
        libjemalloc2 \
        libre2-10 \
        libfmt9 \
        libcctz2 \
        libyaml-cpp0.8 \
        zlib1g \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY --from=builder /app/build-release/auth_service /app/auth_service
COPY configs/ /app/configs/
COPY entrypoint.sh /app/entrypoint.sh
RUN chmod +x /app/entrypoint.sh

EXPOSE 8081

CMD ["/app/entrypoint.sh"]