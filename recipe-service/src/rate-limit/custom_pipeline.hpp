#pragma once

#include <userver/server/middlewares/configuration.hpp>

#include "rate_limit_decorator.hpp"

namespace ratelimit {
class CustomHandlerPipelineBuilder final
    : public userver::server::middlewares::HandlerPipelineBuilder
{
 public:
  using HandlerPipelineBuilder::HandlerPipelineBuilder;

  userver::server::middlewares::MiddlewaresList BuildPipeline(
      userver::server::middlewares::MiddlewaresList server_middleware_pipeline)
      const override;
};

}  // namespace ratelimit