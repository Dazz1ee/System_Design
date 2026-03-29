#pragma once

#include <userver/clients/http/client.hpp>
#include <userver/components/component.hpp>
#include <userver/components/component_base.hpp>

#include "entity/ingredient.hpp"
#include "entity/recipe.hpp"

namespace recipe {

class InMemoryDbService final : public userver::components::ComponentBase {
 public:
  static constexpr std::string_view kName = "in-memory-db-service";

  InMemoryDbService(const userver::components::ComponentConfig& config,
                    const userver::components::ComponentContext& context);

  entity::Recipe CreateRecipe(const entity::Recipe& recipe);

  std::vector<entity::IngredientForRecipe> GetRecipeIngredients(std::int64_t recipe_id) const;

  std::vector<entity::Recipe> GetRecipes(std::int64_t last_id, std::int64_t limit) const;

 private:
  std::unordered_map<int64_t, entity::Recipe> recipes_;
  std::unordered_map<int64_t, entity::Ingredient> ingredients_;
  std::vector<entity::RecipeIngredient> recipe_ingredients_;
  std::atomic<int64_t> next_recipe_id_ = {0};
};

}  // namespace recipe