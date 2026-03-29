#include <handler/login/handler.hpp>

#include "schemas/auth.hpp"

namespace auth::login {

Handler::Handler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context),
      auth_service_(
          context.FindComponent<services::AuthService>()) {}

void Validate(const schemas::LoginRequestDTO& request_dto) {
  if (request_dto.email.empty() || request_dto.password.empty()) {
    throw userver::server::handlers::ClientError();
  }
}

std::string Handler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext&) const {
  auto request_json =
      userver::formats::json::FromString(request.RequestBody());

  auto request_dto =
      request_json.As<schemas::LoginRequestDTO>();
  Validate(request_dto);

  auto response_dto = auth_service_.Login(request_dto);

  auto response_json =
      userver::formats::json::ValueBuilder{response_dto}.ExtractValue();

  auto& response = request.GetHttpResponse();
  response.SetContentType(userver::http::content_type::kApplicationJson);
  response.SetStatus(userver::server::http::HttpStatus::kOk);
  return userver::formats::json::ToString(response_json);
}

}  // namespace user