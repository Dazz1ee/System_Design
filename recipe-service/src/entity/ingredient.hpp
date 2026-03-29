#pragma once

#include <string>
#include <optional>

namespace recipe::entity {
struct Ingredient {
  int64_t id;
  std::string name;
};

struct IngredientForRecipe {
  int64_t id;
  std::string name;
  std::optional<float> amount;
  std::optional<std::string> unit;
};
}  // namespace recipe