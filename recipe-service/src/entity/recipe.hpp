#pragma once

#include <jwt-cpp/jwt.h>

#include <boost/container/container_fwd.hpp>
#include <chrono>
#include <optional>
#include <string>
#include <userver/chaotic/array.hpp>
#include <userver/chaotic/primitive.hpp>
#include <userver/formats/bson/types.hpp>
#include <userver/formats/bson/value.hpp>
#include <userver/formats/json/value.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/formats/parse/common_containers.hpp>
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

inline Recipe Parse(const userver::formats::json::Value& json,
                    userver::formats::parse::To<Recipe>) {
  return {json["id"].As<std::int64_t>(),
          json["title"].As<std::string>(),
          json["description"].As<std::string>(),
          json["steps"].As<std::vector<std::string>>(),
          json["servings"].As<std::optional<int>>(),
          json["cook_time_minutes"].As<std::optional<int>>(),
          json["author_id"].As<std::int64_t>(),
          json["ingredients"].As<std::vector<IngredientForRecipe>>(),
          json["created_at"].As<std::chrono::system_clock::time_point>()};
}

inline userver::formats::json::Value Serialize(
    const Recipe& recipe,
    userver::formats::serialize::To<userver::formats::json::Value>) {
  userver::formats::json::ValueBuilder builder;
  builder["id"] = recipe.id;
  builder["title"] = recipe.title;
  builder["description"] = recipe.description.value_or(nullptr);
  builder["steps"] = userver::chaotic::Array<
      USERVER_NAMESPACE::chaotic::Primitive<std::string>,
      std::vector<std::string>>{recipe.steps};
  builder["servings"] = recipe.servings.value_or(NULL);
  builder["cook_time_minutes"] = recipe.cook_time_minutes.value_or(NULL);
  builder["author_id"] = recipe.author_id;
  builder["ingredients"] = userver::chaotic::Array<
      USERVER_NAMESPACE::chaotic::Primitive<IngredientForRecipe>,
      std::vector<IngredientForRecipe>>{recipe.ingredients};
  builder["created_at"] = recipe.created_at;

  return builder.ExtractValue();
}

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
    const userver::formats::bson::Value& bson,
    userver::formats::parse::To<MongoRecipeIngredient>) {
  if (!bson.IsObject()) {
    return {};
  }
  return {bson["ingredient_id"].As<userver::formats::bson::Oid>().ToString(),
          bson["name"].As<std::string>(),
          bson["amount"].As<std::optional<double>>(),
          bson["unit"].As<std::optional<std::string>>()};
}

inline MongoRecipeIngredient Parse(
    const userver::formats::json::Value& json,
    userver::formats::parse::To<MongoRecipeIngredient>) {
  return {json["ingredient_id"].As<std::string>(),
          json["name"].As<std::string>(),
          json["amount"].As<std::optional<double>>(),
          json["unit"].As<std::optional<std::string>>()};
}

inline userver::formats::json::Value Serialize(
    const MongoRecipeIngredient& ingredient,
    userver::formats::serialize::To<userver::formats::json::Value>) {
  userver::formats::json::ValueBuilder builder;
  builder["ingredient_id"] = ingredient.ingredient_id;
  builder["name"] = ingredient.name;
  builder["amount"] = ingredient.amount.value_or(0.0);
  builder["unit"] = ingredient.unit.value_or("");
  return builder.ExtractValue();
}
struct RecipeStep {
  int step_number;
  std::string description;
};

inline RecipeStep Parse(const userver::formats::bson::Value& bson,
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

inline RecipeStep Parse(const userver::formats::json::Value& json,
                        userver::formats::parse::To<RecipeStep>) {
  return {json["step_number"].As<int>(), json["description"].As<std::string>()};
}

inline userver::formats::json::Value Serialize(
    const RecipeStep& step,
    userver::formats::serialize::To<userver::formats::json::Value>) {
  userver::formats::json::ValueBuilder builder;
  builder["step_number"] = step.step_number;
  builder["description"] = step.description;
  return builder.ExtractValue();
}

inline userver::formats::json::Value Serialize(
    const MongoRecipe& recipe,
    userver::formats::serialize::To<userver::formats::json::Value>) {
  userver::formats::json::ValueBuilder builder;
  builder["id"] = recipe.id;
  builder["title"] = recipe.title;
  builder["description"] = recipe.description.value_or(nullptr);
  builder["steps"] =
      userver::chaotic::Array<USERVER_NAMESPACE::chaotic::Primitive<RecipeStep>,
                              std::vector<RecipeStep>>{recipe.steps};
  builder["servings"] = recipe.servings.value_or(NULL);
  builder["cook_time_minutes"] = recipe.cook_time_minutes.value_or(NULL);
  builder["author_id"] = recipe.author_id;
  builder["ingredients"] = userver::chaotic::Array<
      USERVER_NAMESPACE::chaotic::Primitive<MongoRecipeIngredient>,
      std::vector<MongoRecipeIngredient>>{recipe.ingredients};
  builder["created_at"] = recipe.created_at;
  return builder.ExtractValue();
}

inline MongoRecipe Parse(const userver::formats::json::Value& json,
                         userver::formats::parse::To<MongoRecipe>) {
  return {
      json["id"].As<std::string>(),
      json["title"].As<std::string>(),
      json["description"].As<std::optional<std::string>>(),
      json["steps"].As<std::vector<RecipeStep>>(),
      json["servings"].As<std::optional<int>>(),
      json["cook_time_minutes"].As<std::optional<int>>(),
      json["author_id"].As<int64_t>(),
      json["ingredients"].As<std::vector<MongoRecipeIngredient>>(),
      json["created_at"].As<std::chrono::system_clock::time_point>(),
  };
}

inline MongoRecipe Parse(const userver::formats::bson::Value& bson,
                         userver::formats::parse::To<MongoRecipe>) {
  auto author_str = bson["author_id"].As<std::optional<std::string>>();
  return {
      bson["_id"].As<userver::formats::bson::Oid>().ToString(),
      bson["title"].As<std::string>(),
      bson["description"].As<std::optional<std::string>>(),
      bson["steps"].As<std::vector<RecipeStep>>(),
      bson["servings"].As<std::optional<int>>(),
      bson["cook_time_minutes"].As<std::optional<int>>(),
      author_str ?  std::stoll(author_str.value()) : 0,
      bson["ingredients"].As<std::vector<MongoRecipeIngredient>>(),
      bson["created_at"].As<std::chrono::system_clock::time_point>(),
  };
}

struct RecipeIngredient {
  int64_t recipe_id;
  int64_t ingredient_id;

  std::optional<float> amount;
  std::optional<std::string> unit;
};
}  // namespace recipe::entity