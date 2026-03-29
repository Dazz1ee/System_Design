#include <userver/clients/dns/component.hpp>
#include <userver/clients/http/component_list.hpp>
#include <userver/components/component_list.hpp>
#include <userver/components/minimal_server_component_list.hpp>
#include <userver/congestion_control/component.hpp>
#include <userver/server/handlers/ping.hpp>

#include <userver/utils/daemon_run.hpp>

#include <auth/jwt_auth_factory.hpp>
#include "auth/jwt_auth_checker.hpp"
#include "handler/create_recipe/handler.hpp"
#include "handler/create_user/handler.hpp"
#include "handler/get_ingredients/handler.hpp"
#include "handler/get_recipes/handler.hpp"
#include "handler/login/handler.hpp"
#include "service/recipe_service.hpp"
#include "service/user_service.hpp"

int main(int argc, char* argv[]) {
  userver::server::handlers::auth::RegisterAuthCheckerFactory<
      auth::jwt::JwtAuthCheckerFactory>();
  auto component_list =
      userver::components::MinimalServerComponentList()
          .Append<userver::server::handlers::Ping>()
          .AppendComponentList(userver::clients::http::ComponentList())
          .Append<userver::clients::dns::Component>()
          .Append<userver::congestion_control::Component>()
          .Append<recipe::InMemoryDbService>()
          .Append<recipe::PostgresDbService>()
          .Append<recipe::DbService>()
          .Append<recipe::services::RecipeService>()
          .Append<user::services::UserService>()
          .Append<auth::services::AuthServiceClient>()
          .Append<auth::jwt::JwtAuthComponent>()
          .Append<recipe::all::Handler>()
          .Append<recipe::create::Handler>()
          .Append<recipe::ingredients::Handler>()
          .Append<user::create::Handler>()
          .Append<user::Handler>();

  return userver::utils::DaemonMain(argc, argv, component_list);
}