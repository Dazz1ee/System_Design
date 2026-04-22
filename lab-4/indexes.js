// авторизация
db.users.createIndex({ email: 1 }, { unique: true })
db.users.createIndex({ forename: "text", surname: "text" })

// Рецепты

db.ingredients.createIndex({ name: 1 }, { unique: true })
db.recipes.createIndex({ author_id: 1 })
db.recipes.createIndex({ created_at: -1, _id: -1 })
db.recipes.createIndex({ title: "text" })
db.favorite_recipes.createIndex(
    { user_id: 1, recipe_id: 1 },
    { unique: true }
)
