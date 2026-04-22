#pragma once

#include <userver/server/handlers/http_handler_base.hpp>

#include "service/recipe_service.hpp"
#include "service/user_service.hpp"

namespace recipe::create::v2 {

class Handler final : public userver::server::handlers::HttpHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-create-recipe-v2";

  Handler(const userver::components::ComponentConfig& config,
          const userver::components::ComponentContext& context);

  std::string HandleRequestThrow(
      const userver::server::http::HttpRequest& request,
      userver::server::request::RequestContext&) const override;

 private:
  const services::RecipeService& recipe_service_;
};

}  // namespace recipe::create