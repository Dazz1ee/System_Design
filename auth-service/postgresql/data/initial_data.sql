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