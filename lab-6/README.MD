# Инструкция к запуску
## Mongo новая бд

`docker-compose up -d recipe-write-mongo`

Была созданая новая БД, которая хранит в себе записи рецептов, для нее накатить схему:
```
rs.initiate({
  _id: "rs0",
  members: [
    { _id: 0, host: "recipe-write-mongo:27017" }
  ]
});


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
    db.createCollection("outbox", {
    validator: {
        $jsonSchema: {
            bsonType: "object",
            additionalProperties: true,
            required: [
                "event_type",
                "aggregate_id",
                "status",
                "created_at",
                "payload"
                ],

            properties: {
                event_type: {
                    bsonType: "string",
                    description: "Тип события"
                    },

                aggregate_id: {
                    bsonType: "string",
                    description: "ID aggregate"
                    },

                status: {
                    enum: [
                        "NEW",
                        "PROCESSING",
                        "DONE",
                        "FAILED"
                        ],
                    description: "Статус"
                    },

                created_at: {
                    bsonType: "date",
                    description: "Дата создания"
                    },

                processed_at: {
                    bsonType: ["date", "null"]
                    },

                next_retry_at: {
                    bsonType: ["date", "null"]
                    },

                attempts: {
                    bsonType: "int",
                    minimum: 0
                    },

                payload: {
                    bsonType: "object",
                    description: "Payload"
                    },

                last_error: {
                    bsonType: ["string", "null"]
                    }
                }
            }
        },
    validationLevel: "strict",
    validationAction: "error"
    });

db.outbox.createIndex(
    { status: 1});

db.outbox.createIndex(
  { processed_at: 1 },
  { expireAfterSeconds: 604800 }
)
```

Схема для прошлой дб остается та же

## Запуск rabbit
`docker-compose up -d rabbit1 rabbit2`\
или поднять все необходиые компоненты
`docker-compose up -d`

# О выбранном решении

Для выполнения ЛР был выбран брокер сообщений RabbitMQ. Выбранное условия гарантии доставки - at-least-once. Данная гарантия достигается transactional outbox и manual ack. CQRS в целом подходит под данную архитектуру, но в данной реализации ингредиенты - это справочная таблица, в которой данными заполняются руками и поэтому полная синхронизация не возможна