#include <handler/create_recipe/handler.hpp>

#include "auth/jwt_auth_checker.hpp"
#include "auth/jwt_utils.hpp"
#include "schemas/recipe.hpp"

#include "service/recipe_service.hpp"

namespace recipe::create {

Handler::Handler(const userver::components::ComponentConfig& config,
                 const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context),
      recipe_service_(context.FindComponent<services::RecipeService>()) {}

void Validate(const schemas::CreateRecipeRequestDTO& request) {
  if (request.title.empty() || request.steps.empty() ||
      request.ingredients.empty()) {
    throw userver::server::handlers::ClientError();
  }
}

std::string Handler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext& context) const {
  const auto request_json =
      userver::formats::json::FromString(request.RequestBody());

  const auto request_dto = request_json.As<schemas::CreateRecipeRequestDTO>();
  Validate(request_dto);

  auto user_id = context.GetData<int64_t>(auth::jwt::JwtUtils::kClaim);

  auto response_dto = recipe_service_.CreateRecipe(request_dto, user_id);
  const auto response_json =
      userver::formats::json::ValueBuilder{response_dto}.ExtractValue();

  auto& response = request.GetHttpResponse();
  response.SetContentType(userver::http::content_type::kApplicationJson);
  response.SetStatus(userver::server::http::HttpStatus::kCreated);
  return userver::formats::json::ToString(response_json);
}

}  // namespace recipe::create