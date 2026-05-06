#pragma once

#include <userver/clients/http/client.hpp>
#include <userver/components/component_base.hpp>
#include <userver/components/component.hpp>

#include "schemas/recipe.hpp"
// #include "../build-debug/src/schemas/recipe.hpp"
#include "cache/ingredient_cache_service.hpp"
#include "cache/pg_ingredient_cache_service.hpp"
#include "cache/pg_recipe_cache_service.hpp"
#include "cache/recipe_cache_service.hpp"
#include "db_service.hpp"
#include "mongo_db_service.hpp"

namespace recipe::services {

class RecipeService final : public userver::components::ComponentBase {
public:
  static constexpr std::string_view kName = "recipe-service";

  RecipeService(const userver::components::ComponentConfig& config,
                    const userver::components::ComponentContext& context);

  schemas::CreateRecipeResponseDTO CreateRecipe(
      const schemas::CreateRecipeRequestDTO& request,
      std::int64_t userId) const;

  schemas::GetRecipesResponseDTO GetRecipes(std::int64_t last_id, int64_t limit) const;

  schemas::GetIngredientsResponseDTO GetRecipeIngredients(std::int64_t recipe_id) const;

  schemas::CreateRecipeResponseV2DTO CreateRecipeV2(
    const schemas::CreateRecipeRequestV2DTO& request,
    std::int64_t userId) const;

  schemas::GetRecipesResponseV2DTO GetRecipesV2(std::string last_id, int64_t limit) const;

  schemas::GetIngredientsResponseV2DTO GetRecipeIngredientsV2(std::string recipe_id) const;

private:
  const DbService& db_service_;
  const MongoDbService& mongo_db_service_;
  const cache::services::IngredientCacheService& ingredient_cache_service_;
  const cache::services::RecipeCacheService& recipe_cache_service_;
  const cache::services::PgIngredientCacheService& pg_ingredient_cache_service_;
  const cache::services::PgRecipeCacheService& pg_recipe_cache_service_;
};

}  // namespace recipe