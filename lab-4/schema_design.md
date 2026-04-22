## Схема
```
db.createCollection("ingredients", {
    validator: {
        $jsonSchema: {
            bsonType: "object",
            required: ["name", "version"],
            additionalProperties: true,
            properties: {
                name: {
                    bsonType: "string",
                    minLength: 1,
                    maxLength: 100
                    },
                version: {
                    bsonType: ["int", "long"],
                    }
                }
            }
        },
    validationLevel: "strict",
    validationAction: "error"
    })
    db.createCollection("recipes", {
        validator: {
            $jsonSchema: {
                bsonType: "object",
                required: ["title", "author_id", "created_at", "version"],
                additionalProperties: true,
                properties: {
                    title: {
                        bsonType: "string",
                        minLength: 1,
                        maxLength: 200
                        },
                    description: {
                        bsonType: ["string", "null"]
                        },
                    servings: {
                        bsonType: ["int", "long"],
                        minimum: 1
                        },
                    cook_time_minutes: {
                        bsonType: ["int", "long"],
                        minimum: 0
                        },
                    author_id: {
                        bsonType: "string"
                        },
                    created_at: {
                        bsonType: "date"
                        },
                    version: {
                        bsonType: ["int", "long"],
                        },

                    steps: {
                        bsonType: "array",
                        items: {
                            bsonType: "object",
                            required: ["step_number", "description"],
                            additionalProperties: true,
                            properties: {
                                step_number: {
                                    bsonType: "int"
                                    },
                                description: {
                                    bsonType: "string"
                                    }
                                }
                            }
                        },

                    ingredients: {
                        bsonType: "array",
                        items: {
                            bsonType: "object",
                            required: ["ingredient_id", "name"],
                            additionalProperties: true,
                            properties: {
                                ingredient_id: {
                                    bsonType: "objectId"
                                    },
                                name: {
                                    bsonType: "string"
                                    },
                                amount: {
                                    bsonType: ["int", "double", "long"]
                                    },
                                unit: {
                                    bsonType: "string"
                                    }
                                }
                            }
                        }
                    }
                }
            },
        validationLevel: "strict",
        validationAction: "error"
        })

    db.createCollection("favorite_recipes", {
        validator: {
            $jsonSchema: {
                bsonType: "object",
                required: ["user_id", "recipe_id", "created_at", "version"],
                additionalProperties: true,
                properties: {
                    user_id: {
                        bsonType: "string"
                        },
                    recipe_id: {
                        bsonType: "objectId"
                        },
                    created_at: {
                        bsonType: "date"
                        },
                    recipe_title: {
                        bsonType: ["string", "null"]
                        },
                    recipe_description: {
                        bsonType: ["string", "null"]
                        },
                    version: {
                        bsonType: ["int", "long"],
                        },
                    }
                }
            },
        validationLevel: "strict",
        validationAction: "error"
        })
```

## Объяснение выбора

1. В `recipes.steps` embedded document, потому что это не отдельный ресурс, и `steps` всегда привязаны к конкретному рецепту.

2. В `recipes.ingredients` - это ссылка, потому что ингредиенты задумывались как справочная таблица. Но для того, чтобы избежать нескольких запросов в бд, был добавлен `ingredient.name` (денормализация). Проблем с консистентностью данных быть не должно, так как name не меняется

3. В `favorite_recipes.recipe_id` - это ссылка. Embedded не может быть просто по сценарию использования. В эту коллекцию также были добавлены часть данных из коллекции `recipes`(денормализация), чтобы избежать лишних запросов в бд при скроллинге ленты избранного. Проблем с консистентностью данных не ожидается

