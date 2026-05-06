#pragma once

#include "loading_cache_service.hpp"

#include <userver/components/component.hpp>
#include <userver/components/component_base.hpp>
#include "entity/recipe.hpp"
#include "service/mongo_db_service.hpp"

namespace cache::services {

class PgIngredientCacheService final
    : public userver::components::ComponentBase,
      public LoadingCacheService<
          std::vector<recipe::entity::IngredientForRecipe>> {
 public:
  static constexpr std::string_view kName = "pg-ingredient-cache-service";

  PgIngredientCacheService(const userver::components::ComponentConfig& config,
                         const userver::components::ComponentContext& context);
};
}  // namespace cache::services