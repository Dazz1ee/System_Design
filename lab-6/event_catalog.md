Структура event CreateRecipe ```
{
  "event": {
    "title": {
      "bsonType": "string",
      "minLength": 1,
      "maxLength": 200
    },
    "description": {
      "bsonType": ["string", "null"]
    },
    "servings": {
      "bsonType": ["int", "long"],
      "minimum": 1
    },
    "cook_time_minutes": {
      "bsonType": ["int", "long"],
      "minimum": 0
    },
    "author_id": {
      "bsonType": "string"
    },
    "created_at": {
      "bsonType": "date"
    },
    "version": {
      "bsonType": ["int", "long"]
    },
    "steps": {
      "bsonType": "array",
      "items": {
        "bsonType": "object",
        "required": ["step_number", "description"],
        "additionalProperties": true,
        "properties": {
          "step_number": {
            "bsonType": "int"
          },
          "description": {
            "bsonType": "string"
          }
        }
      }
    },
    "ingredients": {
      "bsonType": "array",
      "items": {
        "bsonType": "object",
        "required": ["ingredient_id", "name"],
        "additionalProperties": true,
        "properties": {
          "ingredient_id": {
            "bsonType": "objectId"
          },
          "name": {
            "bsonType": "string"
          },
          "amount": {
            "bsonType": ["int", "double", "long"]
          },
          "unit": {
            "bsonType": "string"
          }
        }
      }
    }
  }
}
```

Просто передает сущность из бд на событие создания рецепта. Если консьюмер прочитает сообщение 2 раза и сделает 2 инсерта в бд, то само состояние не изменится

**Ссылки на реализацию:**
- [producer](/recipe-service/src/service/events/recipe_producer_job.cpp)
- [consumer](/recipe-service/src/service/events/recipe_consumer.cpp)