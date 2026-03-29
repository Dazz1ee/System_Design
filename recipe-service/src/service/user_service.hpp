#pragma once

#include <userver/components/component.hpp>
#include <userver/components/component_base.hpp>
#include <userver/yaml_config/schema.hpp>

#include "schemas/user.hpp"
#include "auth_service_client.hpp"

namespace user::services {

class UserService final : public userver::components::ComponentBase {
 public:
  static constexpr std::string_view kName = "user-service";

  UserService(const userver::components::ComponentConfig& config,
              const userver::components::ComponentContext& context);

  schemas::CreateUserResponseDTO CreateUser(
      const schemas::CreateUserRequestDTO& request) const;

  schemas::LoginResponseDTO Login(
      const schemas::LoginRequestDTO& request) const;

 private:
  const auth::services::AuthServiceClient& auth_service_client_;
};

}  // namespace user::services