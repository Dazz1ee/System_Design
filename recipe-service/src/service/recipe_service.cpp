#include "recipe_service.hpp"

#include <userver/logging/log.hpp>
#include "user_service.hpp"

namespace recipe::services {
RecipeService::RecipeService(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : ComponentBase(config, context),
      db_service_(context.FindComponent<DbService>()),
      mongo_db_service_(context.FindComponent<MongoDbService>()),
      ingredient_cache_service_(
          context.FindComponent<cache::services::IngredientCacheService>()),
      recipe_cache_service_(
          context.FindComponent<cache::services::RecipeCacheService>()),
      pg_ingredient_cache_service_(
          context.FindComponent<cache::services::PgIngredientCacheService>()),
      pg_recipe_cache_service_(
          context.FindComponent<cache::services::PgRecipeCacheService>()) {}

entity::Recipe BuildEntity(const schemas::CreateRecipeRequestDTO& dto,
                           int64_t author_id) {
  entity::Recipe entity;
  entity.id = 0;
  entity.title = dto.title;
  entity.description = dto.description;
  entity.servings = dto.servings;
  entity.cook_time_minutes = dto.cookTimeMinutes;
  entity.author_id = author_id;
  entity.created_at = std::chrono::system_clock::now();
  entity.steps = dto.steps;

  entity.ingredients.reserve(dto.ingredients.size());
  for (const auto& ingredient : dto.ingredients) {
    entity::IngredientForRecipe ingredient_entity;

    const auto& quantity = ingredient.quantity;
    ingredient_entity.id = ingredient.id;
    ingredient_entity.amount = quantity->amount;
    ingredient_entity.unit = quantity->unit;

    entity.ingredients.push_back(std::move(ingredient_entity));
  }

  return entity;
}

entity::MongoRecipe BuildMongoEntity(
    const schemas::CreateRecipeRequestV2DTO& dto, int64_t author_id) {
  entity::MongoRecipe entity;
  entity.title = dto.title;
  entity.description = dto.description;
  entity.servings = dto.servings;
  entity.cook_time_minutes = dto.cookTimeMinutes;
  entity.author_id = author_id;
  entity.created_at = std::chrono::system_clock::now();

  entity.steps.reserve(dto.steps.size());
  for (int i = 0; i < dto.steps.size(); ++i) {
    entity.steps.push_back({i, dto.steps[i]});
  }

  entity.ingredients.reserve(dto.ingredients.size());
  for (const auto& ingredient : dto.ingredients) {
    entity::MongoRecipeIngredient ingredient_entity;

    const auto& quantity = ingredient.quantity;
    ingredient_entity.ingredient_id = ingredient.id;
    ingredient_entity.amount = quantity->amount;
    ingredient_entity.unit = quantity->unit;

    entity.ingredients.push_back(std::move(ingredient_entity));
  }

  return entity;
}

schemas::CreateRecipeResponseDTO RecipeService::CreateRecipe(
    const schemas::CreateRecipeRequestDTO& request,
    std::int64_t user_id) const {
  entity::Recipe recipe = BuildEntity(request, user_id);
  const entity::Recipe& created_recipe = db_service_.CreateRecipe(recipe);
  schemas::CreateRecipeResponseDTO response;
  response.id = created_recipe.id;
  response.createdAt =
      USERVER_NAMESPACE::utils::datetime::TimePointTz(recipe.created_at);

  return response;
}

schemas::GetRecipesResponseDTO RecipeService::GetRecipes(
    std::int64_t last_id, std::int64_t limit) const {
  std::string cache_key = std::to_string(last_id) + std::to_string(limit);
  std::vector<entity::Recipe> recipes =
      pg_recipe_cache_service_.Get(cache_key, [this, &last_id, &limit] {
        return db_service_.GetRecipes(last_id, limit);
      });
  std::vector<schemas::RecipeResponseDTO> recipesResponse;
  recipesResponse.reserve(recipes.size());

  for (auto recipe : recipes) {
    schemas::RecipeResponseDTO dto;

    dto.id = recipe.id;
    dto.title = recipe.title;
    dto.description = recipe.description;
    dto.servings = recipe.servings;
    dto.cookTimeMinutes = recipe.cook_time_minutes;
    dto.steps = recipe.steps;
    dto.authorId = recipe.author_id;
    dto.createdAt =
        USERVER_NAMESPACE::utils::datetime::TimePointTz(recipe.created_at);

    recipesResponse.push_back(std::move(dto));
  }
  schemas::GetRecipesResponseDTO response;
  response.recipes = std::move(recipesResponse);
  return response;
}
schemas::GetIngredientsResponseDTO RecipeService::GetRecipeIngredients(
    std::int64_t recipe_id) const {
  std::vector<entity::IngredientForRecipe> ingredients =
      pg_ingredient_cache_service_.Get(
          std::to_string(recipe_id), [this, &recipe_id] {
            return db_service_.GetRecipeIngredients(recipe_id);
          });
  std::vector<schemas::IngredientDTO> ingredientsResponse;
  ingredientsResponse.reserve(ingredients.size());

  for (const auto& [id, name, amount, unit] : ingredients) {
    schemas::IngredientDTO ingredient_dto;

    ingredient_dto.id = id;
    ingredient_dto.name = name;
    ingredient_dto.amount = amount;
    ingredient_dto.unit = unit;

    ingredientsResponse.push_back(std::move(ingredient_dto));
  }

  schemas::GetIngredientsResponseDTO response;
  response.ingredients = std::move(ingredientsResponse);
  return response;
}

schemas::CreateRecipeResponseV2DTO RecipeService::CreateRecipeV2(
    const schemas::CreateRecipeRequestV2DTO& request,
    std::int64_t user_id) const {
  entity::MongoRecipe recipe = BuildMongoEntity(request, user_id);
  const entity::MongoRecipe& created_recipe =
      mongo_db_service_.CreateRecipeV2(recipe);
  schemas::CreateRecipeResponseV2DTO response;
  response.id = created_recipe.id;
  response.createdAt =
      USERVER_NAMESPACE::utils::datetime::TimePointTz(recipe.created_at);

  return response;
}

schemas::GetRecipesResponseV2DTO RecipeService::GetRecipesV2(
    std::string last_id, std::int64_t limit) const {
  std::string cache_key = last_id + std::to_string(limit);
  std::vector<entity::MongoRecipe> recipes =
      recipe_cache_service_.Get(cache_key, [this, &last_id, &limit] {
        return mongo_db_service_.GetRecipes(last_id, limit);
      });
  std::vector<schemas::RecipeResponseV2DTO> recipesResponse;
  recipesResponse.reserve(recipes.size());

  for (auto recipe : recipes) {
    schemas::RecipeResponseV2DTO dto;
    std::vector<std::string> steps;
    std::vector<entity::RecipeStep> mongoSteps = recipe.steps;
    std::sort(mongoSteps.begin(), mongoSteps.end(),
              [](const entity::RecipeStep& a, const entity::RecipeStep& b) {
                return a.step_number < b.step_number;
              });
    for (const auto& step : mongoSteps) {
      steps.push_back(step.description);
    }

    dto.id = recipe.id;
    dto.title = recipe.title;
    dto.description = recipe.description;
    dto.servings = recipe.servings;
    dto.cookTimeMinutes = recipe.cook_time_minutes;
    dto.steps = steps;
    dto.authorId = recipe.author_id;
    dto.createdAt =
        USERVER_NAMESPACE::utils::datetime::TimePointTz(recipe.created_at);

    recipesResponse.push_back(std::move(dto));
  }
  schemas::GetRecipesResponseV2DTO response;
  response.recipes = std::move(recipesResponse);
  return response;
}

schemas::GetIngredientsResponseV2DTO RecipeService::GetRecipeIngredientsV2(
    std::string recipe_id) const {
  std::vector<entity::MongoRecipeIngredient> ingredients =
      ingredient_cache_service_.Get(recipe_id, [this, &recipe_id] {
        return mongo_db_service_.GetRecipeIngredients(recipe_id);
      });
  std::vector<schemas::IngredientV2DTO> ingredientsResponse;
  ingredientsResponse.reserve(ingredients.size());

  for (const auto& [id, name, amount, unit] : ingredients) {
    schemas::IngredientV2DTO ingredient_dto;

    ingredient_dto.id = id;
    ingredient_dto.name = name;
    ingredient_dto.amount = amount;
    ingredient_dto.unit = unit;

    ingredientsResponse.push_back(std::move(ingredient_dto));
  }

  schemas::GetIngredientsResponseV2DTO response;
  response.ingredients = std::move(ingredientsResponse);
  return response;
}

}  // namespace recipe::services