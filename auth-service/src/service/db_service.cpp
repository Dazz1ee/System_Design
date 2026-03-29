#include "db_service.hpp"

namespace auth {
DbService::DbService(const userver::components::ComponentConfig& config,
                     const userver::components::ComponentContext& context)
    : ComponentBase(config, context),
      inMemoryDbService_(context.FindComponent<InMemoryDbService>()),
      postgresDbService_(context.FindComponent<PostgresDbService>()) {
  enablePostgres_ = config["enable_postgres"].As<bool>(false);
}

userver::yaml_config::Schema DbService::GetStaticConfigSchema() {
  return userver::yaml_config::impl::SchemaFromString(R"(
type: object
description: DB service
additionalProperties: false
properties:
    load-enabled:
        type: boolean
        description: whether to enable the component

    enable_postgres:
        type: boolean
        description: Use Postgres or in-memory DB
)");
}

entity::User DbService::Save(const entity::User& user) const {
  if (enablePostgres_) {
    return postgresDbService_.Save(user);
  }

  return inMemoryDbService_.Save(user);
}

entity::UserWithRoles DbService::GetUserByEmail(const std::string& email) const {
  if (enablePostgres_) {
    return postgresDbService_.GetUserByEmail(email);
  }

  return inMemoryDbService_.GetUserByEmail(email);
}

}  // namespace recipe