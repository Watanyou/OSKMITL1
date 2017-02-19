#include <thread>
#include <stdio.h>
#include <conio.h>
#include <mutex>


#define NUM_THREADS     5
#define N_BUFFER        20
#define N_DATA          100000
#define N_PRODUCER      20
#define N_CONSUMER      30

std::mutex m;
int append_count=0;
int consume_count=0;
int data_count=1000;

void add_item(int buf[], int *tail_ptr);
void remove_item(int buf[], int *head_ptr);
bool isEmpty(int buf[], int head, int tail);
bool isFull(int buf[], int head, int tail);
void append(int buf[],int *head_ptr,int *tail_ptr);
void consume(int buf[],int *head_ptr,int *tail_ptr);

int main () {
    int buf[N_BUFFER] = {0};
    int buf_head = 0;
    int buf_tail = 0;
    std::thread producer[10];
    std::thread consumer[10];

    printf("Start\n");
    for(int i= 0;i<10;i++)
        producer[i] = std::thread(append, buf, &buf_head, &buf_tail);

    for(int i=0 ;i<10;i++){
        producer[i].join();
    }
    for(int i = 0;i<N_BUFFER;i++)
        printf("%d ",buf[i]);

    printf("\n%d ",buf_head);
    printf("\n%d ",buf_tail);
    printf("\n%d ",append_count);
    printf("\n%d ",consume_count);
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

void append(int buf[],int *head_ptr,int *tail_ptr){
    while(data_count>=0){
        m.lock();
        if(!isFull(buf, *head_ptr, *tail_ptr)){
            add_item(buf,tail_ptr);
            append_count++;
            data_count--;
        }
        m.unlock();
    }
}
void consume(int buf[],int *head_ptr,int *tail_ptr){
    m.lock();
    remove_item(buf,head_ptr);
    m.unlock();
    consume_count++;
}
