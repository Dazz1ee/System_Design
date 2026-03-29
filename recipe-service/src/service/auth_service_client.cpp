#include "auth_service_client.hpp"

#include <userver/clients/http/component.hpp>
#include <userver/clients/http/response.hpp>
#include <userver/formats/json.hpp>
#include <userver/logging/log.hpp>

#include <userver/server/handlers/exceptions.hpp>
#include "schemas/user.hpp"

namespace auth::services {

using namespace userver;

AuthServiceClient::AuthServiceClient(
    const components::ComponentConfig& config,
    const components::ComponentContext& context)
    : ComponentBase(config, context),
      http_client_(
          context.FindComponent<components::HttpClient>().GetHttpClient()),
      base_url_(config["base-url"].As<std::string>()) {}

yaml_config::Schema AuthServiceClient::GetStaticConfigSchema() {
  return yaml_config::impl::SchemaFromString(R"(
type: object
description: auth service
additionalProperties: false
properties:
    load-enabled:
        type: boolean
        description: whether to enable the component

    base-url:
        type: string
        description: auth service base url
)");
}

schemas::CreateUserResponseDTO AuthServiceClient::CreateUser(
    const std::string& request) const {
  auto response = http_client_.CreateRequest()
                      .post(base_url_ + CREATE_USER_PATH, request)
                      .headers({{"Accept", "application/json"},
                                {"Content-Type", "application/json"}})
                      .timeout(std::chrono::seconds{2})
                      .perform();

  if (response->status_code() == clients::http::Status::kConflict) {
    throw server::handlers::CustomHandlerException(
        server::handlers::HandlerErrorCode::kConflictState);
  }

  if (response->status_code() != clients::http::Status::kCreated &&
      response->status_code() != clients::http::Status::kOk) {
    LOG_ERROR() << "User service returned status: " << response->status_code();
    throw server::handlers::CustomHandlerException(
        server::handlers::HandlerErrorCode::kBadGateway);
  }

  const auto response_json = formats::json::FromString(response->body());
  return response_json.As<schemas::CreateUserResponseDTO>();
}

schemas::LoginResponseDTO AuthServiceClient::Login(
    const std::string& request) const {
  auto response = http_client_.CreateRequest()
                      .post(base_url_ + LOGIN_PATH, request)
                      .headers({{"Accept", "application/json"},
                                {"Content-Type", "application/json"}})
                      .timeout(std::chrono::seconds{2})
                      .retry()
                      .perform();

  if (response->status_code() == clients::http::Status::kConflict) {
    throw server::handlers::CustomHandlerException(
        server::handlers::HandlerErrorCode::kConflictState);
  }

  if (response->status_code() == clients::http::Status::kUnauthorized) {
    throw server::handlers::CustomHandlerException(
        server::handlers::HandlerErrorCode::kUnauthorized);
  }

  if (response->status_code() != clients::http::Status::kOk) {
    throw server::handlers::CustomHandlerException(
        server::handlers::HandlerErrorCode::kBadGateway);
  }

  const auto response_json = formats::json::FromString(response->body());
  return response_json.As<schemas::LoginResponseDTO>();
}

std::shared_ptr<schemas::JWKSResponseDTO> AuthServiceClient::GetJwks() const {
  auto now = std::chrono::steady_clock::now();

  if (now - last_update_ < kCacheTtl) {
    auto cache_ptr = jwks_cache_.Read();
    if (*cache_ptr && !(*cache_ptr)->keys.empty()) {
      return *cache_ptr;
    }
  }

  std::lock_guard lock(update_mutex_);

  try {
    auto response = http_client_.CreateRequest()
                        .get(base_url_ + JWKS_PATH)
                        .headers({{"Accept", "application/json"}})
                        .perform();

    if (!response || response->status_code() != clients::http::Status::kOk) {
      throw server::handlers::CustomHandlerException(
          server::handlers::HandlerErrorCode::kBadGateway);
    }

    auto parsed_json = formats::json::FromString(response->body());

    auto new_jwks = std::make_shared<schemas::JWKSResponseDTO>(
        parsed_json.As<schemas::JWKSResponseDTO>());

    jwks_cache_.Assign(new_jwks);
    last_update_ = std::chrono::steady_clock::now();

    return new_jwks;
  } catch (const std::exception& e) {
    auto old_cache = jwks_cache_.Read();
    if (*old_cache && !(*old_cache)->keys.empty()) {
      LOG_WARNING() << "Failed to get JWKS: " << e.what();
      return *old_cache;
    }
    throw;
  }
}
}  // namespace auth::services