

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Compile this program with:
//    cc -std=c11 -Wall -Werror -o money money.c


//MONEY
/*void money(int total, int *dollars, int *cents)
{
    *dollars = total/100;
    *cents = total%100;
}  
int main(int argc, char *argv[])
{   int dollars;
    int cents;
    int total = atoi(argv[1]);
    money(total, &dollars, &cents);
    printf("%i dollars and %i cents \n", dollars, cents); 
}*/
//MAXIMUM

/*int *maximum_a(int values[], int n)
{
int j;
int *max = &values[0];
	for(j = 1; j < n; j++)
    {
        if (values[j] > *max) max = &values[j]; 
    } 
    return max;
}

int main(int argc, char *argv[])
{   
    int vals[argc-1];
    int j;
    int i = 0;
	for(j = 1; j < argc; j++) 
    {
        vals[i]=atoi(argv[j]);
        i++;
    }
    int index;
    int *max = maximum_a(vals, argc-1);
    index = (max-vals);
    printf("max is %i at index is %i \n", *max, index); 
}
*/

//MAXIMUM v2

int *maximum_a(int *values, int n)
{
int j;
int *max = values;
	for(j = 1; j < n; j++)
    {
        if (*(values +(j)) > *max) max = values+(j); 
    } 
    return max;
}

int main(int argc, char *argv[])
{   
    int vals[argc-1];
    int j;
    int i = 0;
	for(j = 1; j < argc; j++) 
    {
        vals[i]=atoi(argv[j]);
        i++;
    }
    int index;
    int *max = maximum_a(vals, argc-1);
    index = (max-vals);
    printf("max is %i at index is %i \n", *max, index); 
}
 
