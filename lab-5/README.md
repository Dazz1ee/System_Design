## Метрики

В рамках ЛР были добавлен хендлер, который отдает метрики + prometheus + grafana

Метрики по которым, смотрел производительность метода

Дашборд для графаны лежит в [data](/lab-5/data/dashboard-1778109606858.json)

```
avg by(percentile) (http_handler_timings{http_path="/api/v2/recipes", application="recipe-service", percentile=~"p50|p95|p90|p99"})
```

```
avg by(percentile) (http_handler_timings{http_path="/api/v1/recipes", application="recipe-service", percentile=~"p50|p95|p90|p99"})
```

```
avg by(percentile) (http_handler_timings{http_path="/api/v2/recipes/_id_/ingredients", application="recipe-service", percentile=~"p50|p95|p90|p99"})
```

```
avg by(percentile) (http_handler_timings{http_path="/api/v1/recipes/_id_/ingredients", application="recipe-service", percentile=~"p50|p95|p90|p99"})
```

Запуск докер образов для prometheus и grafana

```
docker-compose up -d grafana prometheus
```

Адреса:
```
http://localhost:9090 - prometheus
http://localhost:3000 - grafana

```

## Кеш 
Кеширование реализовано через стратегию cache aside, подробнее почему такой выбор в [perfomance_design](/lab-5/perfomance_design.md)

Всего 3 кеша: один для рецептов, второй для ингредиентов, третий для rate-limiting. Разделили кеши, чтобы они не конкурировали за память. Для всех кешей политика вытеснения `allkeys-lfu`

Запуск докер образов для кешей

```
docker-compose up -d redis-rate-limit redis-recipe redis-ingredient
```


## Rate-limit

За основу взять fixed window, подробнее почему такой выбор в [perfomance_design](/lab-5/perfomance_design.md). `TTL=1min` для любого ip адреса. IP берется из header `X-Real-IP`.


Реализовано было через [middleware](https://userver.tech/d6/dcc/md_en_2userver_2http__server__middlewares.html#autotoc_md688), чтобы можно было удобно добавить rate-limiting на любой хендлер с кастомными лимитами. 

Хендлеры, на которых установлен rate-limit:
- `POST /api/v1/user`
- `POST /api/v1/login`
- `POST /api/v1/recipe`
- `POST /api/v2/recipe`