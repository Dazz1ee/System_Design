#include "jwt_utils.hpp"
#include <chrono>
#include <stdexcept>

namespace auth::jwt {

void JwtUtils::ValidateToken(const ::jwt::decoded_jwt<::jwt::traits::kazuho_picojson>& token,
                             const std::shared_ptr<schemas::JWKSResponseDTO>& jwks) {
  if (!jwks || jwks->keys.empty()) {
    throw std::runtime_error("JWKS error");
  }

  auto now = std::chrono::system_clock::now();
  if (token.has_expires_at() && token.get_expires_at() < now) {
    throw std::runtime_error("Token expired");
  }

  const auto kid = token.get_key_id();

  for (const auto& public_key : jwks->keys) {
    if (public_key.kid == kid) {
      if (public_key.kty != "RSA") {
        throw std::runtime_error("Unsupported key type: " + public_key.kty);
      }

      try {
        ::jwt::verify()
            .allow_algorithm(::jwt::algorithm::rs256(public_key.pem, "", "", ""))
            .with_claim(std::string(kClaim), token.get_payload_claim(std::string(kClaim)))
            .verify(token);

        return;
      } catch (const std::exception& e) {
        throw std::runtime_error(std::string("Token verification failed: ") + e.what());
      }
    }
  }

  throw std::runtime_error("Public key with kid '" + kid + "' not found");
}

}  // namespace auth::jwt