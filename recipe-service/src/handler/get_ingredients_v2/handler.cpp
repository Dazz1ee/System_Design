#include <handler/get_ingredients_v2/handler.hpp>

#include "schemas/recipe.hpp"

#include "service/recipe_service.hpp"

namespace recipe::ingredients::v2 {

Handler::Handler(const userver::components::ComponentConfig& config,
                 const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context),
      recipe_service_(context.FindComponent<services::RecipeService>()) {}

std::string Handler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext&) const {
const auto& recipe_id_str = request.GetPathArg("id");
  if (recipe_id_str.empty()) {
    throw userver::server::handlers::ClientError();
  }

  auto response_dto = recipe_service_.GetRecipeIngredientsV2(recipe_id_str);
  const auto response_json =
      userver::formats::json::ValueBuilder{response_dto}.ExtractValue();

  auto& response = request.GetHttpResponse();
  response.SetContentType(userver::http::content_type::kApplicationJson);
  response.SetStatus(userver::server::http::HttpStatus::kOk);
  return userver::formats::json::ToString(response_json);
}

}  // namespace recipe