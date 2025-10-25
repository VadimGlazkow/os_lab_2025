#ifndef SUM_H
#define SUM_H

#include <stdint.h>

struct SumArgs {
  int *array;
  int begin;
  int end;
};

// Функция для подсчета суммы элементов массива
int Sum(const struct SumArgs *args);

// Функция для потока
void *ThreadSum(void *args);

#endif