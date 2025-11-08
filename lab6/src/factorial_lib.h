#ifndef FACTORIAL_LIB_H
#define FACTORIAL_LIB_H

#include <stdint.h>

struct FactorialArgs {
  uint64_t begin;
  uint64_t end;
  uint64_t mod;
  int thread_id;
};

uint64_t MultModulo(uint64_t a, uint64_t b, uint64_t mod);

#endif