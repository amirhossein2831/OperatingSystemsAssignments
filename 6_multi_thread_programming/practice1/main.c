#include <stdio.h>
#include <stdlib.h>

#define SIZE 10

int* cal(int input[], int start,int end){
    int* count = (int*)malloc(sizeof(int)*end-start);
    count[start] = 1;

    for(int i=start+1;i<end;i++){
        count[i]=count[i-1]+input[i];
    }

    return count;
}

int main(){
    int input[SIZE]= {1,2,3,4,5,6,7,8,9,10};
    int* res = cal(input ,0,SIZE);

    for(int i=0;i<SIZE;i++){
        printf("%d ",res[i]);
    }
    printf("\n");
    return 0;
}
