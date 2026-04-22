    // Авторизация
    db.users.insertOne({
        forename: "Иван",
        surname: "Иванов",
        email: "ivan@gmail.com",
        password: "test",
        roles: ["user"],
        created_at: new Date(),
        version: 1
        })

    db.users.find({
        $text: { $search: "иван шурпатов" }
        })

    
    // Рецепты
    db.recipes.insertOne({
        title: "Новый рецепт",
        description: "Описание",
        servings: 2,
        cook_time_minutes: 30,
        author_id: "1",
        created_at: new Date(),
        version: 1,
        steps: [
            { step_number: 1, description: "Разбить яйца" },
            { step_number: 2, description: "Разогреть сковороду" },
            { step_number: 3, description: "Приготовить" }
            ],

        ingredients: [
            { ingredient_id: getId("Яйцо"), name: "Яйцо", amount: 2, unit: "шт" },
            { ingredient_id: getId("Масло сливочное"), name: "Масло сливочное", amount: 10, unit: "г" }
            ]
        })

    const last_id = "69e7d4a26ba8a10ad1992e85"
    db.recipes.find({
        _id: { $gt: new ObjectId(last_id) }
        })
        .sort({ _id: 1 })
        .limit(12)

    db.recipes.find({
        $text: { $search: "рецепт" }
        })

    const recipe_id = "69e9202b7389c309123853fd"
    db.recipes.findOne(
        { _id: new ObjectId(recipe_id) },
        { "ingredients": 1 }
        )

    db.recipes.find({ author_id: "1" })
        .sort({ created_at: -1 })

    db.favorite_recipes.insertOne(
        {
            user_id: "222",
            recipe_id: new ObjectId(recipe_id),
            recipe_title: "Новый рецепт",
            recipe_description: "Новый рецепт",
            created_at: new Date(),
            version: 1
            }
        )