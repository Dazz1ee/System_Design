#include "pg_ingredient_cache_service.hpp"

#include <userver/formats/bson/inline.hpp>
#include <userver/server/handlers/exceptions.hpp>
#include <userver/storages/redis/component.hpp>

#include "service/mongo_db_service.hpp"

namespace cache::services {

using userver::formats::bson::MakeArray;
using userver::formats::bson::MakeDoc;

PgIngredientCacheService::PgIngredientCacheService(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : ComponentBase(config, context),
      LoadingCacheService(
          context
              .FindComponent<userver::components::Redis>("redis-database")
              .GetClient("ingredient"),
          {std::chrono::seconds{15}, std::chrono::seconds{60}, 4}) {}

}  // namespace cache::services