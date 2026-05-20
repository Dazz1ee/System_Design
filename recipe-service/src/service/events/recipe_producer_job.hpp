#pragma once

#include <userver/components/component.hpp>
#include <userver/components/component_base.hpp>
#include <userver/utils/periodic_task.hpp>

#include <userver/storages/mongo/pool.hpp>
#include <userver/urabbitmq/client.hpp>

#include "service/mongo_db_service.hpp"

namespace recipe {

class RecipeProducerJob final : public userver::components::ComponentBase {
public:
  static constexpr std::string_view kName = "recipe-producer-job";

  RecipeProducerJob(
      const userver::components::ComponentConfig& config,
      const userver::components::ComponentContext& context);

  ~RecipeProducerJob() override;

  void OnAllComponentsLoaded() override;

private:
  void SendCreateRecipeEvent() const;

  const MongoDbService& mongo_db_service_;
  const std::shared_ptr<userver::urabbitmq::Client> rabbit_;
  userver::utils::PeriodicTask periodic_task_;
  userver::urabbitmq::Exchange exchange_;
  userver::utils::PeriodicTask queue_;
};

}  // namespace recipe