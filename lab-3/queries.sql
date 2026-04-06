-- для auth бд
INSERT INTO auth.users (forename, surname, email, password)
VALUES ('Иван', 'Иванов', 'ivan@gmail.com', 'test') RETURNING id;

SELECT id, forename, surname, email FROM auth.users WHERE email = 'ivan@example.com';

SELECT id, forename, surname, email FROM auth.users
WHERE (forename || ' ' || surname) ILIKE '%иван шурпатов%';






--

-- для recipe бд

INSERT INTO recipe.recipes (title, description, servings, cook_time_minutes, author_id)
VALUES ('Новый рецепт', 'Описание', 2, 30, 1)
RETURNING id;
INSERT INTO recipe.recipe_steps (recipe_id, step_number, description)
VALUES (2, 30, 1)
RETURNING id;

SELECT id, title, description, servings, cook_time_minutes, author_id, created_at
FROM recipe.recipes
WHERE id > 100000
ORDER BY id
LIMIT 12

SELECT id, title, description, servings, cook_time_minutes, author_id, created_at
FROM recipe.recipes
WHERE title ILIKE '%суп%';

SELECT id, step_number, description
FROM recipe.recipe_steps
WHERE recipe_id = :recipe_id ORDER BY step_number;

INSERT INTO recipe.recipe_ingredients (recipe_id, ingredient_id, amount, unit)
VALUES (1, 2, 100, 'г');

SELECT i.id,
       i.name,
       ri.amount,
       ri.unit
FROM recipe.recipe_ingredients ri
         JOIN recipe.ingredients i ON i.id = ri.ingredient_id
WHERE ri.recipe_id = 1;

SELECT id, title, description, servings, cook_time_minutes, author_id, created_at
FROM recipe.recipes
WHERE author_id = 1
ORDER BY created_at DESC;

INSERT INTO recipe.favorite_recipes (user_id, recipe_id)
values (1, 5);