#pragma once

#include <userver/formats/json/serialize.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/formats/serialize/common_containers.hpp>
#include <userver/storages/redis/client.hpp>
#include <userver/yaml_config/schema.hpp>

namespace cache::services {

template <typename V>
class LoadingCacheService {
 public:
  LoadingCacheService(userver::storages::redis::ClientPtr redis_client_,
                      userver::storages::redis::CommandControl redis_cc_)
      : redis_client_(std::move(redis_client_)), redis_cc_(redis_cc_) {}

  virtual ~LoadingCacheService() = default;

  template <typename Func>
  V Get(const std::string& key, Func&& factory) const {
    const auto result = redis_client_->Get(key, redis_cc_).Get();
    if (result) {
      return userver::formats::json::FromString(*result).As<V>();
    }

    auto value = factory();
    const auto value_json =
        userver::formats::json::ValueBuilder{value}.ExtractValue();
    const auto set_result =
        redis_client_->Set(key, ToString(value_json),
                           std::chrono::seconds(TTL_SECONDS), redis_cc_);

    return value;
  }

 private:
  userver::storages::redis::ClientPtr redis_client_;
  userver::storages::redis::CommandControl redis_cc_;
  inline static const int TTL_SECONDS = 180;
};

}  // namespace cache::services