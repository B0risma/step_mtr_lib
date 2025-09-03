#include <future>
#include <iostream>
#include <bitset>
#include <array>
#include <chrono>
#include <mutex>
#include <thread>
#include "step_mtr.h"
#include <unistd.h>

using namespace std;

int main(int argc, char ** argv){
    
    StepMtr mtr({104, 106, 108, 113});
    mtr.set_dir(StepMtr::BWD);
    mtr.set_speed(10);
    auto fut = std::async(std::launch::async, &StepMtr::run, &mtr);
    sleep(2);
    mtr.stop();
    fut.wait();

    sleep(1);
    mtr.reset();

    mtr.set_dir(StepMtr::FWD);
    fut = std::async(std::launch::async, &StepMtr::run, &mtr);
    sleep(2);
    mtr.stop();
    fut.wait();
    // mtr.set_dir(StepMtr::BWD);
    // cout << "BWD to 0" << endl;
    // fut = std::async(std::launch::async, &StepMtr::run_pos, &mtr, 0);
    // fut.wait();
}

int async_example(){
    StepMtr mtr({104, 106, 108, 113});
    mtr.set_dir(StepMtr::FWD);
    mtr.set_speed(100);
    auto fut = std::async(std::launch::async, &StepMtr::run_pos, &mtr, 100);
    mtr.wait_start();
    while(mtr.is_busy()){
        cout << "wait" << endl;
        this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    fut.wait();

    fut = async(launch::async, &StepMtr::run, &mtr);
    mtr.wait_start();
    cout << "wait loop" << endl;
    for(auto i = 0; i < 10; ++i){
        this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    mtr.stop();
    fut.wait();
}