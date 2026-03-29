#include "user_service.hpp"

#include <userver/clients/http/component.hpp>
#include <userver/formats/json.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/logging/log.hpp>

#include <userver/server/handlers/exceptions.hpp>
#include "schemas/user.hpp"

namespace user::services {

using namespace userver;

UserService::UserService(const components::ComponentConfig& config,
                         const components::ComponentContext& context)
    : ComponentBase(config, context),
      auth_service_client_(
          context.FindComponent<auth::services::AuthServiceClient>()) {}

schemas::CreateUserResponseDTO UserService::CreateUser(
    const schemas::CreateUserRequestDTO& request) const {
  formats::json::ValueBuilder builder;
  builder["name"] = request.name;
  builder["email"] = request.email;
  builder["password"] = request.password;
  const auto request_json = formats::json::ToString(builder.ExtractValue());

  return auth_service_client_.CreateUser(request_json);
}

schemas::LoginResponseDTO UserService::Login(
    const schemas::LoginRequestDTO& request) const {
  formats::json::ValueBuilder builder;
  builder["email"] = request.email;
  builder["password"] = request.password;
  const auto request_json = formats::json::ToString(builder.ExtractValue());

  return auth_service_client_.Login(request_json);
}

}  // namespace user::services