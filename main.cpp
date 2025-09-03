#include <cstdint>
#include <cstdio>
#include <fstream>
#include <future>
#include <ios>
#include <iostream>
#include <bitset>
#include <array>
#include <chrono>
#include <mutex>
#include <string>
#include <thread>
#include "step_mtr.h"
#include <unistd.h>
#include <cstring>
#include <vector>
#include "gpio_rai.hpp"
#include <csignal>

using namespace std;

inline void clear_term(){
    std::cout << "\033c";
}

bool started = true;

void exec_cmd();

void init_mtr(StepMtr &mtr){
    
}

int main(int argc, char ** argv){
    // exec_cmd();
    StepMtr mtr({104,106, 108, 113});
    mtr.init();
    char ch = 0;
    while ((ch=getchar()) != 'q') {
        if(ch == '+')
            mtr.set_dir(StepMtr::FWD);
        else if( ch == '-')
            mtr.set_dir(StepMtr::BWD);
        else continue;
        mtr.next_step();
        std::cout << "pos: " << mtr.get_pos() << endl;
    }
}

void snif_states(){
    auto pins = {104, 106, 108, 113};
    auto gpio_h = "/sys/class/gpio/gpio";
    char buf[1024];
    bitset<4> zero;
    bitset<4> last_cmd; 
    std::vector<uint8_t> log;
    log.reserve(100000);
    const auto start = std::chrono::system_clock::now();

    signal(SIGINT, [](int signal){
        cout << "exit\n";
        started = false;
    });
    while(started){//std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - start).count() < 15){
        clear_term();
        bitset<4> cmd = 0;
        for(auto i = 0; i < 4; ++i){
            auto pin = *(pins.begin()+i);
            memset(buf, 0, 1024);
            ifstream in;
            in.open(gpio_h+std::to_string(pin)+"/value");
            if(!in.is_open()) continue;
            in.read(buf, 1024);
            cmd[i] = atoi(buf);
            // cout << pin << ": " << buf << endl;
        }
        // cout << cmd << endl;
        if(cmd != last_cmd && cmd != zero){
            log.push_back(cmd.to_ulong() & 0xFF);
            last_cmd = cmd;
        }
        usleep(10000);
    }
    ofstream out("log", ios_base::binary);
    if(out.is_open()){
        out.write((char*)log.data(), log.size());
    }
}

void exec_cmd(){
    string in;
    array<Gpio, 4> pins;
    auto pin_arr = {104, 106, 108, 113};
    for( auto i = 0; i < pins.size(); ++i){
        int pin_n = *(pin_arr.begin() + i);
        pins[i] = Gpio(pin_n, 1);
    }
    while(true){
        cin >> in;
        bitset<4> cmd(in);
        cout << "cmd: " << cmd << endl;
        for(auto pin_n = 0; pin_n < pins.size(); ++pin_n){
            auto &pin = pins[pin_n];
            pin.write(cmd[pin_n]);
        }
    }
}

void async_example(){
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


//direction test
// {
    //     StepMtr mtr({104, 106, 108, 113});
    //     mtr.set_dir(StepMtr::Dir(atoi(argv[1])));
    //     mtr.set_speed(10);
    //     auto fut = std::async(std::launch::async, &StepMtr::run, &mtr);
    //     sleep(3);
    //     mtr.stop();
    //     fut.wait();
    // }

    // sleep(2);
    // cout << "swap pins" << endl;

    // {
    //     StepMtr mtr({106, 104, 113, 108});
    //     mtr.set_dir(StepMtr::BWD);
    //     mtr.set_speed(10);
    //     auto fut = std::async(std::launch::async, &StepMtr::run, &mtr);
    //     sleep(3);
    //     mtr.stop();
    //     fut.wait();
    // }
    // sleep(2);
    // cout << "forward" << endl;
    // {
    //     StepMtr mtr({104, 106, 108, 113});
    //     mtr.set_dir(StepMtr::FWD);
    //     mtr.set_speed(10);
    //     auto fut = std::async(std::launch::async, &StepMtr::run, &mtr);
    //     sleep(3);
    //     mtr.stop();
    //     fut.wait();
    // }
    // sleep(2);
    // cout << "forward swap" << endl;
    // {
    //     StepMtr mtr({106, 104, 113, 108});
    //     mtr.set_dir(StepMtr::FWD);
    //     mtr.set_speed(10);
    //     auto fut = std::async(std::launch::async, &StepMtr::run, &mtr);
    //     sleep(3);
    //     mtr.stop();
    //     fut.wait();
    // }
    // mtr.set_dir(StepMtr::BWD);
    // cout << "BWD to 0" << endl;
    // fut = std::async(std::launch::async, &StepMtr::run_pos, &mtr, 0);
    // fut.wait();