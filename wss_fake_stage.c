#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define M (1024*1024)

typedef long unsigned un_long;

int main(int argc, char **argv){
	un_long *array, i = 0;
	int j, k,  array_size = 500;
    int len = array_size * M;	
    int round = 2;
    int maxinter = len / sizeof(un_long) / 10;
    array = (un_long *)malloc(len);
	if(array == NULL)
		printf("fail\n");
	for(j=0; j < round; j++){
        for(k=1;k<=10;k++){
		    for(i=0 ; i < maxinter * k; i = i + 512){
			    *(array + i)  = 0;
		    }
        }
	}
    free(array);
}

