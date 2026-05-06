#pragma once

#include "loading_cache_service.hpp"

#include <userver/components/component.hpp>
#include <userver/components/component_base.hpp>

#include "../../entity/recipe.hpp"

namespace cache::services {

class PgRecipeCacheService final
    : public userver::components::ComponentBase,
      public LoadingCacheService<
          std::vector<recipe::entity::Recipe>> {
 public:
  static constexpr std::string_view kName = "pg-recipe-cache-service";

  PgRecipeCacheService(const userver::components::ComponentConfig& config,
                         const userver::components::ComponentContext& context);

};
}  // namespace cache::services