#pragma once

#include <string>
#include <userver/clients/http/client.hpp>
#include <userver/components/component.hpp>
#include <userver/components/component_base.hpp>
#include <userver/yaml_config/schema.hpp>
#include <userver/rcu/rcu.hpp>
#include <userver/engine/mutex.hpp>

#include "schemas/user.hpp"

namespace auth::services {

class AuthServiceClient final : public userver::components::ComponentBase {
public:
  static constexpr std::string_view kName = "auth-service";

  AuthServiceClient(const userver::components::ComponentConfig& config,
              const userver::components::ComponentContext& context);

  schemas::CreateUserResponseDTO CreateUser(
      const std::string& request) const;

  schemas::LoginResponseDTO Login(
      const std::string& request) const;

  std::shared_ptr<schemas::JWKSResponseDTO> GetJwks() const;

  static userver::yaml_config::Schema GetStaticConfigSchema();

private:
  userver::clients::http::Client& http_client_;
  std::string base_url_;
  mutable userver::engine::Mutex update_mutex_;
  mutable userver::rcu::Variable<std::shared_ptr<schemas::JWKSResponseDTO>> jwks_cache_;
  mutable std::chrono::steady_clock::time_point last_update_{};
  static constexpr std::chrono::minutes kCacheTtl{60};
  const std::string CREATE_USER_PATH = "/api/v1/user";
  const std::string LOGIN_PATH = "/api/v1/login";
  const std::string JWKS_PATH = "/.well-known/jwks.json";
};

}  // namespace user::services