#pragma once

#include "loading_cache_service.hpp"

#include <userver/components/component.hpp>
#include <userver/components/component_base.hpp>
#include "entity/recipe.hpp"
#include "service/mongo_db_service.hpp"

namespace cache::services {

class IngredientCacheService final
    : public userver::components::ComponentBase,
      public LoadingCacheService<
          std::vector<recipe::entity::MongoRecipeIngredient>> {
 public:
  static constexpr std::string_view kName = "ingredient-cache-service";

  IngredientCacheService(const userver::components::ComponentConfig& config,
                         const userver::components::ComponentContext& context);
};
}  // namespace cache::services