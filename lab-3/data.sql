-- Для auth бд

-- Роли
INSERT INTO auth.roles (name)
SELECT v.name
FROM (VALUES ('admin'),
             ('user'),
             ('agent'),
             ('creator'),
             ('editor'),
             ('executor'),
             ('moderator'),
             ('support'),
             ('analyst'),
             ('manager')) AS v(name)
WHERE NOT EXISTS (SELECT 1
                  FROM auth.roles r
                  WHERE r.name = v.name);


-- Пользователи
INSERT INTO auth.users (forename, surname, email, password)
SELECT v.forename, v.surname, v.email, v.password
FROM (VALUES ('Админ', 'Админов', 'admin@gmail.com', 'admin'),
             ('Петр', 'Иванов', 'test@gmail.com', 'test'),
             ('Петр', 'Иванов', 'test@ya.ru', 'test'),
             ('Елена', 'Иванова', 'testf@gmail.com', 'test'),
             ('Елена', 'Иванова', 'testf@ya.ru', 'test'),
             ('Иван', 'Шурпатов', 'wwteam@gmail.com', 'test'),
             ('Иван', 'Шурпатов', 'wwteamsupport@gmail.com', 'test'),
             ('Агент', 'Краснов', 'dtrump@gmail.com', 'test'),
             ('Никита', 'Макаров', 'nmakarov@ya.ru', 'test'),
             ('Линус ', 'Торвальдс ', 'ltorvalds@gmail.com', 'test')) AS v(forename, surname, email, password)
WHERE NOT EXISTS (SELECT 1
                  FROM auth.users u
                  WHERE u.email = v.email);


-- mtm user_roles

INSERT INTO auth.user_roles (user_id, role_id)
SELECT u.id, r.id
FROM auth.users u,
     auth.roles r
WHERE u.email = 'admin@gmail.com'
    AND r.name = 'admin'
   OR u.email = 'test@gmail.com'
    AND r.name = 'user'
   OR u.email = 'test@ya.ru'
    AND r.name = 'user'
   OR u.email = 'testf@gmail.com'
    AND r.name = 'user'
   OR u.email = 'testf@ya.ru'
    AND r.name = 'user'
   OR u.email = 'wwteam@gmail.com'
    AND r.name = 'user'
   OR u.email = 'wwteamsupport@gmail.com'
    AND r.name = 'support'
   OR u.email = 'dtrump@gmail.com'
    AND r.name = 'user'
   OR u.email = 'nmakarov@ya.ru'
    AND r.name = 'user'
   OR u.email = 'ltorvalds@gmail.com'
    AND r.name = 'user'
ON CONFLICT DO NOTHING;











--

-- Для recipe бд

-- ингредиенты
INSERT INTO recipe.ingredients (name)
SELECT v.name
FROM (VALUES ('Соль'),
             ('Сахар'),
             ('Мука'),
             ('Яйцо'),
             ('Молоко'),
             ('Масло сливочное'),
             ('Курица'),
             ('Чеснок'),
             ('Лук'),
             ('Помидор'),
             ('Сыр'),
             ('Перец'),
             ('Оливковое масло'),
             ('Рис'),
             ('Морковь')) AS v(name)
WHERE NOT EXISTS (SELECT 1
                  FROM recipe.ingredients i
                  WHERE i.name = v.name);

-- рецепты
INSERT INTO recipe.recipes (title, description, servings, cook_time_minutes, author_id)
SELECT *
FROM (VALUES ('Блины', 'Классические блины', 2, 20, 1),
             ('Омлет', 'Простой омлет', 1, 10, 2),
             ('Куриный суп', 'Домашний суп', 4, 60, 3),
             ('Рис с чесноком', 'Ароматный рис', 3, 30, 1),
             ('Гренки с сыром', 'Быстрый перекус', 1, 10, 2),
             ('Паста с томатами', 'Итальянская паста', 2, 25, 3),
             ('Жареная курица', 'Хрустящая курица', 4, 45, 1),
             ('Салат', 'Овощной салат', 2, 15, 2),
             ('Морковный суп', 'Полезный суп', 3, 40, 3),
             ('Яичница', 'Быстрая яичница', 1, 8, 1))
         AS v(title, description, servings, cook_time_minutes, author_id)
WHERE NOT EXISTS (SELECT 1
                  FROM recipe.recipes r
                  WHERE r.title = v.title);

-- шаги рецепта
INSERT INTO recipe.recipe_steps (recipe_id, step_number, description)
SELECT r.id, v.step_number, v.description
FROM recipe.recipes r
         JOIN (VALUES ('Блины', 1, 'Смешать муку, молоко и яйца'),
                      ('Блины', 2, 'Разогреть сковороду'),
                      ('Блины', 3, 'Жарить до готовности'),

                      ('Омлет', 1, 'Взбить яйца'),
                      ('Омлет', 2, 'Разогреть сковороду'),
                      ('Омлет', 3, 'Приготовить'),

                      ('Куриный суп', 1, 'Отварить курицу'),
                      ('Куриный суп', 2, 'Добавить овощи'),
                      ('Куриный суп', 3, 'Варить 40 минут'),

                      ('Рис с чесноком', 1, 'Отварить рис'),
                      ('Рис с чесноком', 2, 'Обжарить чеснок'),
                      ('Рис с чесноком', 3, 'Смешать'),

                      ('Гренки с сыром', 1, 'Намазать хлеб маслом'),
                      ('Гренки с сыром', 2, 'Добавить сыр'),
                      ('Гренки с сыром', 3, 'Обжарить'),

                      ('Паста с томатами', 1, 'Отварить пасту'),
                      ('Паста с томатами', 2, 'Сделать соус'),
                      ('Паста с томатами', 3, 'Смешать'),

                      ('Жареная курица', 1, 'Приправить курицу'),
                      ('Жареная курица', 2, 'Разогреть масло'),
                      ('Жареная курица', 3, 'Обжарить'),

                      ('Салат', 1, 'Нарезать овощи'),
                      ('Салат', 2, 'Смешать'),
                      ('Салат', 3, 'Добавить масло'),

                      ('Морковный суп', 1, 'Отварить морковь'),
                      ('Морковный суп', 2, 'Измельчить'),
                      ('Морковный суп', 3, 'Подать'),

                      ('Яичница', 1, 'Разбить яйца'),
                      ('Яичница', 2, 'Разогреть сковороду'),
                      ('Яичница', 3, 'Приготовить'))
    AS v(title, step_number, description) ON r.title = v.title
WHERE NOT EXISTS (SELECT 1
                  FROM recipe.recipe_steps rs
                  WHERE rs.recipe_id = r.id
                    AND rs.step_number = v.step_number);

-- mtm recipe_ingredients
INSERT INTO recipe.recipe_ingredients (recipe_id, ingredient_id, amount, unit)
SELECT r.id, i.id, v.amount, v.unit
FROM (VALUES ('Блины', 'Мука', 200, 'г'),
             ('Блины', 'Яйцо', 2, 'шт'),
             ('Блины', 'Молоко', 300, 'мл'),
             ('Блины', 'Масло сливочное', 20, 'г'),

             ('Омлет', 'Яйцо', 3, 'шт'),
             ('Омлет', 'Масло сливочное', 10, 'г'),
             ('Омлет', 'Соль', 1, 'ч.л'),

             ('Куриный суп', 'Курица', 500, 'г'),
             ('Куриный суп', 'Лук', 1, 'шт'),
             ('Куриный суп', 'Морковь', 1, 'шт'),

             ('Рис с чесноком', 'Рис', 200, 'г'),
             ('Рис с чесноком', 'Чеснок', 3, 'зубчика'),

             ('Салат', 'Помидор', 2, 'шт'),
             ('Салат', 'Лук', 1, 'шт'),
             ('Салат', 'Оливковое масло', 10, 'мл'))
         AS v(recipe_title, ingredient_name, amount, unit)
         JOIN recipe.recipes r ON r.title = v.recipe_title
         JOIN recipe.ingredients i ON i.name = v.ingredient_name
WHERE NOT EXISTS (SELECT 1
                  FROM recipe.recipe_ingredients ri
                  WHERE ri.recipe_id = r.id
                    AND ri.ingredient_id = i.id);
