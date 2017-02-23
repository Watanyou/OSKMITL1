#include <thread>
#include <conio.h>
#include <mutex>
#include <iostream>
#include <time.h>

using namespace std;


std::mutex m;
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

int main (int argc, char* argv[]) {
    cout<<"Producers "<<atoi(argv[1]);
    cout<<" ,Consumers "<<atoi(argv[2])<<endl;
    cout<<"Buffer size "<<atoi(argv[3])<<endl;
    cout<<"Requests "<<atoi(argv[4])<<endl<<endl;

    buf_size = atoi(argv[3]);
    int data_size = atoi(argv[4]);
    data_count = data_size;
    int *buf = new int[buf_size];
    int buf_head = 0;
    int buf_tail = 0;
    int startTime;
    int finishTime;
    thread producer[atoi(argv[1])];
    thread consumer[atoi(argv[2])];

    for(int i = 0; i< buf_size; i++)
        buf[i] = 0;

    startTime = clock();
    for(int i= 0;i<atoi(argv[1]);i++){
        producer[i] = thread(append, buf, &buf_head, &buf_tail);
    }
    for(int i= 0;i<atoi(argv[2]);i++){
        consumer[i] = thread(consume, buf, &buf_head, &buf_tail);
    }
    for(int i=0 ;i<atoi(argv[1]);i++){
        producer[i].join();
    }
    for(int i=0 ;i<atoi(argv[2]);i++){
        consumer[i].join();
    }
    finishTime = clock();

    double reqrate = ((double)req/(double)data_size)*100;
    double Elapsed_Time = (finishTime-startTime)/1000.0;
    double Throughput = req/Elapsed_Time;

    printf("Successfully consumed %d requests (%.2f\%)\n", req, reqrate);
    printf("Elapsed Time: %.3f s\n", Elapsed_Time);
    printf("Throughput: %.2f successful requests/s\n", Throughput);
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
            //cout << "Append " << "Head " << *head_ptr << "Tail " << *tail_ptr << "Buf " << buf[*tail_ptr] << endl;
            if(!isFull(buf, *head_ptr, *tail_ptr)){
                timer = 0;
                add_item(buf,tail_ptr);
                data_count--;
                req++;
            }else{
                //cout << "Append Waiting " << timer << endl;
                if(timer!=0){//Timer is Running
                    if(isTimeout(timer)){
                        //cout << "Time Out" << endl;
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
            //cout << "Remove " << "Head " << *head_ptr << "Tail " << *tail_ptr << "Buf " << buf[*head_ptr] << endl;
            if(!isEmpty(buf,*head_ptr, *tail_ptr)){
                timer = 0;
                remove_item(buf,head_ptr);
                data_count--;
                req++;
            }else{
                //cout << "Remove Waiting " << timer << endl;
                if(timer!=0){//Timer is Running
                    if(isTimeout(timer)){
                        //cout << "Time Out" << endl;
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

