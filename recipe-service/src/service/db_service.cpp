#include "db_service.hpp"

namespace recipe {
DbService::DbService(const userver::components::ComponentConfig& config,
                     const userver::components::ComponentContext& context)
    : ComponentBase(config, context),
      inMemoryDbService_(context.FindComponent<InMemoryDbService>()),
      postgresDbService_(context.FindComponent<PostgresDbService>()) {
  enablePostgres_ = config["enable_postgres"].As<bool>(false);
}

userver::yaml_config::Schema DbService::GetStaticConfigSchema() {
  return userver::yaml_config::impl::SchemaFromString(R"(
type: object
description: DB service
additionalProperties: false
properties:
    load-enabled:
        type: boolean
        description: whether to enable the component

    enable_postgres:
        type: boolean
        description: Use Postgres or in-memory DB
)");
}

entity::Recipe DbService::CreateRecipe(const entity::Recipe& recipe) const {
  if (enablePostgres_) {
    return postgresDbService_.CreateRecipe(recipe);
  }

  return inMemoryDbService_.CreateRecipe(recipe);
}

std::vector<entity::IngredientForRecipe> DbService::GetRecipeIngredients(
    std::int64_t recipe_id) const {
  if (enablePostgres_) {
    return postgresDbService_.GetRecipeIngredients(recipe_id);
  }

  return inMemoryDbService_.GetRecipeIngredients(recipe_id);
}

std::vector<entity::Recipe> DbService::GetRecipes(std::int64_t last_id,
                                                  std::int64_t limit) const {
  if (enablePostgres_) {
    return postgresDbService_.GetRecipes(last_id, limit);
  }

  return inMemoryDbService_.GetRecipes(last_id, limit);
};

}  // namespace recipe