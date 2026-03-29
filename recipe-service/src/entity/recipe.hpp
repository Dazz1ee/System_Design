#pragma once

#include <chrono>
#include <optional>
#include <string>
#include <vector>
#include "entity/ingredient.hpp"

namespace recipe::entity {

struct Recipe {
  int64_t id;

  std::string title;
  std::optional<std::string> description;
  std::vector<std::string> steps;
  std::optional<int> servings;
  std::optional<int> cook_time_minutes;
  int64_t author_id;
  std::vector<IngredientForRecipe> ingredients;
  std::chrono::system_clock::time_point created_at;
};

struct RecipeIngredient {
  int64_t recipe_id;
  int64_t ingredient_id;

  std::optional<float> amount;
  std::optional<std::string> unit;
};

}  // namespace recipe