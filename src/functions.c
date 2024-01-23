#include "functions.h"
#include <stdio.h>

void print_dice(int number){
    char* dice_strings[6] = {
        " ＿＿＿＿＿\n|　　　　　|\n|　　　　　|\n|　　〇　　|\n|　　　　　|\n|　　　　　|\n ￣￣￣￣￣",

        " ＿＿＿＿＿\n|　　　　　|\n|　●　　　|\n|　　　　　|\n|　　　●　|\n|　　　　　|\n ￣￣￣￣￣",

        " ＿＿＿＿＿\n|　　　　　|\n|　●　　　|\n|　　●　　|\n|　　　●　|\n|　　　　　|\n ￣￣￣￣￣",

        " ＿＿＿＿＿\n|　　　　　|\n|　●　●　|\n|　　　　　|\n|　●　●　|\n|　　　　　|\n ￣￣￣￣￣",

        " ＿＿＿＿＿\n|　　　　　|\n|　●　●　|\n|　　●　　|\n|　●　●　|\n|　　　　　|\n ￣￣￣￣￣",

        " ＿＿＿＿＿\n|　　　　　|\n|　●　●　|\n|　●　●　|\n|　●　●　|\n|　　　　　|\n ￣￣￣￣￣"

    };

    printf("%s", dice_strings[number-1]);
    printf("\n");
}

parity even_or_odd(int number){
    if(number % 2 == 0){
        return EVEN;
    }else{
        return ODD;
    }
}

bool judge(int number1, int number2, parity expect){
    return even_or_odd(number1+number2) == expect;
}