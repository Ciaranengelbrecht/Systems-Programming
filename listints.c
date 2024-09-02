

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

// Compile this program with:
//    cc -std=c11 -Wall -Werror -o listints listints.c


void swap(int* xp, int* yp)
{
    int temp = *xp;
    *xp = *yp;
    *yp = temp;
}

void selectionSort(int arr[], int n)
{
    int i, j, min_idx;
 
    // One by one move boundary of unsorted subarray
    for (i = 0; i < n - 1; i++) {
 
        // Find the minimum element in unsorted array
        min_idx = i;
        for (j = i + 1; j < n; j++)
            if (arr[j] < arr[min_idx])
                min_idx = j;
 
        // Swap the found minimum element
        // with the first element
        swap(&arr[min_idx], &arr[i]);
    }
    for (i = 0; i < n - 1; i++) {

        if(arr[j]==arr[j-1]) arr[j]=0;
    }


}





int main(int argc, char *argv[])
{ int num = 0;
    int i =0;
    int diff = 0; int extra=1; int count =0;
    int range[2];
    int  *list= malloc(sizeof(int)*1000);

    char* token = strtok(argv[1],",");

    while (token){
        int j=0;
        num = atoi(token);
        list[i] = num;
       
        i++;
        while (token[j]){
                range[0]=atoi(token);
                if (token[j]=='-'){count++; range[1]=atoi(token+j+1);
                diff = range[1]-range[0];
                extra = extra + diff;
                for (int m=0;m<diff;++m){list[i+m]=range[0]+m+1;
                }
                i=i+diff;
                }
                ++j;
        }   
        token = strtok(0,","); 
     }
    size_t n = sizeof(list)+count+extra;
    selectionSort(list, n);
     
    int k; 
    for (k = 0; k < n; k++)
        printf("%i ", list[k]);
        printf("\n"); 
}
 