#pragma once

#include <optional>
#include <string>
#include <userver/formats/json/value.hpp>
#include <userver/formats/json/value_builder.hpp>

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

inline IngredientForRecipe Parse(
    const userver::formats::json::Value& json,
    userver::formats::parse::To<IngredientForRecipe>) {
  return {json["id"].As<int64_t>(), json["name"].As<std::string>(),
          json["amount"].As<std::optional<double>>(),
          json["unit"].As<std::optional<std::string>>()};
}

inline userver::formats::json::Value Serialize(
    const IngredientForRecipe& recipe,
    userver::formats::serialize::To<userver::formats::json::Value>) {
  userver::formats::json::ValueBuilder builder;
  builder["id"] = recipe.id;
  builder["name"] = recipe.name;
  builder["amount"] = recipe.amount.value_or(NULL);
  builder["unit"] = recipe.unit.value_or(nullptr);

  return builder.ExtractValue();
}

struct MongoIngredient {
  std::string id;
  std::string name;
  std::optional<float> amount;
  std::optional<std::string> unit;
};

inline auto constexpr kIngredientForRecipeMapping = [](auto& i) {
  return std::tie(i.id, i.name, i.amount, i.unit);
};

}  // namespace recipe::entity