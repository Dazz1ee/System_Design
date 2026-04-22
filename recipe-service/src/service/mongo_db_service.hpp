#pragma once

#include <userver/clients/http/client.hpp>
#include <userver/components/component.hpp>
#include <userver/components/component_base.hpp>
#include <userver/storages/mongo/component.hpp>
#include "entity/ingredient.hpp"
#include "entity/recipe.hpp"

namespace recipe {

class MongoDbService final : public userver::components::ComponentBase {
 public:
  static constexpr std::string_view kName = "mongo-db-service";

  MongoDbService(const userver::components::ComponentConfig& config,
                 const userver::components::ComponentContext& context);

  entity::MongoRecipe CreateRecipe(const entity::MongoRecipe& recipe) const;

  std::vector<entity::MongoRecipeIngredient> GetRecipeIngredients(
      const std::string& recipe_id) const;

  std::vector<entity::MongoRecipe> GetRecipes(std::string& last_id,
                                              int limit) const;

 private:
  const userver::storages::mongo::PoolPtr mongo_;
};
}  // namespace recipe