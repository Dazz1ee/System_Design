#pragma once

#include <chrono>
#include <iostream>
#include <optional>
#include <string>
#include <userver/formats/bson/types.hpp>
#include <userver/formats/bson/value.hpp>
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

inline auto constexpr kRecipeMapping = [](auto& i) {
  return std::tie(i.id, i.title, i.description, i.servings, i.cook_time_minutes,
                  i.author_id, i.created_at);
};

struct MongoRecipeIngredient {
  std::string ingredient_id;
  std::string name;
  std::optional<double> amount;
  std::optional<std::string> unit;
};

inline MongoRecipeIngredient Parse(
    userver::formats::bson::Value bson,
    userver::formats::parse::To<MongoRecipeIngredient>) {
  if (!bson.IsObject()) {
    return {};
  }
  return {
    bson["ingredient_id"].As<userver::formats::bson::Oid>().ToString(),
    bson["name"].As<std::string>("Unknown"),
    bson["amount"].As<std::optional<double>>(),
    bson["unit"].As<std::optional<std::string>>()
  };
}

struct RecipeStep {
  int step_number;
  std::string description;
};

inline RecipeStep Parse(userver::formats::bson::Value bson,
                 userver::formats::parse::To<RecipeStep>) {
  return {bson["step_number"].As<int>(), bson["description"].As<std::string>()};
}

struct MongoRecipe {
  std::string id;

  std::string title;
  std::optional<std::string> description;
  std::vector<RecipeStep> steps;
  std::optional<int> servings;
  std::optional<int> cook_time_minutes;
  int64_t author_id;
  std::vector<MongoRecipeIngredient> ingredients;
  std::chrono::system_clock::time_point created_at;
};

struct RecipeIngredient {
  int64_t recipe_id;
  int64_t ingredient_id;

  std::optional<float> amount;
  std::optional<std::string> unit;
};
}  // namespace recipe::entity