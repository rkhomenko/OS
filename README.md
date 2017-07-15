# OS Labs
## Структура
```text
|
+--- labNN
|   +--- include
|   +--- src
|   +--- test
|       +--- test1
|       +--- ...
|       +--- testN
|   +--- report
|       +--- report.tex
|       +--- Makefile
|   +--- Makefile
|   +--- run_tests.sh
|   +--- README.md
+--- Makefile
```

В каждой папке с лабораторной (labNN) находятся:
1. Исходный код (include, src)
2. Тесты (test).
3. Скрипт для тестирования (run_tests.sh)
4. Отчет (report)
5. Задание (файл README.md)

## Сборка
Для сборки необходим cc совместимый компилятор (gcc, clang) и make
(возможно потребуется GNU Make). Сборка осуществляется запуском make
в корне репозитория. Можно указывать директорию для сборки. Указывайте
абсолютный путь! Опускать директорию сборки не рекомендуется!
```bash
make BUILD_DIR=/absolute/path/to/build/dir
```
Скомпилированные программы будут находиться в папках $(BUILD_DIR)/labNN.

При необходимости можно указать тип сборки: debug или release.
Реально необходим только debug, т.к. добавляет отладочную информацию для gdb.
```bash
make BUILD_DIR=/absolute/path/to/build/dir BUILD_TYPE=debug
```

Очистка производится командой
```bash
make BUILD_DIR=/absolute/path/to/build/dir clean
```

## Тесты
Структура
```text
|
+--- test
|   +--- test1
|       +--- 1.txt
|       +--- ...
|       +--- N.txt
|       +--- stdout.txt
|       +--- stderr.txt
|   +--- ...
|   +--- testN
```

В папке test содержатся поддиректории (testNN), представляющие отдельный тест.
В каждой папке testNN содержатся:
1. 1.txt, ..., N.txt - набор файлов, необходимых для работы программы
2. stdout.txt - ожидаемый вывод программы в поток вывода
3. stderr.txt - ожидаемый вывод программы в поток ошибок

Скрипт run_tests.sh использует утилиту cmp для сравнения
ожидаемых потоков вывода и ошибок с реальными, после чего выводит
пройден (passed) тест или нет (failed).

Запуск тестов для всех лабораторных осуществляется командой make в корне
репозитория
```bash
make BUILD_DIR=/absolute/path/to/build/dir test
```

## Отчеты
При наличии пакета texlive отчеты можно собрать командой
```bash
make BUILD_DIR=/absolute/path/to/build/dir report
```
Отчеты будут находиться в папках $(BUILD_DIR)/labNN.