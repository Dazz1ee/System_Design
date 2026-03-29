#include "jwt_auth_checker.hpp"

#include <jwt-cpp/jwt.h>

#include <userver/chaotic/validators.hpp>
#include <userver/http/common_headers.hpp>

#include "jwt_utils.hpp"

namespace auth::jwt {

namespace {
constexpr std::string_view kAlgorithm = "Bearer ";
}  // namespace

JwtChecker::JwtChecker(const services::AuthServiceClient& auth_service_client)
    : auth_service_client_(auth_service_client) {}

JwtChecker::AuthCheckResult JwtChecker::CheckAuth(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext& context) const {
  const std::string_view auth_header =
      request.GetHeader(userver::http::headers::kAuthorization);
  if (auth_header.empty()) {
    return AuthCheckResult{AuthCheckResult::Status::kTokenNotFound,
                           "Missing 'Authorization' header"};
  }

  if (!auth_header.starts_with(kAlgorithm)) {
    return AuthCheckResult{AuthCheckResult::Status::kInvalidToken,
                           "Invalid authorization type, expected 'Bearer'"};
  }

  const std::string_view token = auth_header.substr(kAlgorithm.length());
  try {
    auto decoded = ::jwt::decode(std::string(token));

    auto jwks = auth_service_client_.GetJwks();
    JwtUtils::ValidateToken(decoded, jwks);

    auto claim = std::string(JwtUtils::kClaim);
    auto user_id = decoded.get_payload_claim(claim);
    context.SetData<int64_t>(claim, std::stoll(user_id.as_string()));
    return AuthCheckResult{AuthCheckResult::Status::kOk, "Token is valid"};
  } catch (const ::jwt::error::token_verification_exception& exc) {
    return AuthCheckResult{
        AuthCheckResult::Status::kInvalidToken,
        "Token verification failed: " + std::string{exc.what()}};
  } catch (const std::exception& exc) {
    return AuthCheckResult{
        AuthCheckResult::Status::kForbidden,
        "Token processing error: " + std::string{exc.what()}};
  }
}

JwtAuthComponent::JwtAuthComponent(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : LoggableComponentBase(config, context) {
  authorizer_ = std::make_shared<JwtChecker>(
      context.FindComponent<services::AuthServiceClient>());
}

JwtCheckerPtr JwtAuthComponent::Get() const { return authorizer_; }

}  // namespace auth::jwt