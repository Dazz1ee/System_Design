#include "mongo_db_service.hpp"

#include <userver/formats/bson/inline.hpp>
#include <userver/server/handlers/exceptions.hpp>
#include <userver/storages/mongo/component.hpp>
#include <userver/formats/serialize/common_containers.hpp>

namespace recipe {

using userver::formats::bson::MakeArray;
using userver::formats::bson::MakeDoc;

MongoDbService::MongoDbService(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : ComponentBase(config, context),
      mongo_(context.FindComponent<userver::components::Mongo>("mongo-db")
                 .GetPool()) {}

entity::MongoRecipe MongoDbService::CreateRecipe(
    const entity::MongoRecipe& recipe) const {
  auto recipes_col = mongo_->GetCollection("recipes");
  auto ingredients_col = mongo_->GetCollection("ingredients");

  auto created_at = std::chrono::system_clock::now();

  userver::formats::bson::ValueBuilder steps_builder(
      userver::formats::common::Type::kArray);
  for (const auto& step : recipe.steps) {
    steps_builder.PushBack(MakeDoc("step_number", step.step_number,
                                   "description", step.description));
  }

  userver::formats::bson::ValueBuilder ingredients_builder(
      userver::formats::common::Type::kArray);
  for (const auto& ingredient : recipe.ingredients) {
    ingredients_builder.PushBack(
        MakeDoc("ingredient_id", userver::formats::bson::Oid{ingredient.ingredient_id},
                "amount", static_cast<double>(ingredient.amount.value_or(0.0f)),
                "unit", ingredient.unit.value_or("")));
  }

  userver::formats::bson::ValueBuilder ids_builder(
      userver::formats::common::Type::kArray);

  for (const auto& ing : recipe.ingredients) {
    userver::formats::bson::Oid oid{ing.ingredient_id};
    ids_builder.PushBack(
        userver::formats::bson::ValueBuilder(oid).ExtractValue());
  }

  auto cursor = ingredients_col.Find(
      MakeDoc(
          "_id",
          MakeDoc("$in", ids_builder.ExtractValue())
      )
  );

  std::unordered_map<std::string, std::string> name_map;
  for (const auto& doc : cursor) {
    name_map[doc["_id"].As<userver::formats::bson::Oid>().ToString()] =
        doc["name"].As<std::string>();
  }

  userver::formats::bson::ValueBuilder ing_builder(userver::formats::common::Type::kArray);
  for (const auto& ingredient : recipe.ingredients) {
    if (!name_map.contains(ingredient.ingredient_id)) {
      continue;
    }

    ing_builder.PushBack(MakeDoc(
        "ingredient_id", userver::formats::bson::Oid{ingredient.ingredient_id},
        "name", name_map[ingredient.ingredient_id],
        "amount", ingredient.amount.value_or(0.0),
        "unit", ingredient.unit.value_or("")
    ));
  }
  userver::formats::bson::Oid oid;
  auto doc =
      MakeDoc("_id", oid, "title", recipe.title, "description",
              recipe.description ? *recipe.description : "", "servings",
              recipe.servings.value_or(1), "cook_time_minutes",
              recipe.cook_time_minutes.value_or(0), "author_id",
              std::to_string(recipe.author_id), "created_at", created_at,
              "version", 1, "steps", steps_builder, "ingredients", ing_builder);

  auto result = recipes_col.InsertOne(doc);

  entity::MongoRecipe response = recipe;
  response.id = oid.ToString();
  response.created_at = created_at;

  return response;
}

std::vector<entity::MongoRecipe> MongoDbService::GetRecipes(std::string& last_id,
                                         int limit) const {
  auto recipes_col = mongo_->GetCollection("recipes");

  userver::formats::bson::ValueBuilder query_builder(
      userver::formats::common::Type::kObject);

  if (!last_id.empty()) {
    query_builder["_id"] =
        MakeDoc("$gt", userver::formats::bson::Oid{last_id});
  }

  auto filter = query_builder.ExtractValue();

  namespace mongo_opts = userver::storages::mongo::options;
  auto cursor = recipes_col.Find(
      filter,
      mongo_opts::Sort({{"_id", mongo_opts::Sort::Direction::kAscending}}),
      mongo_opts::Limit{static_cast<size_t>(limit)});


  std::vector<entity::MongoRecipe> recipes;
  for (const auto& doc : cursor) {
    entity::MongoRecipe entity;
    entity.id = doc["_id"].As<userver::formats::bson::Oid>().ToString();
    entity.title = doc["title"].As<std::string>();
    entity.created_at = doc["created_at"].As<std::chrono::system_clock::time_point>();

    entity.description = doc["description"].As<std::optional<std::string>>();
    entity.servings = doc["servings"].As<std::optional<int>>();
    entity.cook_time_minutes = doc["cook_time_minutes"].As<std::optional<int>>();

    entity.steps = doc["steps"].As<std::vector<entity::RecipeStep>>({});
    entity.ingredients = doc["ingredients"].As<std::vector<entity::MongoRecipeIngredient>>({});

    auto author_str = doc["author_id"].As<std::optional<std::string>>();
    entity.author_id = author_str ? std::stoll(author_str.value()) : 0;

    recipes.push_back(std::move(entity));
  }

  return recipes;
}

std::vector<entity::MongoRecipeIngredient>
MongoDbService::GetRecipeIngredients(const std::string& recipe_id) const {
  auto collection = mongo_->GetCollection("recipes");

  auto filter = MakeDoc(
      "_id", userver::formats::bson::Oid{recipe_id}
  );

  userver::storages::mongo::options::Projection projection;
  projection.Include("ingredients");

  auto doc = collection.FindOne(filter, projection);

  if (!doc) {
    throw userver::server::handlers::CustomHandlerException(
        userver::server::handlers::HandlerErrorCode::kResourceNotFound,
        userver::server::handlers::CustomHandlerException::ExternalBody{"Recipe not found"});
  }

  return (*doc)["ingredients"].As<std::vector<entity::MongoRecipeIngredient>>();
}

}  // namespace recipe