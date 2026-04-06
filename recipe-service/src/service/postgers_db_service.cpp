#include "postgers_db_service.hpp"
#include <userver/server/handlers/exceptions.hpp>
#include <userver/storages/postgres/cluster.hpp>

#include "handler/get_ingredients/handler.hpp"

namespace recipe {
PostgresDbService::PostgresDbService(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : ComponentBase(config, context),
      cluster_(
          context.FindComponent<userver::components::Postgres>("postgres-db-1")
              .GetCluster()) {}

void CreateRecipeSteps(userver::storages::postgres::Transaction& transaction,
                       const entity::Recipe& recipe, const std::int64_t& recipe_id) {
  const std::vector<std::string> steps = recipe.steps;

  std::vector<int64_t> recipe_ids;
  std::vector<int64_t> step_numbers;
  std::vector<std::string> step_descriptions;
  for (int i = 0; i < steps.size(); ++i) {
    recipe_ids.push_back(recipe_id);
    step_numbers.push_back(i + 1);
    step_descriptions.push_back(steps[i]);
  }

  transaction.Execute(
      "INSERT INTO recipe.recipe_steps (recipe_id, step_number, description) "
      "SELECT * FROM UNNEST($1, $2, $3) ",
      recipe_ids, step_numbers, step_descriptions);
}

void CreateRecipeIngredients(
    userver::storages::postgres::Transaction& transaction,
    const entity::Recipe& recipe, const std::int64_t& recipe_id) {
  const std::vector<entity::IngredientForRecipe> ingredients =
      recipe.ingredients;

  std::vector<int64_t> recipe_ids;
  std::vector<int64_t> ingredients_ids;
  std::vector<std::string> names;
  std::vector<std::optional<float>> amounts;
  std::vector<std::optional<std::string>> units;
  std::vector<std::optional<std::string>> step_numbers;

  for (const auto& ing : ingredients) {
    recipe_ids.push_back(recipe_id);
    ingredients_ids.push_back(ing.id);
    amounts.push_back(ing.amount);
    units.push_back(ing.unit);
  }

  auto create_recipe_ingredients = transaction.Execute(
      "INSERT INTO recipe.recipe_ingredients (recipe_id, "
      "ingredient_id, amount, unit) "
      "SELECT * FROM UNNEST($1, $2, $3, $4) ",
      recipe_ids, ingredients_ids, amounts, units);
}

entity::Recipe PostgresDbService::CreateRecipe(
    const entity::Recipe& recipe) const {
  auto transaction =
      cluster_->Begin(userver::storages::postgres::TransactionOptions{});

  std::chrono::system_clock::time_point created_at =
      std::chrono::system_clock::now();
  auto result = transaction.Execute(
      "INSERT INTO recipe.recipes (title, description, servings, "
      "cook_time_minutes, author_id, created_at) "
      "VALUES ($1, $2, $3, $4, $5, $6) "
      "RETURNING id",
      recipe.title, recipe.description, recipe.servings,
      recipe.cook_time_minutes, recipe.author_id,
      userver::storages::postgres::TimePointWithoutTz{created_at});

  std::int64_t recipe_id = result.AsSingleRow<std::int64_t>();
  CreateRecipeIngredients(transaction, recipe, recipe_id);
  CreateRecipeSteps(transaction, recipe, recipe_id);

  transaction.Commit();

  return {.id = recipe_id,
          .title = recipe.title,
          .description = recipe.description,
          .steps = recipe.steps,
          .servings = recipe.servings,
          .cook_time_minutes = recipe.cook_time_minutes,
          .author_id = recipe.author_id,
          .ingredients = recipe.ingredients,
          .created_at = created_at};
};

std::vector<entity::IngredientForRecipe>
PostgresDbService::GetRecipeIngredients(std::int64_t id) const {
  auto result = cluster_->Execute(
      userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
      "SELECT i.id AS id, i.name AS name, ri.amount AS amount, ri.unit AS unit "
      "FROM recipe.recipe_ingredients ri JOIN recipe.ingredients i ON i.id = "
      "ri.ingredient_id WHERE ri.recipe_id = $1",
      id);

  if (result.IsEmpty()) {
    throw userver::server::handlers::CustomHandlerException(
        userver::server::handlers::HandlerErrorCode::kResourceNotFound,
        userver::server::handlers::CustomHandlerException::ExternalBody{
            "Recipe not found"});
  }

  return result.AsContainer<std::vector<entity::IngredientForRecipe>>(
      userver::storages::postgres::kRowTag);
}

std::vector<entity::Recipe> PostgresDbService::GetRecipes(
    std::int64_t lastId, std::int64_t limit) const {
  auto result = cluster_->Execute(
      userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
      "SELECT id, title, description, servings, cook_time_minutes, author_id, "
      "created_at "
      "FROM recipe.recipes "
      "WHERE id > $1 "
      "ORDER BY id "
      "LIMIT $2",
      lastId, limit);

  std::vector<entity::Recipe> recipes;
  for (auto row : result) {
    auto [id, title, desc, serv, cook, auth, created] = row.As<
        int64_t,
        std::string,
        std::optional<std::string>,
        std::optional<int>,
        std::optional<int>,
        int64_t,
        std::chrono::system_clock::time_point
    >();

    entity::Recipe r;
    r.id = id;
    r.title = std::move(title);
    r.description = std::move(desc);
    r.servings = serv;
    r.cook_time_minutes = cook;
    r.author_id = auth;
    r.created_at = created;

    recipes.push_back(std::move(r));
  }

  return recipes;
}
}  // namespace recipe