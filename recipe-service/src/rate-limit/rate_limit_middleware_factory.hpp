#pragma once

#include <userver/server/middlewares/http_middleware_base.hpp>
#include <userver/formats/yaml/serialize.hpp>

#include "../service/cache/rate_limit_cache_service.hpp"
#include "rate_limit_decorator.hpp"

namespace ratelimit {

class RateLimitMiddlewareFactory final
    : public userver::server::middlewares::HttpMiddlewareFactoryBase {
 public:
  static constexpr std::string_view kName{RateLimitDecorator::kName};

  RateLimitMiddlewareFactory(const userver::components::ComponentConfig& config, const userver::components::ComponentContext& context);


 private:
  std::unique_ptr<userver::server::middlewares::HttpMiddlewareBase> Create(
      const userver::server::handlers::HttpHandlerBase& handler,
      userver::yaml_config::YamlConfig middleware_config) const override;

  cache::services::RateLimitCacheService& rate_limit_cache_service_;

  userver::yaml_config::Schema GetMiddlewareConfigSchema() const override;
};
}  // namespace ratelimit