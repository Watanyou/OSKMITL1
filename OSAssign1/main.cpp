#include <thread>
#include <conio.h>
#include <mutex>
#include <iostream>
#include <chrono>

using namespace std;


std::mutex m;
int buf_size=0;
int in_req=0;
int out_req=0;
int buf_req=0;

void add_item(int buf[], int *tail_ptr);
void remove_item(int buf[], int *head_ptr);
bool isEmpty(int buf[], int head, int tail);
bool isFull(int buf[], int head, int tail);
void append(int id, int buf[],int *head_ptr,int *tail_ptr);
void consume(int id, int buf[],int *head_ptr,int *tail_ptr);
bool isTimeout(double time);

int main (int argc, char* argv[]) {
    cout<<"Producers "<<atoi(argv[1]);
    cout<<" ,Consumers "<<atoi(argv[2])<<endl;
    cout<<"Buffer size "<<atoi(argv[3])<<endl;
    cout<<"Requests "<<atoi(argv[4])<<endl<<endl;

    buf_size = atoi(argv[3]);
    in_req = atoi(argv[4]);
    int *buf = new int[buf_size];
    int buf_head = 0;
    int buf_tail = 0;
    thread producer[atoi(argv[1])];
    thread consumer[atoi(argv[2])];

    for(int i = 0; i< buf_size; i++)
        buf[i] = 0;

    auto startTime = std::chrono::high_resolution_clock::now();

    for(int i= 0;i<atoi(argv[1]);i++){
        producer[i] = thread(append, i, buf, &buf_head, &buf_tail);
    }
    for(int i= 0;i<atoi(argv[2]);i++){
        consumer[i] = thread(consume, i, buf, &buf_head, &buf_tail);
    }
    for(int i=0 ;i<atoi(argv[1]);i++){
        producer[i].join();
    }
    for(int i=0 ;i<atoi(argv[2]);i++){
        consumer[i].join();
    }

    auto elapsed = std::chrono::high_resolution_clock::now() - startTime;

    double reqrate = ((double)out_req/(double)atoi(argv[4]))*100;
    double Elapsed_Time = std::chrono::duration_cast<std::chrono::seconds>(elapsed).count();
    double Throughput = out_req/(std::chrono::duration_cast<std::chrono::seconds>(elapsed).count());

    printf("Successfully consumed %d requests (%.2f\%)\n", out_req, reqrate);
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

void append(int id, int buf[],int *head_ptr,int *tail_ptr){
    double timer=0;
    while(1){
        m.lock();
        if(in_req>0){
            //cout << id << " Append " << " Head " << *head_ptr << " Tail " << *tail_ptr << " Buf " << buf[*tail_ptr] << endl;
            if(!isFull(buf, *head_ptr, *tail_ptr)){
                timer = 0;
                add_item(buf,tail_ptr);
                in_req--;
            }else{
                //cout << "Append Waiting " << timer << endl;
                if(timer!=0){//Timer is Running
                    if(isTimeout(timer)){
                        //cout << "Time Out" << endl;
                        in_req--;
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
void consume(int id, int buf[],int *head_ptr,int *tail_ptr){
    while(1){
        m.lock();
        if(in_req > 0 || !isEmpty(buf,*head_ptr, *tail_ptr)){
            //cout << id << " Remove " << " Head " << *head_ptr << " Tail " << *tail_ptr << " Buf " << buf[*head_ptr] << endl;
            if(!isEmpty(buf,*head_ptr, *tail_ptr)){
                remove_item(buf,head_ptr);
                out_req++;
            }
        }else{
            m.unlock();
            return;
        }
        m.unlock();
    }
}

bool isTimeout(double time){
    double thisTime = clock();
    if(thisTime-time>=1)
        return true;
    else
        return false;

}


