#pragma once

#include <string>
#include <optional>

namespace recipe::entity {
struct Ingredient {
  int64_t id;
  std::string name;
};

inline auto constexpr kIngredientMapping = [](auto& i) {
  return std::tie(i.id, i.name);
};

struct IngredientForRecipe {
  int64_t id;
  std::string name;
  std::optional<float> amount;
  std::optional<std::string> unit;
};

inline auto constexpr kIngredientForRecipeMapping = [](auto& i) {
  return std::tie(i.id, i.name, i.amount, i.unit);
};

}  // namespace recipe