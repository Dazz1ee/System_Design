// Авторизация
db.createCollection("users", {
    validator: {
        $jsonSchema: {
            bsonType: "object",
            required: ["forename", "surname", "email", "password", "roles", "created_at", "version"],
            additionalProperties: true,
            properties: {
                forename: {
                    bsonType: "string",
                    minLength: 1
                    },
                surname: {
                    bsonType: "string",
                    minLength: 1
                    },
                email: {
                    bsonType: "string",
                    pattern: "^.+@.+\\..+$"
                    },
                password: {
                    bsonType: "string",
                    minLength: 3
                    },
                roles: {
                    bsonType: "array",
                    items: {
                        bsonType: "string"
                        }
                    },
                created_at: {
                    bsonType: "date"
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

// Рецепты

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