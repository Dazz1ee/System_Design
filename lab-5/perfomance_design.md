## 1. Снятие метрик до начала выполнения ЛР

Метрики GET ручек, для который возможно добавить кеш

![](/lab-5/images/grafana1.png)

![](/lab-5/images/grafana2.png)

Результаты из нагрузочного в постмане:

![](/lab-5/images/per-1.png)

Список метрик, по которым были построенны графики в графане: 
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

```
postgresql_connections_opened{application="recipe-service"}
```


## 2. Выбор стратегии кеширования

Как видно из метрик для оптимизации латенси, кеш не нужен (4ms в 99п, поиск только по индексам). Но можно добавить кеш, чтобы снять нагрузку с бд. Например: есть ручка с получением списка рецептов, если был бы фронт, он запрашивал бь при загрузке 1 страницу, и тогда разумно закещировать наиболее требуемые ресурсы из бд.

Была выбрана стратегия *cache aside*, так как она проста в реализации, не надо придумывать велосипед, чтобы решить dual write problem (запись в кеш и бд).

Инвалидация кеша только по ttl, так как строгая консистентность данных в случае с рецептами кажется оверхедом.


## 3. Выбор стратегии в rate-limiting

Rate-limiting был реализован на основе алгоритма *fixed window* из-за двух причин: простота в реализации и малое потребление памяти. `TTL=1min` для любого ip адреса. IP берется из header `X-Real-IP`.

## 4. Метрики после добавления кеша

Как можно заметить производительность осталось +- на том же уровне


![](/lab-5/images/grafana3.png)

![](/lab-5/images/grafana4.png)

![](/lab-5/images/per-2.png)


Но умешилось количество открытых соединений к бд

![](/lab-5/images/grafana5.png)
