#pragma once

#include <userver/server/handlers/http_handler_base.hpp>

#include "service/user_service.hpp"

namespace user {

class Handler final : public userver::server::handlers::HttpHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-login";

  Handler(const userver::components::ComponentConfig& config,
          const userver::components::ComponentContext& context);

  std::string HandleRequestThrow(
      const userver::server::http::HttpRequest& request,
      userver::server::request::RequestContext&) const override;

 private:
  const services::UserService& user_service_;
};

}  // namespace recipe::user