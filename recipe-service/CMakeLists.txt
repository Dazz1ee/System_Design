cmake_minimum_required(VERSION 3.12...3.31)
project(recipe_service CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(DownloadUserver)

find_package(
    userver
    COMPONENTS core chaotic #
    postgresql
    mongo
    redis
    rabbitmq
    QUIET
)
if (NOT userver_FOUND)
    # Tries TRY_DIR first, falls back to downloading userver from GitHub using CPM.
    download_userver(TRY_DIR third_party/userver)
endif ()

#finding jwt-cpp
find_package(jwt-cpp QUIET)
if (NOT jwt-cpp_FOUND)
    message("Loading JWT-CPP")
    include(cmake/get_cpm.cmake)
    # download jwt-cpp
    CPMAddPackage(
        NAME jwt-cpp
        GITHUB_REPOSITORY Thalhammer/jwt-cpp
        VERSION 0.7.2
        OPTIONS "JWT_BUILD_EXAMPLES OFF" "JWT_BUILD_TESTS OFF" "JWT_CMAKE_FILES_INSTALL ON"
    )
endif ()

userver_setup_environment()

# Common sources
include_directories(src)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/src)

add_library(
    ${PROJECT_NAME}_objs OBJECT
    src/handler/create_user/handler.cpp #
    src/handler/get_recipes/handler.cpp #
    src/handler/create_recipe/handler.cpp #
    src/handler/get_ingredients/handler.cpp #
    src/handler/get_recipes_v2/handler.cpp #
    src/handler/create_recipe_v2/handler.cpp #
    src/handler/get_ingredients_v2/handler.cpp #
    src/handler/login/handler.cpp #
    src/service/user_service.cpp #
    src/service/recipe_service.cpp #
    src/service/auth_service_client.cpp #
    src/service/db_service.cpp #
    src/service/in_memory_db_service.cpp #
    src/service/postgers_db_service.cpp #
    src/service/mongo_db_service.cpp #
    src/auth/jwt_auth_checker.cpp #
    src/auth/jwt_auth_factory.cpp #
    src/auth/jwt_utils.cpp #
    src/service/cache/ingredient_cache_service.cpp #
    src/service/cache/recipe_cache_service.cpp #
    src/service/cache/pg_ingredient_cache_service.cpp #
    src/service/cache/pg_recipe_cache_service.cpp #
    src/service/cache/rate_limit_cache_service.cpp #
    src/rate-limit/rate_limit_decorator.cpp #
    src/rate-limit/rate_limit_middleware_factory.cpp
    src/rate-limit/custom_pipeline.cpp
    src/service/events/recipe_producer_job.cpp
    src/service/events/recipe_consumer.cpp
    src/service/events/recipe_consumer.hpp #
)
target_link_libraries(
    ${PROJECT_NAME}_objs
    PUBLIC userver::core #
    userver::postgresql
    userver::mongo
    userver::redis
    userver::rabbitmq
    jwt-cpp::jwt-cpp
)

# The Service
add_executable(${PROJECT_NAME} src/main.cpp)

# /// [chaotic] cmake
file(GLOB_RECURSE SCHEMAS ${CMAKE_CURRENT_SOURCE_DIR}/schemas/*.yaml)
userver_target_generate_chaotic(
    ${PROJECT_NAME}-chgen
    # Generate serializers for responses
    GENERATE_SERIALIZERS
    LAYOUT "/components/schemas/([^/]*)/=schemas::{0}"
    OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/src
    SCHEMAS ${SCHEMAS}
    RELATIVE_TO ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(${PROJECT_NAME}_objs PUBLIC ${PROJECT_NAME}-chgen)
# /// [chaotic]

target_include_directories(${PROJECT_NAME}
    PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}/src
)
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_objs)

# Functional testing
userver_testsuite_add_simple()

# Install
include(GNUInstallDirs)

if (DEFINED ENV{PREFIX})
    message(STATUS "Set install prefix: $ENV{PREFIX}")
    file(TO_CMAKE_PATH "$ENV{PREFIX}" PREFIX_PATH)
    set(CMAKE_INSTALL_PREFIX "${PREFIX_PATH}")
endif ()

file(GLOB CONFIGS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/configs/*.yaml ${CMAKE_CURRENT_SOURCE_DIR}/configs/*.json)

install(
    TARGETS ${PROJECT_NAME}
    DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT ${PROJECT_NAME}
)
install(
    FILES ${CONFIGS_FILES}
    DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/${PROJECT_NAME}
    COMPONENT ${PROJECT_NAME}
)