#include "postgres_db_service.hpp"
#include <userver/server/handlers/exceptions.hpp>
#include <userver/server/http/http_status.hpp>

namespace auth {
PostgresDbService::PostgresDbService(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : ComponentBase(config, context) {}

entity::User PostgresDbService::Save(const entity::User& user) const {
  throw userver::server::handlers::CustomHandlerException(
      userver::server::handlers::HandlerErrorCode::kUnknownError,
      userver::server::handlers::CustomHandlerException::ExternalBody{
          "Is not implemented"});
}

entity::UserWithRoles PostgresDbService::GetUserByEmail(const std::string& email) const {
  throw userver::server::handlers::CustomHandlerException(
      userver::server::handlers::HandlerErrorCode::kUnknownError,
      userver::server::handlers::CustomHandlerException::ExternalBody{
          "Is not implemented"});
}

}  // namespace auth