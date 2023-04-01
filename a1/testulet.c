#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    char *verf="size_greater=125";
    char *substr = "size_greater=";
    char *number = verf+strlen(substr);
    int elnumero=-1;
    elnumero = atoi(number);
    printf("%d\n",elnumero);
}