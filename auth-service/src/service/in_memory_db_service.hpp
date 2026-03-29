#pragma once

#include <userver/clients/http/client.hpp>
#include <userver/components/component.hpp>
#include <userver/components/component_base.hpp>

#include "entity/entities.hpp"

namespace auth {

class InMemoryDbService final : public userver::components::ComponentBase {
 public:
  static constexpr std::string_view kName = "in-memory-db-service";

  InMemoryDbService(const userver::components::ComponentConfig& config,
                    const userver::components::ComponentContext& context);

  entity::User Save(const entity::User& user);

  entity::UserWithRoles GetUserByEmail(const std::string& email) const;

 private:
  std::unordered_map<std::int64_t, entity::User> users_;
  std::unordered_map<std::int64_t, entity::Role> roles_;
  std::vector<entity::UserRoles> user_roles_;
  std::atomic<std::int64_t> next_user_id_ = {0};
};

}  // namespace auth