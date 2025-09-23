#!/bin/bash

if [ $# -eq 0 ]; then
    echo "Ошибка: не указаны аргументы"
    exit 1
fi

count=$#
sum=0

for num in $@; do
    if [[ $num =~ ^-?[0-9]+$ ]]; then
        ((sum += num))
    else
        echo "Ошибка: '$num' не является целым числом"
        exit 1
    fi
done

average=$((sum / count))

echo "Количество аргументов: $count"
echo "Сумма: $sum"
echo "Среднее арифметическое: $average"