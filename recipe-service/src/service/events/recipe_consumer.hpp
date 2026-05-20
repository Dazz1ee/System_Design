#pragma once

#include <userver/storages/postgres/cluster.hpp>
#include <userver/urabbitmq/consumer_component_base.hpp>

#include "service/mongo_db_service.hpp"

namespace recipe {

class RecipeCreatedConsumer final
    : public userver::urabbitmq::ConsumerComponentBase {
public:
  static constexpr std::string_view kName = "recipe-created-consumer";

  RecipeCreatedConsumer(
      const userver::components::ComponentConfig& config,
      const userver::components::ComponentContext& context);

protected:
  void Process(std::string message) override;
private:
  MongoDbService& mongo_db_service_;
};



}  // namespace recipe