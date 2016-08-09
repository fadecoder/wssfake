#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define M (1024*1024)

typedef long unsigned un_long;

int main(int argc, char **argv){
	un_long *array, i = 0;
	int j, ch, array_size = 1000;
    while((ch = getopt(argc, argv, "m:"))!= -1) {
        switch(ch) {
            case 'm':
                array_size = atoi(optarg);
                break;
            default:
                break;
        }
    }

    printf("array size:%dMB\n", array_size);
    int len = array_size * M;	
    int round = 100;
    int maxinter = len / sizeof(un_long);
    array = (un_long *)malloc(len);
	if(array == NULL)
		printf("fail\n");
	for(j=0; j < round; j++){
		for(i=0 ; i < maxinter; i = i + 512){
	//		*(array + i)  = i;
		}
	}
    free(array);
}

