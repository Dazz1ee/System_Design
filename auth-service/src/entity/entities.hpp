#pragma once

#include <string>
#include <chrono>
#include <vector>

namespace auth::entity {
struct User {
  std::int64_t id;
  std::string forename;
  std::string surname;
  std::string email;
  std::string password;
  std::chrono::system_clock::time_point created_at;
};

struct Role {
  std::int64_t id;
  std::string name;
  std::chrono::system_clock::time_point created_at;
};

struct UserWithRoles {
  std::int64_t id;
  std::string forename;
  std::string surname;
  std::string email;
  std::string password;
  std::chrono::system_clock::time_point created_at;
  std::vector<Role> roles;
};

struct UserRoles {
  std::int64_t user_id;
  std::int64_t role_id;
};
enum RoleEnum {
  ROLE_ADMIN,
  ROLE_USER
};
inline std::string ToString(RoleEnum role) {
  switch (role) {
    case RoleEnum::ROLE_ADMIN: return "admin";
    case RoleEnum::ROLE_USER: return "user";
  }
  return "user";
}
}