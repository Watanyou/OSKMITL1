#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <stdio.h>

using namespace std;

#define NUM_THREADS     5
#define N_BUFFER        1000
#define N_ADD           20
#define N_REMOVE        30

void *add_item(void *threadid);
void *remove_item(void *threadid);
//void *PrintHello(void *threadid);

int main () {
   pthread_t threads[NUM_THREADS];
   int rc;
   int i;

   for( i=0; i < NUM_THREADS; i++ ){
        printf("main() : creating thread, %d \n",i);
        //rc = pthread_create(&threads[i], NULL, PrintHello, (void *)i);

        if (rc){
            printf("Error:unable to create thread, %d \n",rc);
            exit(-1);
        }
   }
   pthread_exit(NULL);
}
/*
void *PrintHello(void *threadid) {
   long tid;
   tid = (long)threadid;
   printf("Hello World! Thread %d' \n",tid);
   pthread_exit(NULL);
}
*/
void *add_item(void *threadid) {

}

void *remove_item(void *threadid) {


}

