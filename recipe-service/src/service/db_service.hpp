#pragma once

#include <userver/components/component.hpp>
#include <userver/components/component_base.hpp>
#include <userver/yaml_config/schema.hpp>

#include "entity/ingredient.hpp"
#include "entity/recipe.hpp"
#include "in_memory_db_service.hpp"
#include "postgers_db_service.hpp"

namespace recipe {

class DbService final : public userver::components::ComponentBase {
 public:
  static constexpr std::string_view kName = "db-service";

  DbService(const userver::components::ComponentConfig& config,
            const userver::components::ComponentContext& context);

  entity::Recipe CreateRecipe(const entity::Recipe& recipe) const;

  std::vector<entity::IngredientForRecipe> GetRecipeIngredients(
      std::int64_t id) const;

  std::vector<entity::Recipe> GetRecipes(std::int64_t lastId,
                                         std::int64_t limit) const;

  static userver::yaml_config::Schema GetStaticConfigSchema();

 private:
  bool enablePostgres_ = false;
  InMemoryDbService& inMemoryDbService_;
  const PostgresDbService& postgresDbService_;
};

}  // namespace recipe