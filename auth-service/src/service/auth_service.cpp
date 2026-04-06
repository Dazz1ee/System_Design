#include "auth_service.hpp"

#include <jwt-cpp/jwt.h>
#include <userver/formats/json.hpp>
#include <userver/fs/read.hpp>
#include <filesystem>
#include <userver/server/handlers/exceptions.hpp>
#include "schemas/auth.hpp"

namespace auth::services {

namespace {
constexpr std::string_view jwtType = "JWT";
constexpr std::string_view algorithm = "RS256";
constexpr const char* userId = "user_id";
}  // namespace

AuthService::AuthService(const userver::components::ComponentConfig& config,
                         const userver::components::ComponentContext& context)
    : ComponentBase(config, context),
      db_service_(context.FindComponent<DbService>()) {
  auto& task_processor = context.GetTaskProcessor("fs-task-processor");
  std::cout << "CWD: " << std::filesystem::current_path() << std::endl;
  public_key_ = userver::fs::ReadFileContents(
      task_processor, config["public-key-path"].As<std::string>());

  private_key_ = userver::fs::ReadFileContents(
      task_processor, config["private-key-path"].As<std::string>());
}

userver::yaml_config::Schema AuthService::GetStaticConfigSchema() {
  return userver::yaml_config::impl::SchemaFromString(R"(
type: object
description: user service
additionalProperties: false
properties:
    load-enabled:
        type: boolean
        description: whether to enable the component

    public-key-path:
        type: string
        description: public key
    private-key-path:
        type: string
        description: private key
)");
}

schemas::CreateUserResponseDTO AuthService::CreateUser(
    const schemas::CreateUserRequestDTO& request) const {
  entity::User user = {.id = 0,
                       .forename = request.forename,
                       .surname = request.surname,
                       .email = request.email,
                       .password = request.password,
                       .created_at = std::chrono::system_clock::now()};
  const auto& saved_user = db_service_.Save(user);

  return schemas::CreateUserResponseDTO{.id = saved_user.id};
}

schemas::LoginResponseDTO AuthService::Login(
    const schemas::LoginRequestDTO& request) const {
  const auto& user = db_service_.GetUserByEmail(request.email);
  if (request.password != user.password) {
    throw userver::server::handlers::Unauthorized();
  }
  auto token =
      jwt::create()
          .set_type(std::string(jwtType))
          .set_key_id(KID)
          .set_algorithm(std::string(algorithm))
          .set_payload_claim(userId, jwt::claim(std::to_string(user.id)))
          .set_issued_at(std::chrono::system_clock::now())
          .set_expires_at(std::chrono::system_clock::now() +
                          std::chrono::hours(24))
          .sign(jwt::algorithm::rs256(public_key_, private_key_, "", ""));

  return schemas::LoginResponseDTO{.token = token, .userId = user.id};
}

schemas::JWKSResponseDTO AuthService::GetJwks() const {
  schemas::JWKSResponseDTO response;

  schemas::JWK jwk;
  jwk.kty = "RSA";
  jwk.use = "sig";
  jwk.alg = "RS256";
  jwk.kid = KID;
  jwk.pem = public_key_;

  response.keys.push_back(jwk);
  return response;
}

}  // namespace auth::services