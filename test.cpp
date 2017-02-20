#include <thread>
#include <conio.h>
#include <mutex>
#include <iostream>
#include <time.h>

using namespace std;


#define N_PRODUCER      20
#define N_CONSUMER      30

std::mutex m;
int append_count=0;
int consume_count=0;
int buf_size=0;
int data_count=0;
int req=0;

void add_item(int buf[], int *tail_ptr);
void remove_item(int buf[], int *head_ptr);
bool isEmpty(int buf[], int head, int tail);
bool isFull(int buf[], int head, int tail);
void append(int buf[],int *head_ptr,int *tail_ptr);
void consume(int buf[],int *head_ptr,int *tail_ptr);
bool isTimeout(int time);

int main () {
    int prod = 20;
    int consm = 30;
    buf_size = 1000;
    data_count = 100000;
    int *buf = new int[buf_size];
    int buf_head = 0;
    int buf_tail = 0;
    int startTime;
    int finishTime;
    thread producer[prod];
    thread consumer[consm];

    startTime = clock();
    for(int i= 0;i<prod;i++){
        producer[i] = thread(append, buf, &buf_head, &buf_tail);
    }
    for(int i= 0;i<consm;i++){
        consumer[i] = thread(consume, buf, &buf_head, &buf_tail);
    }
    for(int i=0 ;i<prod;i++){
        producer[i].join();
    }
    for(int i=0 ;i<consm;i++){
        consumer[i].join();
    }
    finishTime = clock();

    cout << "Request " << req << " Times" << endl;
    cout << "Append " << append_count << " Times" << endl;
    cout << "Consume " << consume_count << " Times" << endl;
    cout << "Time use " << finishTime-startTime << " ms" << endl;
    delete(buf);
}

void add_item(int buf[], int *tail_ptr){
    buf[*tail_ptr] = 1;
    (*tail_ptr)++;
    if(*tail_ptr >= buf_size)
        *tail_ptr -= buf_size;
}

void remove_item(int buf[], int *head_ptr){
    buf[*head_ptr] = 0;
    (*head_ptr)++;
    if(*head_ptr >= buf_size)
        *head_ptr -= buf_size;
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
    int timer=0;
    while(1){
        m.lock();
        if(data_count>0){
            if(!isFull(buf, *head_ptr, *tail_ptr)){
                add_item(buf,tail_ptr);
                append_count++;
                data_count--;
                req++;
            }else{
                if(timer!=0){//Timer is Running
                    if(isTimeout(timer)){
                        data_count--;
                        timer = 0;
                    }
                }else
                    timer = clock();
            }
        }else{
            m.unlock();
            return;
        }
        m.unlock();
    }
}
void consume(int buf[],int *head_ptr,int *tail_ptr){
    int timer;
    while(1){
        m.lock();
        if(data_count>0){
            if(!isEmpty(buf,*head_ptr, *tail_ptr)){
                remove_item(buf,head_ptr);
                consume_count++;
                data_count--;
                req++;
            }else{
                if(timer!=0){//Timer is Running
                    if(isTimeout(timer)){
                        data_count--;
                        timer = 0;
                    }
                }else
                    timer = clock();
            }
        }else{
            m.unlock();
            return;
        }
        m.unlock();
    }
}

bool isTimeout(int time){
    int thisTime = clock();
    if(thisTime-time>=10)
        return true;
    else
        return false;

}

