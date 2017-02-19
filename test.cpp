#include <pthread.h>
#include <stdio.h>
#include <conio.h>

#define NUM_THREADS     5
#define N_BUFFER        1000
#define N_DATA          100000
#define N_PRODUCER      20
#define N_CONSUMER      30

void *add_item(int buf[], int *tail);
void *remove_item(int buf[], int *head);
bool isEmpty(int head, int tail);
bool isFull(int head, int tail);

int main () {
    int buf[N_BUFFER] = {0};
    int buf_head = 0;
    int buf_tail = 0;
}

void *add_item(int buf[], int *tail){
    buf[*tail] = 1;
    *tail++;
    if(*tail >= N_BUFFER)
        *tail -= N_BUFFER;
}

void *remove_item(int buf[], int *head){
    buf[*head] = 0;
    *head++;
    *head %= N_BUFFER;
}

bool isEmpty(int head, int tail){
    if(*h == 0)
        return true;
    else
        return false;
}
bool isFull(int head, int tail){
    if(*t==1)
        return true;
    else
        return false;
}
