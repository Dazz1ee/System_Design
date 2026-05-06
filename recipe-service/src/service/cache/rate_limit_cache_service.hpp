#pragma once

#include "loading_cache_service.hpp"

#include <userver/components/component.hpp>
#include <userver/components/component_base.hpp>

#include "dto/rate_limit_dto.hpp"
#include "entity/recipe.hpp"
#include "service/mongo_db_service.hpp"

namespace cache::services {

class RateLimitCacheService final : public userver::components::ComponentBase {
 public:
  static constexpr std::string_view kName = "rate-limit-cache-service";

  RateLimitCacheService(const userver::components::ComponentConfig& config,
                        const userver::components::ComponentContext& context);

  RateLimitDto Increase(const std::string& key) const;

 private:
  userver::storages::redis::ClientPtr redis_client_;
  userver::storages::redis::CommandControl redis_cc_;
  inline static const int TTL_SECONDS = 60;
};
}  // namespace cache::services