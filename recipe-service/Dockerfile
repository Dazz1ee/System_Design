FROM ghcr.io/userver-framework/ubuntu-24.04-userver:latest AS builder
WORKDIR /app

COPY . .

ARG BUILD_TYPE=release
RUN cmake --preset ${BUILD_TYPE} \
    && cmake --build build-${BUILD_TYPE} -j$(nproc) --target recipe_service

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
        libhiredis-dev \
        libboost-locale1.83.0 \
        libboost-program-options1.83.0 \
        libboost-regex1.83.0 \
        libboost-stacktrace1.83.0 \
        libboost-atomic1.83.0 \
        libcrypto++8t64 \
        libjemalloc2 \
        libre2-10 \
        libfmt9 \
        libcctz2 \
        libyaml-cpp0.8 \
        zlib1g \
        libbson-dev \
        libmongoc-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY --from=builder /app/build-release/recipe_service /app/recipe_service
COPY configs/ /app/configs/

EXPOSE 8081 8082

CMD ["./recipe_service", \
     "--config", "/app/configs/static_config.yaml", \
     "--config_vars", "/app/configs/config_vars.yaml"]