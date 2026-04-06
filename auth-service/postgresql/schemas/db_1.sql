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
