workspace {
    name "Recipe Management System"
    !identifiers hierarchical

    model {

        user = person "User" "Пользователь, который может просматривать рецепты, создавать новые рецепты, сохранять избранные рецепты и управлять своим аккаунтом"
        admin = person "Admin" "Администратор, который может управлять системой"

        recipe_system = softwareSystem "Recipe Management System" {

            firewall = container "Firewall" {
                description "Балансировщик нагрузки и обратный прокси для управления входящими запросами и обеспечения безопасности системы"
                technology "HAProxy"
                tags "Firewall"
            }

            web_app = container "Веб приложение" {
                    description "Приложение для осуществления заказа услуги и контроля передвижения"
                    tags "WebBrowser" "ClientApp"
            }   

            recipe_database = container "Recipe Database" {
                description "База данных рецептов, ингредиентов и избранного"
                technology "PostgreSQL"
                tags "Database"
            }

            auth_database = container "Auth Database" {
                description "База данных для хранения информации о пользователях и их учетных записях"
                technology "PostgreSQL"
                tags "Database"
            }

            data_storage = container "Data Storage" {
                description "Сохраняет изображения рецептов"
                technology "S3"
                tags "Storage"
            }

            recipe_backend = container "API" {
                description "Приложения для управления рецептами"
                technology "Userserver"
                tags "Backend"

                recipe_api = component "Recipe API" description "Интерфейс для взаимодействия с системой управления рецептами"
                recipe_logic = component "Recipe Logic" description "Логика управления рецептами, ингредиентами и избранным"

                firewall -> recipe_api "Проксирует запрос в API" "HTTP/REST"
                recipe_api -> recipe_logic "Обрабатывает запросы управления рецептами"
                recipe_logic -> recipe_database "Читает и сохраняет данные рецептов, ингредиентов и избранного" "TCP :5432"
                recipe_logic -> data_storage "Сохраняет изображения рецептов" "HTTP :9000"
            }

            auth_service = container "Auth Service" "Приложения для управления пользователями и их учетными записями" "Userserver"  "Backend" {
                auth_service_api = component "Auth Service API" description "Интерфейс для взаимодействия с системой аутентификации"
                auth_service_logic = component "Auth Service Logic" description "Логика аутентификации и управления пользователями"

                recipe_backend -> auth_service_api "Отправляет запрос на аутентификации" "HTTP/REST"
                auth_service_api -> auth_service_logic "Обрабатывает запросы аутентификации"

                recipe_backend -> auth_service_api "Отправляет запрос на авторизацию" "HTTP/REST"
                auth_service_api -> auth_service_logic "Обрабатывает запросы авторизации"

                recipe_backend -> auth_service_api "Отправляет запрос на регистрацию" "HTTP/REST"
                auth_service_api -> auth_service_logic "Обрабатывает запросы регистрации пользователя"

                auth_service_logic -> auth_database "Читает и записывает данные пользователей" "TCP :3306"
            }

            user -> web_app "Действия с рецептами/создание нового аккаунта"

            admin -> web_app "Управление системой"

            web_app -> firewall "Отправляет запрос в API" "HTTP"
        }
    }

    views {
        themes default

        systemLandscape "SystemLandscape"{
            include *
            autoLayout lr
        }

        systemContext recipe_system "SystemContext" {
            include *
            autolayout lr
        }

        container recipe_system "Containers" {
            include *
            autolayout
        }

        dynamic recipe_system "CreateRecipeScenario" {
            autoLayout lr

            title "Cоздание рецепта"

            user -> recipe_system.web_app "Пользователь открывает сайт и выбирает действие 'Создать рецепт'"
            recipe_system.web_app -> recipe_system.firewall "Аутентификация пользователя"
            recipe_system.firewall -> recipe_system.recipe_backend "Проксирует запрос в API"
            recipe_system.recipe_backend -> recipe_system.auth_service "Проксирует запрос в Auth Service"

            recipe_system.web_app -> recipe_system.firewall "Создание рецепта"
            recipe_system.firewall -> recipe_system.recipe_backend "Проксирует запрос в API"
            recipe_system.recipe_backend -> recipe_system.auth_service "Проверка токена доступа и прав пользователя"
            recipe_system.auth_service -> recipe_system.auth_database "Получение данных пользователя из БД"
            recipe_system.recipe_backend -> recipe_system.recipe_database "Сохранение данных рецепта в БД"

            recipe_system.web_app -> recipe_system.firewall "Загрузка изображения рецепта с полученным id"
            recipe_system.firewall -> recipe_system.recipe_backend "Проксирует запрос в API"
            recipe_system.recipe_backend -> recipe_system.data_storage "Сохраняет изображение рецепта в хранилище данных"
        }

        styles {

            element "Person" {
                shape Person
                background #08427b
                color #ffffff
            }

            element "Software System" {
                background #1168bd
                color #ffffff
            }

            element "Container" {
                background #438dd5
                color #ffffff
            }

        }
    }

}