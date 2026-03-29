#!/bin/bash
set -e

KEY_DIR="/app/keys"

mkdir -p $KEY_DIR

if [ ! -f "$KEY_DIR/private.pem" ]; then
  openssl genrsa -out $KEY_DIR/private.pem 2048

  openssl rsa -in $KEY_DIR/private.pem \
      -pubout \
      -out $KEY_DIR/public.pem
fi
exec ./auth_service \
    --config /app/configs/static_config.yaml \
    --config_vars /app/configs/config_vars.yaml