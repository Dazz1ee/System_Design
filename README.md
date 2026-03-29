# System_Design

# Вариант 23 Система управления рецептами https://www.allrecipes.com/

## 1. Запуск

### 1.1 docker-compose
docker-compose up --build

Сервисы будут доступны:
- Auth Service: http://localhost:8000
- Recipe Service: http://localhost:8001

Ключи генерется сами, если их не было

### 1.2 локально

в auth-service необходимо сгенерировать ключи (пример команды: `openssl rsa -in private.pem -pubout -out public.pem`) и положить их в папку keys

дальнейшие действия описаны в Reame каждого сервиса

## 2. Swagger

- [Auth Service API](/auth-service/api/auth.yaml) [Auth Service schemas](/auth-service/schemas/auth.yaml) 
- [Recipe Service API](/recipe-service/api/auth.yaml) [Recipe Service recipe schemas](/recipe-service/schemas/recipe.yaml) и [Recipe Service user schemas](/recipe-service/schemas/user.yaml) 

## 3. Postman коллекция

postman и insomnia коллекции доступны в [collections](/collections/)

## 4. Сервис авторизации

Сервис отвечает за регистрацию пользователей и аутентификацию. 
Тестирование было только ручное

### 4.1 Ручки

POST /api/v1/user  
Регистрация пользователя

Request:
{
  "name": "Иван Иванов",
  "email": "ivan@example.com",
  "password": "securepassword123"
}

Response:
{
    "id": 32
}

Возможные http-статусы:
- 201 
- 400 при ошибках валидации
- 409 если email уже существует
- 500
---

POST /api/v1/login  
Аутентификация  пользователя

Request:
{
  "email": "user@example.com",
  "password": "password"
}

Response:
{
    "token": "...",
    "userId": 32
}

Возможные http-статусы:
- 200
- 400 при ошибках валидации
- 500
---

GET /.well-known/jwks.json
Получение публичных ключей (на практике только один)

Response:
{
    "keys": [
        {
            "kty": "RSA",
            "kid": "test",
            "use": "sig",
            "alg": "RS256",
            "pem": "-----BEGIN PUBLIC KEY-----\nMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAqcCKPEE21NhoADO4wnL2\nVa2+DXzLe31x9guQF7/r0ZM7TZKpYigHkO1tVrrx+0dc3v+ZHsHWzxDDAFsvnfJ7\nqVLTjEMv+u4XwV006gg5edpM0ddTqFkGy7LefoVMk3InNhJZG5V5eazI8ioOFmVw\ndw11gl+b0yh5zZmlbyQuNizK7Il+/vZD0gioZXmosJIuTU37Srq2+2kG6VZ0Cm1E\nXhzsqzM95bmgRhlFrrnTjQiNXINU+cu90MP49cvuPDLP+6SyPxA78B8WKehH+CyC\nemM+eADWTQuyDPSaS47AdjvIAzrpplD56YRWEKbGE6OqBB2GEe1P9ijxus51fgYk\n2QIDAQAB\n-----END PUBLIC KEY-----\n"
        }
    ]
}

Возможные http-статусы:
- 200
- 500
---

## 5. Сервис рецептов

Сервис для работы с рецептами.

### 5.1 Ручки

POST /api/v1/recipes 
Создание рецепта

Headers:
Authorization: Bearer <token>

Request:
{
  "title": "Шоколадный торт",
  "description": "Вкусный шоколадный торт для десерта",
  "servings": 8,
  "cookTimeMinutes": 60,
  "ingredients": [
    {
      "id": 1,
      "quantity": {
        "amount": 200,
        "unit": "г"
      }
    },
    {
      "id": 2,
      "quantity": {
        "amount": 100,
        "unit": "г"
      }
    }
  ],
  "steps": [
    "Разогреть духовку до 180°C",
    "Смешать ингредиенты",
    "Выпекать 45 минут"
  ]
}
Response:
{
    "id": 2,
    "createdAt": "2026-03-29T17:22:52.399143213+00:00"
}

Возможные http-статусы:
- 201
- 401
- 502 при ошибках от auth service
- 400 при ошибках валидации
- 500
---

GET /api/v1/recipes?limit={limit}&last_id={last_id}  
Получение списка рецептов

Параметры запросы опциональные

Response:
{
    "recipes": [
        {
            "id": 3,
            "title": "Шоколадный торт",
            "description": "Вкусный шоколадный торт для десерта",
            "servings": 8,
            "cookTimeMinutes": 60,
            "steps": [
                "Разогреть духовку до 180°C",
                "Смешать ингредиенты",
                "Выпекать 45 минут"
            ],
            "authorId": 3,
            "createdAt": "2026-03-29T17:56:56.956284957+00:00"
        },
        {
            "id": 2,
            "title": "Шоколадный торт",
            "description": "Вкусный шоколадный торт для десерта",
            "servings": 8,
            "cookTimeMinutes": 60,
            "steps": [
                "Разогреть духовку до 180°C",
                "Смешать ингредиенты",
                "Выпекать 45 минут"
            ],
            "authorId": 9,
            "createdAt": "2026-03-29T17:22:52.399143213+00:00"
        },
        {
            "id": 1,
            "title": "Шоколадный торт",
            "description": "Вкусный шоколадный торт для десерта",
            "servings": 8,
            "cookTimeMinutes": 60,
            "steps": [
                "Разогреть духовку до 180°C",
                "Смешать ингредиенты",
                "Выпекать 45 минут"
            ],
            "authorId": 9,
            "createdAt": "2026-03-29T17:22:35.109794957+00:00"
        }
    ]
}

Возможные http-статусы:
- 200
- 400 при ошибках валидации
- 500
---

GET /api/v1/recipes/{id}/ingredients  
Получение рецепта

Response:
{
    "ingredients": [
        {
            "id": 1,
            "name": "Sugar",
            "amount": 200.0,
            "unit": "г"
        },
        {
            "id": 2,
            "name": "Salt",
            "amount": 100.0,
            "unit": "г"
        }
    ]
}

---

POST /api/v1/user  
Регистрация пользователя

Request:
{
  "name": "Иван Иванов",
  "email": "ivan@example.com",
  "password": "securepassword123"
}

Response:
{
    "id": 32
}

Возможные http-статусы:
- 201 
- 400 при ошибках валидации
- 409 если сервис авторизации вернул 409 статус
- 500
---

POST /api/v1/login  
Аутентификация  пользователя

Request:
{
  "email": "user@example.com",
  "password": "password"
}

Response:
{
    "token": "...",
    "userId": 32
}

Возможные http-статусы:
- 200
- 400 при ошибках валидации
- 401 если сервис  авторизации вернул 401 статус
- 500
---