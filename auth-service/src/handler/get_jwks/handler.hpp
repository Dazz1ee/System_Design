#pragma once

#include <userver/server/handlers/http_handler_base.hpp>

#include "service/auth_service.hpp"

namespace auth::jwks {

class Handler final : public userver::server::handlers::HttpHandlerBase {
public:
  static constexpr std::string_view kName = "handler-get-jwks";

  Handler(const userver::components::ComponentConfig& config,
          const userver::components::ComponentContext& context);

  std::string HandleRequestThrow(
      const userver::server::http::HttpRequest& request,
      userver::server::request::RequestContext&) const override;

private:
  const services::AuthService& auth_service_;
};

}  // namespace recipe::user