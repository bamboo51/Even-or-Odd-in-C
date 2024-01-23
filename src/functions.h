#ifndef _FUNCTIONS_HEADER_
#define _FUNCTIONS_HEADER_

#include <stdbool.h>

#define EVEN 0
#define ODD 1

typedef int parity;

void print_dice(int number);
parity even_or_odd(int number);
bool judge(int number1, int number2, parity expect);

#endif