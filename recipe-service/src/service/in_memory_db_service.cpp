#include "in_memory_db_service.hpp"
#include <userver/server/handlers/exceptions.hpp>
namespace recipe {
InMemoryDbService::InMemoryDbService(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : ComponentBase(config, context) {
  ingredients_.emplace(1, entity::Ingredient{1, "Sugar"});
  ingredients_.emplace(2, entity::Ingredient{2, "Salt"});
  ingredients_.emplace(3, entity::Ingredient{3, "Flour"});
  ingredients_.emplace(4, entity::Ingredient{4, "Butter"});
  ingredients_.emplace(5, entity::Ingredient{5, "Eggs"});
}

entity::Recipe InMemoryDbService::CreateRecipe(const entity::Recipe& recipe) {
  for (const auto& ingredient : recipe.ingredients) {
    if (ingredients_.contains(ingredient.id) == false) {
      throw userver::server::handlers::ClientError();
    }
  }
  int64_t id = ++next_recipe_id_;

  auto created_recipe = entity::Recipe(recipe);
  created_recipe.id = id;
  recipes_[id] = created_recipe;
  for (const auto& ingredient : recipe.ingredients) {
    recipe_ingredients_.push_back({id, ingredient.id, ingredient.amount, ingredient.unit});
  }
  return created_recipe;
}

std::vector<entity::IngredientForRecipe>
InMemoryDbService::GetRecipeIngredients(std::int64_t recipe_id) const {
  std::vector<entity::IngredientForRecipe> ingredients;
  std::vector<entity::RecipeIngredient> recipe_ingredients;

  for (const auto& recipe_ingredient : recipe_ingredients_) {
    if (recipe_ingredient.recipe_id == recipe_id) {
      auto ingredient_it =
          ingredients_.find(recipe_ingredient.ingredient_id);
      if (ingredient_it != ingredients_.end()) {
        entity::IngredientForRecipe ingredient_for_recipe;
        ingredient_for_recipe.id = recipe_ingredient.ingredient_id;
        ingredient_for_recipe.name = ingredient_it->second.name;
        ingredient_for_recipe.amount = recipe_ingredient.amount;
        ingredient_for_recipe.unit = recipe_ingredient.unit;

        ingredients.push_back(std::move(ingredient_for_recipe));
      }
    }
  }

  return ingredients;
}

std::vector<entity::Recipe> InMemoryDbService::GetRecipes(
    std::int64_t last_id, std::int64_t limits) const {
  std::vector<entity::Recipe> recipes;
  for (const auto& recipe : recipes_) {
    if (recipe.first > last_id) {
      recipes.push_back(std::move(recipe.second));
    }
    if (recipes.size() >= limits) {
      return recipes;
    }
  }

  return recipes;
}

}  // namespace recipe