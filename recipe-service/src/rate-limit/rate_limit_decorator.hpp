#pragma once

#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/server/middlewares/http_middleware_base.hpp>

#include "../service/cache/rate_limit_cache_service.hpp"

namespace ratelimit {

class RateLimitDecorator final
    : public userver::server::middlewares::HttpMiddlewareBase {
 public:
  static constexpr std::string_view kName = "rate-limit-decorator";

  RateLimitDecorator(
      const userver::server::handlers::HttpHandlerBase& handler,
      userver::yaml_config::YamlConfig config,
      cache::services::RateLimitCacheService& rate_limit_cache_service);

  void HandleRequest(
      userver::server::http::HttpRequest& request,
      userver::server::request::RequestContext& context) const override;

 private:
  cache::services::RateLimitCacheService& rate_limit_cache_service_;

  const long limit_;

  inline static userver::http::headers::PredefinedHeader kRateLimitLimitHeader{
      "X-RateLimit-Limit"};
  inline static userver::http::headers::PredefinedHeader
      kRateLimitRemainingHeader{"X-RateLimit-Remaining"};
  inline static userver::http::headers::PredefinedHeader kRateLimitResetHeader{
      "X-RateLimit-Reset"};
};

}  // namespace ratelimit