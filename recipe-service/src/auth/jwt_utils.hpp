#pragma once

#include <jwt-cpp/jwt.h>
#include "schemas/user.hpp"

namespace auth::jwt {

class JwtUtils {
 public:
  static constexpr std::string_view kClaim = "user_id";

  static void ValidateToken(const ::jwt::decoded_jwt<::jwt::traits::kazuho_picojson>& token,
                            const std::shared_ptr<schemas::JWKSResponseDTO>& jwks);
};

}  // namespace auth::jwt