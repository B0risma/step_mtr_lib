#include "step_mtr.h"
#include "gpio_rai.hpp"
#include <chrono>
#include <cstdint>
#include <iostream>
#include <mutex>
#include <thread>
#include <algorithm>


using namespace std;
// a1, a2, b1, b2
static array<StepMtr::step_cmd, 4> full = {0b1010, 0b0110, 0b0101, 0b1001};
static array<StepMtr::step_cmd, 4> wave = {0b1000, 0b0010, 0b01000, 0b0001}; //dont sure
// half step mode

template <typename T> 
T clamp(const T& in, const T& min, const T& max){
    return  std::min(std::max(in, min), max);
}

StepMtr::StepMtr(const std::initializer_list<int> &list){
    pins[3] = Gpio(*list.begin(), 1); //A1
    pins[2] = Gpio(*(list.begin()+1), 1); //A2
    pins[1] = Gpio(*(list.begin()+2), 1); //B1
    pins[0] = Gpio(*(list.begin()+3), 1); //B2

    //for another direction
    // pins[0] = Gpio(*list.begin(), 1, "A1");
    // pins[1] = Gpio(*(list.begin()+1), 1, "A2");
    // pins[2] = Gpio(*(list.begin()+2), 1, "B1");
    // pins[3] = Gpio(*(list.begin()+3), 1, "B2");
    busy = false;
    started = false;
}

void StepMtr::next_step(){
    cout << "step (A1 A2 B1 B2) " <<  step_n << " " << step_q[step_n] << " pos: " << pos << endl;
    for(auto pin_n : {0,1,2,3}){
        bool val = step_q[step_n][pin_n];
        pins[pin_n].write(val);
    }
    step_n = (step_n+_dir) & 3;
    pos+= _dir;
    cout << "next step " << step_n << endl;
}
void StepMtr::init(const int step_range){
    reset();
    set_pos_limit(-step_range, step_range);
    set_dir(StepMtr::BWD);
    set_speed(100);
    run_pos(pos_limit.first);
    set_pos_limit(0, step_range);
    reset();
}

void StepMtr::run_pos(int t_pos){
    int tmp_pos = clamp(t_pos, pos_limit.first, pos_limit.second);
    if(tmp_pos != t_pos || t_pos == pos) return;
    const int32_t steps = int32_t(t_pos - pos);
    set_dir(steps < 0 ? BWD : FWD);
    std::cout << __func__ << " dir " << _dir << endl;
    // steps = abs(steps);
    busy = true;
    started = true;
    int sleep_ms = 1000/this->speed; //ms
    {
        lock_guard<mutex>lk(notice_mx);
        notice.notify_one();
    }
    while(this->get_pos() != t_pos && started){
        next_step();
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_ms));
    }
    busy = false;
    started = false;
    cout << "pos: " << pos << endl;
    
    
}


void StepMtr::set_mode(Mode mod){
    switch(mod){
        case FULL: step_q = full; break;
        case WAVE: step_q = wave; break;
        case HALF: step_q = full; break; // not implemented
    }
}
// infinity run
void StepMtr::run(){
    if(busy) return;

    busy = true;
    started = true;
    int sleep_ms = 1000/this->speed; //ms
    {
        lock_guard<mutex>lk(notice_mx);
        notice.notify_one();
    }
    while(this->started){
        this->next_step();
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_ms));
    }
    busy = false;
    started = false;
    
}
void StepMtr::stop(){
    started = false;
    for(auto &pin : pins){
        pin.reset();
    }
}