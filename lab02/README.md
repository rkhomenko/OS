# Лабораторная работа №2
## Цель работы
Приобретение практических навыков в:
* Управление процессами в ОС
* Обеспечение обмена данных между процессами посредством каналов
## Задание
Составить и отладить программу на языке Си, осуществляющую работу с процессами
и взаимодействие между ними в одной из двух операционных систем. В результате
работы программа (основной процесс) должен создать для решение задачи один или
несколько дочерних процессов. Взаимодействие между процессами осуществляется
через системные сигналы/события и/или каналы (pipe). Необходимо обрабатывать системные ошибки, которые могут возникнуть в результате работы.
## Вариант(8)
На вход программе подается название 2-ух файлов. Необходимо отсортировать
оба файла (каждый в отдельном процессе) произвольной сортировкой (на
усмотрение студента). Родительским процессом вывести отсортированные файлы
чередованием строк первого и второго файлов.