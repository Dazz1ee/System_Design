#include <handler/get_jwks/handler.hpp>

#include "schemas/auth.hpp"

namespace auth::jwks {

Handler::Handler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context),
      auth_service_(context.FindComponent<services::AuthService>()) {}

std::string Handler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext&) const {
  auto response_dto = auth_service_.GetJwks();
  const auto response_json =
      userver::formats::json::ValueBuilder{response_dto}.ExtractValue();

  auto& response = request.GetHttpResponse();
  response.SetContentType(userver::http::content_type::kApplicationJson);
  response.SetStatus(userver::server::http::HttpStatus::kOk);
  return userver::formats::json::ToString(response_json);
}

}  // namespace user