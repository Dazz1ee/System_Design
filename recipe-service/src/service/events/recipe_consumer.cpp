#include "recipe_consumer.hpp"

#include <userver/logging/log.hpp>

#include "entity/ingredient.hpp"
#include "entity/recipe.hpp"

namespace recipe {

RecipeCreatedConsumer::RecipeCreatedConsumer(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : ConsumerComponentBase(config, context),
      mongo_db_service_(context.FindComponent<MongoDbService>()) {}

void RecipeCreatedConsumer::Process(std::string message) {
  try {
    auto dto =
        userver::formats::json::FromString(message).As<entity::MongoRecipe>();

    LOG_INFO() << "Received recipe.created event";
    LOG_INFO() << "Recipe id: " << dto.id;
    mongo_db_service_.CreateRecipe(dto);

  } catch (const std::exception& ex) {
    LOG_ERROR() << "Consumer failed: " << ex.what();

    throw;
  }
}

}  // namespace recipe