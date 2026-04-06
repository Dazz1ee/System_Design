#include "in_memory_db_service.hpp"

#include <userver/server/handlers/exceptions.hpp>

namespace auth {
InMemoryDbService::InMemoryDbService(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : ComponentBase(config, context) {
  auto now = std::chrono::system_clock::now();

  roles_.emplace(
      1, entity::Role{
             .id = 0, .name = ToString(entity::ROLE_ADMIN), .created_at = now});
  roles_.emplace(
      2, entity::Role{
             .id = 1, .name = ToString(entity::ROLE_USER), .created_at = now});

  users_.emplace(1, entity::User{.id = 0,
                                 .forename = "admin",
                                 .surname = "admin",
                                 .email = "admin@example.com",
                                 .password = "admin123",
                                 .created_at = now});

  user_roles_.push_back(entity::UserRoles{.user_id = 0, .role_id = 1});

  next_user_id_ = 2;
}

entity::User InMemoryDbService::Save(const entity::User& user) {
  int64_t id = ++next_user_id_;
  auto new_user = user;
  new_user.id = id;
  new_user.created_at = std::chrono::system_clock::now();

  for (const auto& pair : users_) {
    if (pair.second.email == user.email) {
      throw userver::server::handlers::ConflictError();
    }
  }

  users_[id] = std::move(new_user);
  for (const auto& role : roles_) {
    if (role.second.name == ToString(entity::ROLE_USER)) {
      user_roles_.push_back(
          entity::UserRoles{.user_id = id, .role_id = role.first});
      break;
    }
  }

  return new_user;
}

entity::UserWithRoles InMemoryDbService::GetUserByEmail(
    const std::string& email) const {
  for (const auto& user : users_) {
    if (user.second.email == email) {
      entity::UserWithRoles user_with_roles;
      user_with_roles.id = user.second.id;
      user_with_roles.email = user.second.email;
      user_with_roles.forename = user.second.forename;
      user_with_roles.surname = user.second.surname;
      user_with_roles.password = user.second.password;
      user_with_roles.created_at = user.second.created_at;
      for (const auto& role : user_roles_) {
        if (role.user_id == user.first) {
          user_with_roles.roles.push_back(roles_.at(role.role_id));
        }
      }
      return user_with_roles;
    }
  }
  throw userver::server::handlers::ResourceNotFound();
}
}  // namespace auth