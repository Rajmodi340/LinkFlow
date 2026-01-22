#include "Base62Encoder.h"

static const std :: string  BASE62_CHARS=
 "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

 std :: string Base62Encoder:: encode( long long number){
 if(number ==0){
    return "0";
 }
 std :: string  result ="";

 while(number>0){
    int remainder = number % 62;
    result= BASE62_CHARS[remainder] +result;
    number=number/62;

 }

 return result;}
