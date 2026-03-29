#pragma once

#include <userver/server/handlers/http_handler_base.hpp>

#include "service/recipe_service.hpp"
#include "service/user_service.hpp"

namespace recipe::all {

class Handler final : public userver::server::handlers::HttpHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-get-recipes";

  Handler(const userver::components::ComponentConfig& config,
          const userver::components::ComponentContext& context);

  std::string HandleRequestThrow(
      const userver::server::http::HttpRequest& request,
      userver::server::request::RequestContext&) const override;

 private:
  const services::RecipeService& recipe_service_;
  const std::int64_t LAST_ID_ = 0;
  const std::int64_t LIMIT_ = 300;
};

}  // namespace recipe::user