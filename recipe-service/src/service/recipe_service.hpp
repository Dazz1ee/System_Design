#pragma once

#include <userver/clients/http/client.hpp>
#include <userver/components/component_base.hpp>
#include <userver/components/component.hpp>

#include "schemas/recipe.hpp"
#include "db_service.hpp"

namespace recipe::services {

class RecipeService final : public userver::components::ComponentBase {
public:
  static constexpr std::string_view kName = "recipe-service";

  RecipeService(const userver::components::ComponentConfig& config,
                    const userver::components::ComponentContext& context);

  schemas::CreateRecipeResponseDTO CreateRecipe(
      const schemas::CreateRecipeRequestDTO& request,
      std::int64_t userId) const;

  schemas::GetRecipesResponseDTO GetRecipes(std::int64_t last_id, int64_t limit) const;

  schemas::GetIngredientsResponseDTO GetRecipeIngredients(std::int64_t recipe_id) const;

private:
  const DbService& db_service_;
};

}  // namespace recipe