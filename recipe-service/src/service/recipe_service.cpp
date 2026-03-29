#include "recipe_service.hpp"

#include <userver/logging/log.hpp>
#include "user_service.hpp"

namespace recipe::services {
RecipeService::RecipeService(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : ComponentBase(config, context),
      db_service_(context.FindComponent<DbService>()) {}

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
  std::vector<entity::Recipe> recipes = db_service_.GetRecipes(last_id, limit);
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
      db_service_.GetRecipeIngredients(recipe_id);
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

}  // namespace recipe::services