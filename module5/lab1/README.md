# Module 5 lab 1

## Сборка

```bash
make
```

## Работа с модулем:

```bash
sudo insmod hello.ko
dmesg | tail
sudo rmmod hello
dmesg | tail
```

## Скриншоты работы

Информация о модуле:

![Информация о модуле](img/modinfo.png)

Сборка модуля:

![Сборка модуля](img/compilation.png)

Загрузка модуля:

![Загрузка модуля](img/insmod.png)

Сообщения в dmesg:

![Сообщения в dmesg](img/dmesg.png)
