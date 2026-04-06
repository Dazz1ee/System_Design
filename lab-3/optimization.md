# Запросы и оптимизация

## Создание нового пользователя (не требует оптимизация)

```sql
EXPLAIN INSERT INTO auth.users (forename, surname, email, password)
VALUES ('Иван', 'Иванов', 'ivan1@gmail.com', 'test') RETURNING id;
```

Вывод:
```text
Insert on users  (cost=0.00..0.02 rows=1 width=144)
  ->  Result  (cost=0.00..0.02 rows=1 width=144)
```

## Поиск пользователя по логину (не требует оптимизация)

```sql
EXPLAIN SELECT (id, forename, surname, email) FROM auth.users WHERE email = 'ivan@example.com';
```

Вывод: поиск уже по индексу
```text
Index Scan using users_email_key on users  (cost=0.15..8.17 rows=1 width=32)
  Index Cond: (email = 'ivan@example.com'::text)
```

## Поиск пользователя по маске имя и фамилии

```sql
EXPLAIN SELECT (id, forename, surname, email)
FROM auth.users
WHERE forename ILIKE '%иван%' AND surname ILIKE '%шурпатов%';
```

Вывод: 
```text
Seq Scan on users  (cost=0.00..2887.19 rows=10 width=58) (actual time=0.013..31.744 rows=2 loops=1)
  Filter: (((forename || ' '::text) || surname) ~~* '%иван шурпатов%'::text)
  Rows Removed by Filter: 100009
Planning Time: 0.066 ms
Execution Time: 31.755 ms
```

Нужен индекс GIN, чтобы фильтрация была быстрее
```sql
CREATE EXTENSION pg_trgm;

CREATE INDEX users_fullname_trgm_idx
ON auth.users USING gin ((forename || ' ' || surname) gin_trgm_ops);
```

вывод после добавление индекса
```text
Bitmap Heap Scan on users  (cost=146.55..183.91 rows=10 width=58) (actual time=0.043..0.044 rows=2 loops=1)
  Recheck Cond: (((forename || ' '::text) || surname) ~~* '%иван шурпатов%'::text)
  Heap Blocks: exact=1
  ->  Bitmap Index Scan on users_fullname_trgm_idx  (cost=0.00..146.54 rows=10 width=0) (actual time=0.038..0.038 rows=2 loops=1)
        Index Cond: (((forename || ' '::text) || surname) ~~* '%иван шурпатов%'::text)
Planning Time: 0.210 ms
Execution Time: 0.055 ms
```

## Создание рецепта (не требует оптимизации)

```sql
EXPLAIN ANALYSE INSERT INTO recipe.recipes (title, description, servings, cook_time_minutes, author_id)
VALUES ('Новый рецепт', 'Описание', 2, 30, 1)
RETURNING id
```

Вывод: 
```text
Insert on recipes  (cost=0.00..0.02 rows=1 width=96) (actual time=0.106..0.106 rows=1 loops=1)
  ->  Result  (cost=0.00..0.02 rows=1 width=96) (actual time=0.070..0.070 rows=1 loops=1)
Planning Time: 0.025 ms
Execution Time: 0.124 ms
```

+ вставка шагов в рецепте

```sql
INSERT INTO recipe.recipe_steps (recipe_id, step_number, description)
VALUES (2, 30, 1)
RETURNING id;
```

```text
Insert on recipe_steps  (cost=0.00..0.01 rows=1 width=52) (actual time=0.107..0.108 rows=1 loops=1)
  ->  Result  (cost=0.00..0.01 rows=1 width=52) (actual time=0.006..0.006 rows=1 loops=1)
Planning Time: 0.020 ms
Trigger for constraint recipe_steps_recipe_id_fkey: time=0.098 calls=1
Execution Time: 0.232 ms
```

## Получения списка рецептов

```sql
EXPLAIN ANALYSE SELECT id, title, description, servings, cook_time_minutes, author_id, created_at
FROM recipe.recipes
WHERE id > 100000
ORDER BY id
LIMIT 12
```

Вывод: поиск уже по индексу
```text
Limit  (cost=0.29..8.50 rows=12 width=109) (actual time=0.004..0.005 rows=12 loops=1)
  ->  Index Scan using recipes_pkey on recipes  (cost=0.29..8.50 rows=12 width=109) (actual time=0.003..0.004 rows=12 loops=1)
        Index Cond: (id > 100000)
Planning Time: 0.354 ms
Execution Time: 0.013 ms
```

## Поиск рецептов по названию

```sql
EXPLAIN SELECT (id, forename, surname, email)
FROM auth.users
WHERE forename ILIKE '%иван%' AND surname ILIKE '%шурпатов%';
```

Вывод: 
```text
Seq Scan on recipes  (cost=0.00..2975.16 rows=10 width=109) (actual time=0.011..22.827 rows=2 loops=1)
  Filter: (title ~~* '%суп%'::text)
  Rows Removed by Filter: 100011
Planning Time: 0.122 ms
Execution Time: 22.849 ms
```

Нужен индекс GIN, чтобы фильтрация была быстрее
```sql
CREATE EXTENSION pg_trgm;

CREATE INDEX recipes_title_trgm_idx
ON recipe.recipes USING gin (title gin_trgm_ops);
```

вывод после добавление индекса
```text
Bitmap Heap Scan on recipes  (cost=17.12..54.96 rows=10 width=109) (actual time=0.011..0.012 rows=2 loops=1)
  Recheck Cond: (title ~~* '%суп%'::text)
  Heap Blocks: exact=1
  ->  Bitmap Index Scan on recipes_title_trgm_idx  (cost=0.00..17.12 rows=10 width=0) (actual time=0.006..0.006 rows=2 loops=1)
        Index Cond: (title ~~* '%суп%'::text)
Planning Time: 0.120 ms
Execution Time: 0.023 ms
```

## Добавление ингредиента в рецепт (не требует оптимизации)

```sql
EXPLAIN ANALYSE INSERT INTO recipe.recipe_ingredients (recipe_id, ingredient_id, amount, unit)
VALUES (1, 2, 100, 'г');
```

Вывод:
```text
Insert on recipe_ingredients  (cost=0.00..0.01 rows=0 width=0) (actual time=0.046..0.046 rows=0 loops=1)
  ->  Result  (cost=0.00..0.01 rows=1 width=56) (actual time=0.001..0.001 rows=1 loops=1)
Planning Time: 0.017 ms
Trigger for constraint recipe_ingredients_recipe_id_fkey: time=0.177 calls=1
Trigger for constraint recipe_ingredients_ingredient_id_fkey: time=0.128 calls=1
Execution Time: 0.360 ms
```

## Получение ингредиентов рецепта

```sql
EXPLAIN ANALYSE SELECT i.id,
       i.name,
       ri.amount,
       ri.unit
FROM recipe.recipe_ingredients ri
         JOIN recipe.ingredients i ON i.id = ri.ingredient_id
WHERE ri.recipe_id = 12;
```

Вывод: поиск уже по индексу
```text
Nested Loop  (cost=4.34..37.52 rows=5 width=72) (actual time=0.003..0.003 rows=0 loops=1)
  ->  Bitmap Heap Scan on recipe_ingredients ri  (cost=4.19..12.66 rows=5 width=48) (actual time=0.003..0.003 rows=0 loops=1)
        Recheck Cond: (recipe_id = 12)
        ->  Bitmap Index Scan on recipe_ingredients_pkey  (cost=0.00..4.19 rows=5 width=0) (actual time=0.002..0.002 rows=0 loops=1)
              Index Cond: (recipe_id = 12)
  ->  Index Scan using ingredients_pkey on ingredients i  (cost=0.15..4.97 rows=1 width=40) (never executed)
        Index Cond: (id = ri.ingredient_id)
Planning Time: 0.156 ms
```

## Получение рецептов пользователя

```sql
EXPLAIN ANALYSE
SELECT id, title, description, servings, cook_time_minutes, author_id, created_at
FROM recipe.recipes
WHERE author_id = 1
ORDER BY created_at DESC;
```

Вывод:
```text
  ->  Seq Scan on recipes  (cost=0.00..2975.16 rows=98 width=109) (actual time=0.007..3.141 rows=105 loops=1)
        Filter: (author_id = 1)
        Rows Removed by Filter: 99908
Planning Time: 0.046 ms
Execution Time: 3.170 ms
```

нужно накинуть индекс на поиск
```sql
CREATE INDEX recipes_author_id_idx
    ON recipe.recipes(author_id);
```

вывод после добавления индекса

```text
Sort  (cost=325.58..325.82 rows=98 width=109) (actual time=0.093..0.096 rows=105 loops=1)
  Sort Key: created_at DESC
  Sort Method: quicksort  Memory: 38kB
  ->  Bitmap Heap Scan on recipes  (cost=5.05..322.34 rows=98 width=109) (actual time=0.021..0.084 rows=105 loops=1)
        Recheck Cond: (author_id = 1)
        Heap Blocks: exact=97
        ->  Bitmap Index Scan on recipes_author_id_idx  (cost=0.00..5.03 rows=98 width=0) (actual time=0.013..0.014 rows=105 loops=1)
              Index Cond: (author_id = 1)
Planning Time: 0.133 ms
Execution Time: 0.107 ms
```

## Добавление рецепта в избранное (не требует оптимизации)

```sql
EXPLAIN ANALYSE INSERT INTO recipe.favorite_recipes (user_id, recipe_id)
values (1, 11);
```

Вывод:
```text
Insert on favorite_recipes  (cost=0.00..0.01 rows=0 width=0) (actual time=0.137..0.137 rows=0 loops=1)
  ->  Result  (cost=0.00..0.01 rows=1 width=24) (actual time=0.020..0.020 rows=1 loops=1)
Planning Time: 0.017 ms
Trigger for constraint favorite_recipes_recipe_id_fkey: time=0.134 calls=1
Execution Time: 0.279 ms
```