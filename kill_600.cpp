#include <cstddef>
#include <functional>
#include <iostream>
#include <iomanip>
#include <list>
#include <vector>
#include <thread>
#include <random>


using namespace std;

constexpr size_t PEOPLE_COUNT=2*2*2*2*2*2*2*2*2*2;
constexpr int thread_loop_count=1e5;
constexpr int thread_count=16;
constexpr int remain_people_count=1;


struct thread_data{
    long long counts[PEOPLE_COUNT]={};
    long long rounds[PEOPLE_COUNT]={};
};

void thread_func(thread_data& data){
    default_random_engine engine(random_device{}());
    // boost::pool<> pool(sizeof(int));   
    // boost::pool_allocator<int> alocator;

    for(int i=0;i<thread_loop_count;i++){
        // list<int,boost::pool_allocator<int>> people(PEOPLE_COUNT,0,alocator);
        list<int> people(PEOPLE_COUNT,0);

        int num=1;
        for(auto& j:people){
            j=num;
            num++;
        }

        while(people.size()!=remain_people_count){
            uniform_int_distribution<> int_dis(1,(people.size()+1)/2);

            auto iter=people.begin();

            std::advance(iter,int_dis(engine)*2-1-1);

            people.erase(iter);

            for(auto& i:people){
                data.rounds[i-1]+=1;
            }
            
        }
        for(auto& i:people){
            data.counts[i-1]+=1; 
        }
        // if(i%(int)(5*1e4)==0){
        //     cout<<"一个线程算了5*1e4次模拟"<<endl;
        // }
    }
    for(int i=0;i<PEOPLE_COUNT;i++){
        data.rounds[i]/=thread_loop_count; 
    }
}


int main(){

    thread_data datas[thread_count];

    vector<thread> threads;
    for(int i=0;i<thread_count;i++){
        threads.emplace_back(std::bind(thread_func,std::ref(datas[i])));
    }

    for(auto& i:threads){
        i.join();
    }

    thread_data final_data;
    for(auto& i:datas){
        for(int j=0;j<PEOPLE_COUNT;j++){
            final_data.counts[j]+=i.counts[j];
            final_data.rounds[j]+=i.rounds[j];
        }
    }
    for(int j=0;j<PEOPLE_COUNT;j++){
        final_data.rounds[j]/=thread_count;
    }

    for(int j=0;j<PEOPLE_COUNT;j++){
        cout<<j+1<<"\t最后存活概率："<<std::fixed << std::setprecision(9) <<(double)final_data.counts[j]/(thread_count*thread_loop_count)<<"\t平均存活轮数："<<final_data.rounds[j]<<endl;
    }

    

    return 0;

}
