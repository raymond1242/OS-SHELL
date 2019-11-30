#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

void bucle(){
    printf("Do nothing\n");
}

int main(){
    while(1){
        bucle;
        sleep(1);
    }
}
