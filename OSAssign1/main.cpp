#include <thread>
#include <conio.h>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <chrono>

using namespace std;


std::mutex m;

int buf_size=0;
int in_req=0;
int out_req=0;


void add_item(bool buf[], int *tail_ptr);
void remove_item(bool buf[], int *head_ptr);
void append(bool buf[],int *head_ptr,int *tail_ptr);
void consume(bool buf[],int *head_ptr,int *tail_ptr);

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

    chrono::time_point<std::chrono::system_clock> startTime, finishTime;


    startTime = chrono::system_clock::now();
    for(int i=0;i<producer_size || i<consumer_size ;i++){
        if(i<producer_size)
            producer[i] = thread(append, buf, &buf_head, &buf_tail);
        if(i<consumer_size)
            consumer[i] = thread(consume, buf, &buf_head, &buf_tail);
    }
    for(int i=0;i<producer_size || i<consumer_size ;i++){
        if(i<producer_size)
            producer[i].join();
        if(i<consumer_size)
            consumer[i].join();
    }
    finishTime = chrono::system_clock::now();



    double reqrate = ((double)out_req/(double)req_size)*100.0;
    chrono::duration<double> Elapsed_Time = finishTime-startTime;
    double Throughput = out_req/Elapsed_Time.count();

    printf("Successfully consumed %d requests (%.2f\%)\n", out_req, reqrate);
    printf("Elapsed Time: %.3f s\n", Elapsed_Time);
    printf("Throughput: %.2f successful requests/s\n", Throughput);
    delete(buf);

}

void add_item(bool buf[], int *tail_ptr){
    buf[(*tail_ptr)++] = 1;
    in_req--;
    if(*tail_ptr >= buf_size)
        *tail_ptr -= buf_size;
}

void remove_item(bool buf[], int *head_ptr){
    buf[(*head_ptr)++] = 0;
    out_req++;
    if(*head_ptr >= buf_size)
        *head_ptr -= buf_size;
}


void append(bool buf[],int *head_ptr,int *tail_ptr){
    while(1){
        m.lock();
        if(in_req>0){
            if(!buf[*tail_ptr]){
                add_item(buf,tail_ptr);
            }
        }else{
            m.unlock();
            return;
        }
        m.unlock();
    }
}



void consume(bool buf[],int *head_ptr,int *tail_ptr){
    while(1){
        m.lock();
        if(in_req > 0 || buf[*head_ptr]){
            if(buf[*head_ptr]){
                remove_item(buf,head_ptr);
            }
        }else{
            m.unlock();
            return;
        }
        m.unlock();
    }
}
