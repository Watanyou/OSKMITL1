#include <thread>
#include <conio.h>
#include <mutex>
#include <iostream>
#include <time.h>

using namespace std;


std::mutex m;
int buf_size=0;
int in_req=0;
int out_req=0;
int buf_req=0;

void add_item(bool buf[], int *tail_ptr);
void remove_item(bool buf[], int *head_ptr);
bool isEmpty(bool buf[], int head, int tail);
bool isFull(bool buf[], int head, int tail);
void append(int id, bool buf[],int *head_ptr,int *tail_ptr);
void consume(int id, bool buf[],int *head_ptr,int *tail_ptr);
bool isTimeout(double time);

int main (int argc, char* argv[]) {
    if(argc != 5){ /*Protect program from missing argument*/
        cout << "Missing or More Argument input" << endl;
        return 1;
    }

    int producer_size = 0, consumer_size = 0, req_size = 0; //Initiate size

    /*Scan from argv and convert to integer*/
    sscanf(argv[1], "%d", &producer_size);
    sscanf(argv[2], "%d", &consumer_size);
    sscanf(argv[3], "%d", &buf_size);
    sscanf(argv[4], "%d", &req_size);

    cout<<"Producers "<<producer_size;
    cout<<" ,Consumers "<<consumer_size<<endl;
    cout<<"Buffer size "<<buf_size<<endl;
    cout<<"Requests "<<req_size<<endl<<endl;

    in_req = req_size;
    bool *buf = new bool[buf_size];
    int buf_head = 0;
    int buf_tail = 0;
    thread producer[producer_size];
    thread consumer[consumer_size];

    for(int i = 0; i< buf_size; i++)
        buf[i] = 0;

    double startTime = clock();

    for(int i= 0;i<producer_size;i++){
        producer[i] = thread(append, i, buf, &buf_head, &buf_tail);
    }
    for(int i= 0;i<consumer_size;i++){
        consumer[i] = thread(consume, i, buf, &buf_head, &buf_tail);
    }
    for(int i=0 ;i<producer_size;i++){
        producer[i].join();
    }
    for(int i=0 ;i<consumer_size;i++){
        consumer[i].join();
    }

    double finishTime = clock();

    double reqrate = ((double)out_req/(double)req_size)*100;
    double Elapsed_Time = (finishTime - startTime)/1000;
    double Throughput = out_req/Elapsed_Time;

    printf("Successfully consumed %d requests (%.2f\%)\n", out_req, reqrate);
    printf("Elapsed Time: %.3f s\n", Elapsed_Time);
    printf("Throughput: %.2f successful requests/s\n", Throughput);
    delete(buf);

}

void add_item(bool buf[], int *tail_ptr){
    buf[*tail_ptr] = 1;
    (*tail_ptr)++;
    if(*tail_ptr >= buf_size)
        *tail_ptr -= buf_size;
}

void remove_item(bool buf[], int *head_ptr){
    buf[*head_ptr] = 0;
    (*head_ptr)++;
    if(*head_ptr >= buf_size)
        *head_ptr -= buf_size;
}

bool isEmpty(bool buf[], int head, int tail){
    return !buf[head];
}
bool isFull(bool buf[], int head, int tail){
    return buf[tail];
}

void append(int id, bool buf[],int *head_ptr,int *tail_ptr){
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
void consume(int id, bool buf[],int *head_ptr,int *tail_ptr){
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
