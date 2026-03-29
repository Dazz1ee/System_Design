#pragma once

#include <userver/components/component.hpp>
#include <userver/components/component_base.hpp>
#include <userver/yaml_config/schema.hpp>

#include "entity/entities.hpp"
#include "in_memory_db_service.hpp"
#include "postgres_db_service.hpp"

namespace auth {

class DbService final : public userver::components::ComponentBase {
public:
  static constexpr std::string_view kName = "db-service";

  DbService(const userver::components::ComponentConfig& config,
            const userver::components::ComponentContext& context);

  entity::User Save(const entity::User& user) const;

  entity::UserWithRoles GetUserByEmail(const std::string& email) const;

  static userver::yaml_config::Schema GetStaticConfigSchema();

private:
  bool enablePostgres_ = false;
  InMemoryDbService& inMemoryDbService_;
  const PostgresDbService& postgresDbService_;
};

}  // namespace recipe