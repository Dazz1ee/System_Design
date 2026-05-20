#include "recipe_producer_job.hpp"

#include <chrono>

#include <userver/formats/bson/inline.hpp>
#include <userver/formats/json.hpp>

#include <userver/formats/bson/serialize.hpp>
#include <userver/formats/bson/types.hpp>
#include <userver/formats/bson/value.hpp>
#include <userver/logging/log.hpp>
#include <userver/storages/mongo/component.hpp>
#include <userver/urabbitmq/component.hpp>
#include <userver/urabbitmq/typedefs.hpp>

#include "entity/ingredient.hpp"
#include "entity/recipe.hpp"

namespace recipe {

using userver::formats::bson::MakeDoc;

RecipeProducerJob::RecipeProducerJob(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : ComponentBase(config, context),
      mongo_db_service_(context.FindComponent<MongoDbService>()),
      rabbit_(
          context.FindComponent<userver::components::RabbitMQ>("recipe-rabbit")
              .GetClient()) {
  periodic_task_.Start("recipe-producer-job", std::chrono::seconds(10),
                       [this] { SendCreateRecipeEvent(); });
}

RecipeProducerJob::~RecipeProducerJob() { periodic_task_.Stop(); }

void RecipeProducerJob::OnAllComponentsLoaded() {
  exchange_ = userver::urabbitmq::Exchange{"recipe.exchange"};
  auto queue = userver::urabbitmq::Queue{"recipe.queue"};

  rabbit_->DeclareExchange(
      exchange_, userver::urabbitmq::Exchange::Type::kDirect,
      userver::engine::Deadline::FromDuration(std::chrono::seconds(5)));

  rabbit_->DeclareQueue(
      userver::urabbitmq::Queue{"recipe.queue"}, {},
      userver::engine::Deadline::FromDuration(std::chrono::seconds(5)));

  rabbit_->BindQueue(
      exchange_, queue, "recipe.created",
      userver::engine::Deadline::FromDuration(std::chrono::seconds(5)));
}

void RecipeProducerJob::SendCreateRecipeEvent() const {
  try {
    auto cursor = mongo_db_service_.GetOutbox();
    for (const auto& doc : cursor) {
      auto oid = doc["_id"].As<userver::formats::bson::Oid>();

      auto payload = doc["payload"];
      auto entity = payload.As<entity::MongoRecipe>();
      auto jsonString = userver::formats::json::ToString(
          userver::formats::json::ValueBuilder(entity).ExtractValue());

      rabbit_->PublishReliable(
          exchange_, "recipe.created", jsonString,
          userver::engine::Deadline::FromDuration(std::chrono::seconds(5)));

      mongo_db_service_.SetOutboxSuccessStatus(oid);

      LOG_INFO() << "Published outbox event: " << oid.ToString();
    }
  } catch (const std::exception& ex) {
    LOG_ERROR() << "Outbox publisher failed: " << ex.what();
  }
}

}  // namespace recipe