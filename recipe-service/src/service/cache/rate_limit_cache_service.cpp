#include "rate_limit_cache_service.hpp"

#include <userver/formats/bson/inline.hpp>
#include <userver/server/handlers/exceptions.hpp>
#include <userver/storages/redis/component.hpp>

#include "service/mongo_db_service.hpp"

namespace cache::services {

using userver::formats::bson::MakeArray;
using userver::formats::bson::MakeDoc;

RateLimitCacheService::RateLimitCacheService(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : ComponentBase(config, context),
      redis_client_(
          context.FindComponent<userver::components::Redis>("redis-database")
              .GetClient("rate-limit")),
      redis_cc_({std::chrono::seconds{15}, std::chrono::seconds{30}, 4}) {}

RateLimitDto RateLimitCacheService::Increase(const std::string& key) const {
  try {
    auto count = redis_client_->Incr(key, redis_cc_).Get();
    if (count == 1) {
      redis_client_->Expire(key, std::chrono::seconds(TTL_SECONDS), redis_cc_).Get();
    }

    auto ttl = redis_client_->Ttl(key, redis_cc_).Get();
    auto expire_seconds = ttl.GetExpire().count();

    return {count, expire_seconds};
  } catch (...) {
    return {0, 0};
  }
}

}  // namespace cache::services