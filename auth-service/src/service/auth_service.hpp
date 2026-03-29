#pragma once

#include <userver/clients/http/client.hpp>
#include <userver/components/component_base.hpp>
#include <userver/components/component.hpp>
#include <string>
#include "entity/entities.hpp"

#include "schemas/auth.hpp"
#include "db_service.hpp"

namespace auth::services {

class AuthService final : public userver::components::ComponentBase {
public:
  static constexpr std::string_view kName = "auth-service";

  AuthService(const userver::components::ComponentConfig& config,
                    const userver::components::ComponentContext& context);

  schemas::CreateUserResponseDTO CreateUser(const schemas::CreateUserRequestDTO& request) const;

  schemas::LoginResponseDTO Login(const schemas::LoginRequestDTO& request) const;

  schemas::JWKSResponseDTO GetJwks() const;

  static userver::yaml_config::Schema GetStaticConfigSchema();

private:
  const DbService& db_service_;
  std::string public_key_;
  std::string private_key_;
  const std::string KID = "test";
};

}  // namespace recipe