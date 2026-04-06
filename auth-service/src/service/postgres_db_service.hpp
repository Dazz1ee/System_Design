#pragma once

#include <userver/clients/http/client.hpp>
#include <userver/components/component.hpp>
#include <userver/components/component_base.hpp>
#include <userver/storages/postgres/component.hpp>
#include "entity/entities.hpp"

namespace auth {

class PostgresDbService final : public userver::components::ComponentBase {
 public:
  static constexpr std::string_view kName = "postgres-db-service";

  PostgresDbService(const userver::components::ComponentConfig& config,
                    const userver::components::ComponentContext& context);

  entity::User Save(const entity::User& user) const;

  entity::UserWithRoles GetUserByEmail(const std::string& email) const;

 private:
  const userver::storages::postgres::ClusterPtr cluster_;
};

}  // namespace auth