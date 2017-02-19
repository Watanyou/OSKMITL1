#include <pthread.h>
#include <stdio.h>
#include <conio.h>

#define NUM_THREADS     5
#define N_BUFFER        20
#define N_DATA          100000
#define N_PRODUCER      20
#define N_CONSUMER      30

void add_item(int buf[], int *tail_ptr);
void remove_item(int buf[], int *head_ptr);
bool isEmpty(int buf[], int head, int tail);
bool isFull(int buf[], int head, int tail);

int main () {
    int buf[N_BUFFER] = {0};
    int buf_head = 0;
    int buf_tail = 0;

    for(int i = 1;i<=10;i++)
        add_item(buf,&buf_tail);
    for(int i = 1;i<=3;i++)
        remove_item(buf,&buf_head);
    for(int i = 0;i<N_BUFFER;i++)
        printf("%d ",buf[i]);
    printf("\n%d ",buf_head);
    printf("\n%d ",buf_tail);
    printf("\n%d ",isEmpty(buf,buf_head,buf_tail));

}

void add_item(int buf[], int *tail_ptr){
    buf[*tail_ptr] = 1;
    (*tail_ptr)++;
    if(*tail_ptr >= N_BUFFER)
        *tail_ptr -= N_BUFFER;
}

void remove_item(int buf[], int *head_ptr){
    buf[*head_ptr] = 0;
    (*head_ptr)++;
    *head_ptr %= N_BUFFER;
}

bool isEmpty(int buf[], int head, int tail){
    if(buf[head] == 0)
        return true;
    else
        return false;
}
bool isFull(int buf[], int head, int tail){
    if(buf[tail]==1)
        return true;
    else
        return false;
}
