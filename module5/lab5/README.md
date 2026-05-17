# Module 5 lab 5

## Сборка

```bash
make
```

## Запуск и проверка

```bash
sudo insmod netlink.ko
./user
dmesg | tail
sudo rmmod netlink
```

## Скриншоты работы

Информация о модуле:

![Информация о модуле](img/modinfo.png)

Сборка модуля:

![Сборка модуля](img/compilation.png)

Загрузка модуля:

![Загрузка модуля](img/insmod1.png)
![Загрузка модуля](img/insmod2.png)