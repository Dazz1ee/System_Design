#include "custom_pipeline.hpp"
#include <userver/server/middlewares/configuration.hpp>

#include "rate_limit_decorator.hpp"

namespace ratelimit {

userver::server::middlewares::MiddlewaresList
CustomHandlerPipelineBuilder::BuildPipeline(
    userver::server::middlewares::MiddlewaresList server_middleware_pipeline)
    const {
  // We could do any kind of transformation here.
  // For the sake of example (and what we assume to be the most common case),
  // we just add some middleware to the pipeline.
  auto& pipeline = server_middleware_pipeline;
  pipeline.emplace_back(ratelimit::RateLimitDecorator::kName);

  return pipeline;
};
}  // namespace ratelimit