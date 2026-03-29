#include "postgers_db_service.hpp"
#include <userver/server/handlers/exceptions.hpp>

namespace recipe {
PostgresDbService::PostgresDbService(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : ComponentBase(config, context) {}

entity::Recipe PostgresDbService::CreateRecipe(
    const entity::Recipe& recipe) const {
  throw userver::server::handlers::CustomHandlerException(
      userver::server::handlers::HandlerErrorCode::kUnknownError,
      userver::server::handlers::CustomHandlerException::ExternalBody{
          "Is not implemented"});
};

std::vector<entity::IngredientForRecipe>
PostgresDbService::GetRecipeIngredients(std::int64_t id) const {
  throw userver::server::handlers::CustomHandlerException(
      userver::server::handlers::HandlerErrorCode::kUnknownError,
      userver::server::handlers::CustomHandlerException::ExternalBody{
          "Is not implemented"});
}

std::vector<entity::Recipe> PostgresDbService::GetRecipes(
    std::int64_t lastId, std::int64_t limit) const {
  throw userver::server::handlers::CustomHandlerException(
      userver::server::handlers::HandlerErrorCode::kUnknownError,
      userver::server::handlers::CustomHandlerException::ExternalBody{
          "Is not implemented"});
}
}  // namespace recipe