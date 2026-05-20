#include <userver/clients/dns/component.hpp>
#include <userver/clients/http/component_list.hpp>
#include <userver/components/component_list.hpp>
#include <userver/components/minimal_server_component_list.hpp>
#include <userver/congestion_control/component.hpp>
#include <userver/server/handlers/ping.hpp>
#include <userver/server/handlers/server_monitor.hpp>
#include <userver/storages/mongo/component.hpp>
#include <userver/storages/secdist/component.hpp>
#include <userver/storages/secdist/provider_component.hpp>
#include <userver/testsuite/testsuite_support.hpp>
#include <userver/urabbitmq/component.hpp>

#include <userver/utils/daemon_run.hpp>

#include <auth/jwt_auth_factory.hpp>
#include <userver/storages/redis/component.hpp>

#include "auth/jwt_auth_checker.hpp"
#include "handler/create_recipe/handler.hpp"
#include "handler/create_recipe_v2/handler.hpp"
#include "handler/create_user/handler.hpp"
#include "handler/get_ingredients/handler.hpp"
#include "handler/get_ingredients_v2/handler.hpp"
#include "handler/get_recipes/handler.hpp"
#include "handler/get_recipes_v2/handler.hpp"
#include "handler/login/handler.hpp"
#include "rate-limit/custom_pipeline.hpp"
#include "rate-limit/rate_limit_middleware_factory.hpp"
#include "service/cache/ingredient_cache_service.hpp"
#include "service/cache/pg_ingredient_cache_service.hpp"
#include "service/cache/pg_recipe_cache_service.hpp"
#include "service/cache/recipe_cache_service.hpp"
#include "service/events/recipe_consumer.hpp"
#include "service/events/recipe_producer_job.hpp"
#include "service/recipe_service.hpp"
#include "service/user_service.hpp"

namespace ratelimit {
class CustomHandlerPipelineBuilder;
}
int main(int argc, char* argv[]) {
  userver::server::handlers::auth::RegisterAuthCheckerFactory<
      auth::jwt::JwtAuthCheckerFactory>();
  auto component_list =
      userver::components::MinimalServerComponentList()
          .Append<userver::server::handlers::ServerMonitor>()
          .Append<userver::server::handlers::Ping>()
          .AppendComponentList(userver::clients::http::ComponentList())
          .Append<userver::clients::dns::Component>()
          .Append<userver::congestion_control::Component>()
          .Append<recipe::InMemoryDbService>()
          .Append<recipe::PostgresDbService>()
          .Append<recipe::MongoDbService>()
          .Append<recipe::DbService>()
          .Append<recipe::services::RecipeService>()
          .Append<user::services::UserService>()
          .Append<auth::services::AuthServiceClient>()
          .Append<recipe::RecipeProducerJob>()
          .Append<recipe::RecipeCreatedConsumer>()
          .Append<cache::services::IngredientCacheService>()
          .Append<cache::services::RecipeCacheService>()
          .Append<cache::services::PgIngredientCacheService>()
          .Append<cache::services::PgRecipeCacheService>()
          .Append<auth::jwt::JwtAuthComponent>()
          .Append<recipe::all::Handler>()
          .Append<recipe::create::Handler>()
          .Append<recipe::ingredients::Handler>()
          .Append<recipe::all::v2::Handler>()
          .Append<recipe::create::v2::Handler>()
          .Append<recipe::ingredients::v2::Handler>()
          .Append<cache::services::RateLimitCacheService>()
          .Append<ratelimit::RateLimitMiddlewareFactory>()
          .Append<user::create::Handler>()
          .Append<user::Handler>()
          .Append<userver::components::TestsuiteSupport>()
          .Append<userver::components::Postgres>("postgres-db-1")
          .Append<userver::components::Mongo>("mongo-db")
          .Append<userver::components::Mongo>("mongo-write-db")
          .Append<userver::components::Redis>("redis-database")
          .Append<userver::components::RabbitMQ>("recipe-rabbit")
          .Append<userver::components::Secdist>()
          .Append<userver::components::DefaultSecdistProvider>()
          .Append<ratelimit::CustomHandlerPipelineBuilder>(
              "custom-handler-pipeline-builder");

  return userver::utils::DaemonMain(argc, argv, component_list);
}