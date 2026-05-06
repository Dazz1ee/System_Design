#include "rate_limit_decorator.hpp"

namespace ratelimit {
RateLimitDecorator::RateLimitDecorator(
    const userver::server::handlers::HttpHandlerBase&,
    userver::yaml_config::YamlConfig config,
    cache::services::RateLimitCacheService& rate_limit_cache_service)
    : rate_limit_cache_service_(rate_limit_cache_service),
      limit_{config["rate-limit-value"].As<long>()} {}

void RateLimitDecorator::HandleRequest(
    userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext& context) const {
  auto path = request.GetRequestPath();
  auto method = request.GetMethodStr();
  auto client_ip = request.GetHeader("x-real-ip");
  auto cache_response = rate_limit_cache_service_.Increase(path + method + client_ip);
  auto count = cache_response.count;

  auto& response = request.GetHttpResponse();

  long remaining = std::max(0l, limit_ - count);
  response.SetHeader(static_cast<std::string>(kRateLimitLimitHeader),
                     std::to_string(limit_));
  response.SetHeader(static_cast<std::string>(kRateLimitRemainingHeader),
                     std::to_string(remaining));
  response.SetHeader(static_cast<std::string>(kRateLimitResetHeader),
                     std::to_string(cache_response.expire_seconds));

  if (count > limit_) {
    response.SetStatus(userver::server::http::HttpStatus::kTooManyRequests);
    return;
  }
  Next(request, context);
};

}  // namespace ratelimit