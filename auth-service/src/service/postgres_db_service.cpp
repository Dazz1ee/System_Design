#include "postgres_db_service.hpp"
#include <userver/server/handlers/exceptions.hpp>
#include <userver/storages/postgres/cluster.hpp>

namespace auth {
PostgresDbService::PostgresDbService(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : ComponentBase(config, context),
      cluster_(
          context.FindComponent<userver::components::Postgres>("postgres-db-1")
              .GetCluster()) {}

entity::User PostgresDbService::Save(const entity::User& user) const {
  std::chrono::system_clock::time_point created_at =
      std::chrono::system_clock::now();
  try {
    auto transaction =
        cluster_->Begin(userver::storages::postgres::TransactionOptions{});
    userver::storages::postgres::ResultSet result = transaction.Execute(
        "INSERT INTO auth.users (forename, surname, email, password, "
        "created_at) "
        "VALUES ($1, $2, $3, $4, $5) "
        "RETURNING id",
        user.forename, user.surname, user.email, user.password,
        userver::storages::postgres::TimePointWithoutTz{created_at});
    std::int64_t id = result.AsSingleRow<std::int64_t>();
    auto role_result = transaction.Execute(
        "INSERT INTO auth.user_roles (user_id, role_id) "
        "VALUES ($1, (SELECT id FROM auth.roles WHERE name = 'user')) ",
        id);

    transaction.Commit();
    return entity::User{id,         user.forename, user.surname,
                        user.email, user.password, created_at};
  } catch (const userver::storages::postgres::UniqueViolation& ex) {
    throw userver::server::handlers::CustomHandlerException(
        userver::server::handlers::HandlerErrorCode::kConflictState,
        userver::server::handlers::ExternalBody{
            "User with this email already exists"});
  }
}

entity::UserWithRoles PostgresDbService::GetUserByEmail(
    const std::string& email) const {
  auto result = cluster_->Execute(
      userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
      "SELECT u.id, u.forename, u.surname, u.email, u.password, u.created_at, "
      "       r.id AS role_id, r.name AS role_name, r.created_at AS "
      "role_created_at "
      "FROM auth.users u "
      "LEFT JOIN auth.user_roles ur ON u.id = ur.user_id "
      "LEFT JOIN auth.roles r ON ur.role_id = r.id "
      "WHERE u.email = $1",
      email);

  if (result.IsEmpty()) {
    throw userver::server::handlers::CustomHandlerException(
        userver::server::handlers::HandlerErrorCode::kUnauthorized,
        userver::server::handlers::CustomHandlerException::ExternalBody{
            "User not found"});
  }

  entity::UserWithRoles user_with_roles;
  bool user_initialized = false;

  for (const auto& row : result) {
    if (!user_initialized) {
      user_with_roles.id = row["id"].As<std::int64_t>();
      user_with_roles.forename = row["forename"].As<std::string>();
      user_with_roles.surname = row["surname"].As<std::string>();
      user_with_roles.email = row["email"].As<std::string>();
      user_with_roles.password = row["password"].As<std::string>();
      user_with_roles.created_at =
          row["created_at"].As<std::chrono::system_clock::time_point>();

      user_initialized = true;
    }

    if (!row["role_id"].IsNull()) {
      entity::Role role;
      role.id = row["role_id"].As<std::int64_t>();
      role.name = row["role_name"].As<std::string>();
      role.created_at =
          row["role_created_at"].As<std::chrono::system_clock::time_point>();

      user_with_roles.roles.push_back(std::move(role));
    }
  }

  if (!user_initialized) {
    throw userver::server::handlers::CustomHandlerException(
        userver::server::handlers::HandlerErrorCode::kResourceNotFound,
        userver::server::handlers::CustomHandlerException::ExternalBody{
            "User not found"});
  }

  return user_with_roles;
}

}  // namespace auth