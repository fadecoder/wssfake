#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define M (1024*1024)

typedef long unsigned un_long;

int main(int argc, char **argv){
    un_long *array, i = 0;
    int j, ch, array_size = 1000;
    int round = 100;
    int cntw=0;

    if(argc > 1){
	array_size=atoi(argv[1]);
	round = atoi(argv[2]);
    }

    int len = array_size * M;	
    int maxinter = len / sizeof(un_long);
    array = (un_long *)malloc(len);
    if(NULL == array){
	printf("fail\n");
    }
    printf("array_size=%d round=%d\n",array_size,round);
    for(j=0; j < round; j++){
//		printf("cnt=%d\n",j);
		for(i=0 ; i < maxinter; i = i + 1){
			*(array + i)  = i;
			cntw++;
		}
    }
    printf("cntw=%d\n",cntw);
    free(array);
}

