#include "rate_limit_middleware_factory.hpp"

namespace ratelimit {
RateLimitMiddlewareFactory::RateLimitMiddlewareFactory(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpMiddlewareFactoryBase(config, context),
      rate_limit_cache_service_(
          context.FindComponent<cache::services::RateLimitCacheService>()) {}

std::unique_ptr<userver::server::middlewares::HttpMiddlewareBase> RateLimitMiddlewareFactory::Create(
    const userver::server::handlers::HttpHandlerBase& handler,
    userver::yaml_config::YamlConfig middleware_config) const {
  return std::make_unique<RateLimitDecorator>(
      handler, std::move(middleware_config), rate_limit_cache_service_);
}

userver::yaml_config::Schema
RateLimitMiddlewareFactory::GetMiddlewareConfigSchema() const {
  return userver::formats::yaml::FromString(R"(
type: object
description: Config for this particular middleware
additionalProperties: false
properties:
    rate-limit-value:
        type: integer
        description: limit value
)")
      .As<userver::yaml_config::Schema>();
}

}  // namespace ratelimit