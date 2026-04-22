// Авторизация
    const users = [
        {
            forename: "Админ",
            surname: "Админов",
            email: "admin@gmail.com",
            password: "admin",
            roles: ["admin"],
            created_at: new Date(),
            version: 1
            },

        {
            forename: "Петр",
            surname: "Иванов",
            email: "test@gmail.com",
            password: "test",
            roles: ["user"],
            created_at: new Date(),
            version: 1
            },

        {
            forename: "Петр",
            surname: "Иванов",
            email: "test@ya.ru",
            password: "test",
            roles: ["user"],
            created_at: new Date(),
            version: 1
            },

        {
            forename: "Елена",
            surname: "Иванова",
            email: "testf@gmail.com",
            password: "test",
            roles: ["user"],
            created_at: new Date(),
            version: 1
            },

        {
            forename: "Елена",
            surname: "Иванова",
            email: "testf@ya.ru",
            password: "test",
            roles: ["user"],
            created_at: new Date(),
            version: 1
            },

        {
            forename: "Иван",
            surname: "Шурпатов",
            email: "wwteam@gmail.com",
            password: "test",
            roles: ["user"],
            created_at: new Date(),
            version: 1
            },

        {
            forename: "Иван",
            surname: "Шурпатов",
            email: "wwteamsupport@gmail.com",
            password: "test",
            roles: ["support"],
            created_at: new Date(),
            version: 1
            },

        {
            forename: "Агент",
            surname: "Краснов",
            email: "dtrump@gmail.com",
            password: "test",
            roles: ["user"],
            created_at: new Date(),
            version: 1
            },

        {
            forename: "Никита",
            surname: "Макаров",
            email: "nmakarov@ya.ru",
            password: "test",
            roles: ["user"],
            created_at: new Date(),
            version: 1
            },

        {
            forename: "Линус",
            surname: "Торвальдс",
            email: "ltorvalds@gmail.com",
            password: "test",
            roles: ["user"],
            created_at: new Date(),
            version: 1
            }
        ]
    db.users.insertMany(users)

// Рецепты
           const ingredients = [
        "Соль", "Сахар", "Мука", "Яйцо", "Молоко",
        "Масло сливочное", "Курица", "Чеснок", "Лук",
        "Помидор", "Сыр", "Перец", "Оливковое масло",
        "Рис", "Морковь"
        ]
    db.ingredients.bulkWrite(
        ingredients.map(name => ({
            updateOne: {
                filter: { name },
                update: { $setOnInsert: { name, version: 1 } },
                upsert: true,
                }
            })) )

    const getId = (name) =>
        db.ingredients.findOne({ name })._id
    const recipes = [
        {
            title: "Блины",
            description: "Классические блины",
            servings: 2,
            cook_time_minutes: 20,
            author_id: "1",
            created_at: new Date(),

            steps: [
                { step_number: 1, description: "Смешать муку, молоко и яйца" },
                { step_number: 2, description: "Разогреть сковороду" },
                { step_number: 3, description: "Жарить до готовности" }
                ],
            version: 1,

            ingredients: [
                {
                    ingredient_id: getId("Мука"),
                    name: "Мука",
                    amount: 200,
                    unit: "г"
                    },
                {
                    ingredient_id: getId("Яйцо"),
                    name: "Яйцо",
                    amount: 2,
                    unit: "шт"
                    },
                {
                    ingredient_id: getId("Молоко"),
                    name: "Молоко",
                    amount: 300,
                    unit: "мл"
                    },
                {
                    ingredient_id: getId("Масло сливочное"),
                    name: "Масло сливочное",
                    amount: 20,
                    unit: "г"
                    }
                ]
            },
        {
            title: "Омлет",
            description: "Простой омлет",
            servings: 1,
            cook_time_minutes: 10,
            author_id: "2",
            created_at: new Date(),

            steps: [
                { step_number: 1, description: "Взбить яйца" },
                { step_number: 2, description: "Разогреть сковороду" },
                { step_number: 3, description: "Приготовить" }
                ],
            version: 1,

            ingredients: [
                {
                    ingredient_id: getId("Яйцо"),
                    name: "Яйцо",
                    amount: 3,
                    unit: "шт"
                    },
                {
                    ingredient_id: getId("Масло сливочное"),
                    name: "Масло сливочное",
                    amount: 10,
                    unit: "г"
                    },
                {
                    ingredient_id: getId("Соль"),
                    name: "Соль",
                    amount: 1,
                    unit: "ч.л"
                    }
                ]
            },
        {
            title: "Куриный суп",
            description: "Домашний суп",
            servings: 4,
            cook_time_minutes: 60,
            author_id: "3",
            created_at: new Date(),

            steps: [
                { step_number: 1, description: "Отварить курицу" },
                { step_number: 2, description: "Добавить овощи" },
                { step_number: 3, description: "Варить 40 минут" }
                ],
            version: 1,

            ingredients: [
                { ingredient_id: getId("Курица"), name: "Курица", amount: 500, unit: "г" },
                { ingredient_id: getId("Лук"), name: "Лук", amount: 1, unit: "шт" },
                { ingredient_id: getId("Морковь"), name: "Морковь", amount: 1, unit: "шт" }
                ]
            },
        {
            title: "Рис с чесноком",
            description: "Ароматный рис",
            servings: 3,
            cook_time_minutes: 30,
            author_id: "1",
            created_at: new Date(),

            steps: [
                { step_number: 1, description: "Отварить рис" },
                { step_number: 2, description: "Обжарить чеснок" },
                { step_number: 3, description: "Смешать" }
                ],
            version: 1,

            ingredients: [
                { ingredient_id: getId("Рис"), name: "Рис", amount: 200, unit: "г" },
                { ingredient_id: getId("Чеснок"), name: "Чеснок", amount: 3, unit: "зубчика" }
                ]
            },
        {
            title: "Гренки с сыром",
            description: "Быстрый перекус",
            servings: 1,
            cook_time_minutes: 10,
            author_id: "2",
            created_at: new Date(),

            steps: [
                { step_number: 1, description: "Намазать хлеб маслом" },
                { step_number: 2, description: "Добавить сыр" },
                { step_number: 3, description: "Обжарить" }
                ],
            version: 1,

            ingredients: [
                { ingredient_id: getId("Масло сливочное"), name: "Масло сливочное", amount: 10, unit: "г" },
                { ingredient_id: getId("Сыр"), name: "Сыр", amount: 50, unit: "г" }
                ]
            },
        {
            title: "Паста с томатами",
            description: "Итальянская паста",
            servings: 2,
            cook_time_minutes: 25,
            author_id: "3",
            created_at: new Date(),

            steps: [
                { step_number: 1, description: "Отварить пасту" },
                { step_number: 2, description: "Сделать соус" },
                { step_number: 3, description: "Смешать" }
                ],
            version: 1,

            ingredients: [
                { ingredient_id: getId("Помидор"), name: "Помидор", amount: 3, unit: "шт" },
                { ingredient_id: getId("Оливковое масло"), name: "Оливковое масло", amount: 10, unit: "мл" }
                ]
            },
        {
            title: "Жареная курица",
            description: "Хрустящая курица",
            servings: 4,
            cook_time_minutes: 45,
            author_id: "1",
            created_at: new Date(),

            steps: [
                { step_number: 1, description: "Приправить курицу" },
                { step_number: 2, description: "Разогреть масло" },
                { step_number: 3, description: "Обжарить" }
                ],
            version: 1,

            ingredients: [
                { ingredient_id: getId("Курица"), name: "Курица", amount: 500, unit: "г" },
                { ingredient_id: getId("Перец"), name: "Перец", amount: 1, unit: "ч.л" }
                ]
            },
        {
            title: "Салат",
            description: "Овощной салат",
            servings: 2,
            cook_time_minutes: 15,
            author_id: "2",
            created_at: new Date(),

            steps: [
                { step_number: 1, description: "Нарезать овощи" },
                { step_number: 2, description: "Смешать" },
                { step_number: 3, description: "Добавить масло" }
                ],
            version: 1,

            ingredients: [
                { ingredient_id: getId("Помидор"), name: "Помидор", amount: 2, unit: "шт" },
                { ingredient_id: getId("Лук"), name: "Лук", amount: 1, unit: "шт" },
                { ingredient_id: getId("Оливковое масло"), name: "Оливковое масло", amount: 10, unit: "мл" }
                ]
            },
        {
            title: "Морковный суп",
            description: "Полезный суп",
            servings: 3,
            cook_time_minutes: 40,
            author_id: "3",
            created_at: new Date(),

            steps: [
                { step_number: 1, description: "Отварить морковь" },
                { step_number: 2, description: "Измельчить" },
                { step_number: 3, description: "Подать" }
                ],
            version: 1,

            ingredients: [
                { ingredient_id: getId("Морковь"), name: "Морковь", amount: 3, unit: "шт" }
                ]
            },
        {
            title: "Яичница",
            description: "Быстрая яичница",
            servings: 1,
            cook_time_minutes: 8,
            author_id: "1",
            created_at: new Date(),

            steps: [
                { step_number: 1, description: "Разбить яйца" },
                { step_number: 2, description: "Разогреть сковороду" },
                { step_number: 3, description: "Приготовить" }
                ],
            version: 1,

            ingredients: [
                { ingredient_id: getId("Яйцо"), name: "Яйцо", amount: 2, unit: "шт" },
                { ingredient_id: getId("Масло сливочное"), name: "Масло сливочное", amount: 10, unit: "г" }
                ]
            }
        ]
    db.recipes.insertMany(recipes)

    const getRecipeId = (name) =>
        db.recipes.findOne({ title: name })._id
    const favorite_recipes = [
        {
            recipe_id: getRecipeId("Блины"),
            user_id: "1",
            created_at: new Date(),
            recipe_title: "Блины",
            version: 1,
            recipe_description: "Классические блины"
            },
        {
            recipe_id: getRecipeId("Омлет"),
            user_id: "1",
            created_at: new Date(),
            recipe_title: "Омлет",
            version: 1,
            recipe_description: "Простой омлет"
            },
        {
            recipe_id: getRecipeId("Куриный суп"),
            user_id: "1",
            created_at: new Date(),
            recipe_title: "Куриный суп",
            version: 1,
            recipe_description: "Домашний суп"
            },
        {
            recipe_id: getRecipeId("Блины"),
            user_id: "2",
            created_at: new Date(),
            recipe_title: "Блины",
            version: 1,
            recipe_description: "Классические блины"
            },
        {
            recipe_id: getRecipeId("Омлет"),
            user_id: "2",
            created_at: new Date(),
            recipe_title: "Омлет",
            version: 1,
            recipe_description: "Простой омлет"
            },
        {
            recipe_id: getRecipeId("Куриный суп"),
            user_id: "2",
            created_at: new Date(),
            recipe_title: "Куриный суп",
            version: 1,
            recipe_description: "Домашний суп"
            },
        {
            recipe_id: getRecipeId("Блины"),
            user_id: "3",
            created_at: new Date(),
            recipe_title: "Блины",
            version: 1,
            recipe_description: "Классические блины"
            },
        {
            recipe_id: getRecipeId("Омлет"),
            user_id: "3",
            created_at: new Date(),
            recipe_title: "Омлет",
            version: 1,
            recipe_description: "Простой омлет"
            },
        {
            recipe_id: getRecipeId("Куриный суп"),
            user_id: "3",
            created_at: new Date(),
            recipe_title: "Куриный суп",
            version: 1,
            recipe_description: "Домашний суп"
            },
        {
            recipe_id: getRecipeId("Блины"),
            user_id: "4",
            created_at: new Date(),
            recipe_title: "Блины",
            version: 1,
            recipe_description: "Классические блины"
            },
        {
            recipe_id: getRecipeId("Омлет"),
            user_id: "4",
            created_at: new Date(),
            recipe_title: "Омлет",
            version: 1,
            recipe_description: "Простой омлет"
            },
        {
            recipe_id: getRecipeId("Куриный суп"),
            user_id: "4",
            created_at: new Date(),
            recipe_title: "Куриный суп",
            version: 1,
            recipe_description: "Домашний суп"
            }
        ]
    db.favorite_recipes.insertMany(favorite_recipes)
