#pragma once

#include <userver/clients/http/client.hpp>
#include <userver/components/component.hpp>
#include <userver/components/component_base.hpp>
#include <userver/storages/postgres/component.hpp>
#include "entity/ingredient.hpp"
#include "entity/recipe.hpp"

namespace recipe {

class PostgresDbService final : public userver::components::ComponentBase {
 public:
  static constexpr std::string_view kName = "postgres-db-service";

  PostgresDbService(const userver::components::ComponentConfig& config,
                    const userver::components::ComponentContext& context);

  entity::Recipe CreateRecipe(const entity::Recipe& recipe) const;

  std::vector<entity::IngredientForRecipe> GetRecipeIngredients(
      std::int64_t id) const;

  std::vector<entity::Recipe> GetRecipes(std::int64_t lastId,
                                         std::int64_t limit) const;
};

}  // namespace recipe