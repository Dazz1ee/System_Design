-- Для auth бд
CREATE SCHEMA IF NOT EXISTS auth;

CREATE TABLE IF NOT EXISTS auth.users
(
    id         BIGSERIAL PRIMARY KEY,
    forename   TEXT      NOT NULL,
    surname    TEXT      NOT NULL,
    email      TEXT      NOT NULL UNIQUE,
    password   TEXT      NOT NULL,
    created_at TIMESTAMP NOT NULL DEFAULT NOW()
);

CREATE TABLE IF NOT EXISTS auth.roles
(
    id         BIGSERIAL PRIMARY KEY,
    name       TEXT      NOT NULL UNIQUE,

    created_at TIMESTAMP NOT NULL DEFAULT NOW()
);

CREATE TABLE IF NOT EXISTS auth.user_roles
(
    user_id BIGINT NOT NULL,
    role_id BIGINT NOT NULL,
    PRIMARY KEY (user_id, role_id),
    FOREIGN KEY (user_id) REFERENCES auth.users (id),
    FOREIGN KEY (role_id) REFERENCES auth.roles (id)
);


CREATE EXTENSION IF NOT EXISTS pg_trgm;

CREATE INDEX IF NOT EXISTS users_forename_trgm_idx
ON auth.users USING gin (forename gin_trgm_ops);

CREATE INDEX IF NOT EXISTS users_surname_trgm_idx
ON auth.users USING gin (surname gin_trgm_ops);





--

-- Для recipe бд

CREATE SCHEMA IF NOT EXISTS recipe;

CREATE TABLE IF NOT EXISTS recipe.ingredients
(
    id   BIGSERIAL PRIMARY KEY,
    name TEXT NOT NULL UNIQUE
);

CREATE TABLE IF NOT EXISTS recipe.recipes
(
    id                BIGSERIAL PRIMARY KEY,
    title             TEXT      NOT NULL,
    description       TEXT,
    servings          INT,
    cook_time_minutes INT,
    author_id         BIGINT    NOT NULL,
    created_at        TIMESTAMP NOT NULL DEFAULT NOW()
);

CREATE TABLE IF NOT EXISTS recipe.recipe_steps
(
    id          BIGSERIAL PRIMARY KEY,
    recipe_id   BIGINT NOT NULL,
    step_number INT    NOT NULL,
    description TEXT   NOT NULL,

    FOREIGN KEY (recipe_id) REFERENCES recipe.recipes (id),
    UNIQUE (recipe_id, step_number)
);

CREATE TABLE IF NOT EXISTS recipe.recipe_ingredients
(   
    recipe_id     BIGINT NOT NULL,
    ingredient_id BIGINT NOT NULL,

    amount        FLOAT,
    unit          TEXT,

    PRIMARY KEY (recipe_id, ingredient_id),
    FOREIGN KEY (recipe_id) REFERENCES recipe.recipes (id),
    FOREIGN KEY (ingredient_id) REFERENCES recipe.ingredients (id)
);

CREATE TABLE IF NOT EXISTS recipe.favorite_recipes (
    user_id BIGINT NOT NULL,
    recipe_id BIGINT NOT NULL,
    created_at TIMESTAMP DEFAULT NOW(),

    PRIMARY KEY (user_id, recipe_id),
    FOREIGN KEY (recipe_id) REFERENCES recipe.recipes(id)
);

CREATE EXTENSION IF NOT EXISTS pg_trgm;

CREATE INDEX IF NOT EXISTS recipes_title_trgm_idx
ON recipe.recipes USING gin (title gin_trgm_ops);

CREATE INDEX IF NOT EXISTS recipes_author_id_idx
    ON recipe.recipes(author_id);