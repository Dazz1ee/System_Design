#include <handler/get_recipes/handler.hpp>

#include "schemas/recipe.hpp"

namespace recipe::all {

Handler::Handler(const userver::components::ComponentConfig& config,
                 const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context),
      recipe_service_(context.FindComponent<services::RecipeService>()) {}
int64_t ConvertToIntOrDefault(const std::string& arg, int64_t default_value) {
  if (arg.empty()) {
    return default_value;
  }

  try {
    return std::stoll(std::string(arg));
  } catch (...) {
    throw userver::server::handlers::ClientError();
  }
}

std::string Handler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext&) const {
  const auto& lastIdArg = request.GetArg("last_id");
  const auto& limitArg = request.GetArg("limit");
  std::int64_t lastId = ConvertToIntOrDefault(lastIdArg, LAST_ID_);
  std::int64_t limit = ConvertToIntOrDefault(limitArg, LIMIT_);

  if (limit > LIMIT_ || limit < 1 || lastId < 0) {
    throw userver::server::handlers::ClientError();
  }

  auto response_dto = recipe_service_.GetRecipes(lastId, limit);

  auto response_json =
      userver::formats::json::ValueBuilder{response_dto}.ExtractValue();

  auto& response = request.GetHttpResponse();
  response.SetContentType(userver::http::content_type::kApplicationJson);
  response.SetStatus(userver::server::http::HttpStatus::kOk);
  return userver::formats::json::ToString(response_json);
}
}  // namespace user