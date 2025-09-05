#include "step_mtr.h"
#include "gpio_raii.hpp"
#include <chrono>
#include <cstdint>
#include <iostream>
#include <mutex>
#include <thread>
#include <algorithm>


using namespace std;
// a1, a2, b1, b2
static StepMtr::cmd_order full = {0b1010, 0b0110, 0b0101, 0b1001, 0b1010, 0b0110, 0b0101, 0b1001}; // 2 sequence
static StepMtr::cmd_order half = {0b1010, 0b0010, 0b0110, 0b0100, 0b0101, 0b0001, 0b1001, 0b1000};
// half step mode

template <typename T> 
T clamp(const T& in, const T& min, const T& max){
    return  std::min(std::max(in, min), max);
}

StepMtr::StepMtr(const uint16_t a1, const uint16_t a2, const uint16_t b1, const uint16_t b2){
    pins[3] = Gpio(a1, 1);
    pins[2] = Gpio(a2, 1);
    pins[1] = Gpio(b1, 1);
    pins[0] = Gpio(b2, 1);
    started = false;
    set_mode(FULL);
    set_pos_limit(0, 100);
}

int StepMtr::one_step(){
    if(unlikely(started)){
        busy_err();
        return 1;
    }
    started = true;
    do_step();
    started = false;
    return 0;
}

void StepMtr::do_step(){
    // cout << "step (A1 A2 B1 B2) " <<  step_n << " " << (*step_q)[step_n] << " pos: " << _pos << endl;
    started = true;
    for(auto pin_n : {0,1,2,3}){
        bool val = (*step_q)[step_n][pin_n];
        pins[pin_n].write(val);
    }
    const int dir = _dir * inverse_move;
    step_n = (step_n + dir) & (step_q->size()-1);
    _pos+= _dir;
    // cout << "next step " << step_n << endl;
}
int StepMtr::init(const int step_range){
    if(unlikely(started)){
        busy_err();
        return 1;
    }
    assign_pos(0);
    set_pos_limit(-step_range, step_range);
    set_dir(StepMtr::BWD);
    set_speed(100);
    run_pos(pos_limit.first);
    set_pos_limit(0, step_range);
    assign_pos(0);
    return 0;
}

int StepMtr::run_pos(int t_pos){
    if(unlikely(started)){
        busy_err();
        return 1;
    }
    if(unlikely(t_pos < pos_limit.first || t_pos > pos_limit.second))
        return 1;
    const int32_t steps = int32_t(t_pos - _pos);
    set_dir(steps < 0 ? BWD : FWD);
    started = true;
    {
        lock_guard<mutex>lk(notice_mx);
        notice.notify_one();
    }
    while(this->get_pos() != t_pos && started){
        do_step();
        std::this_thread::sleep_for(std::chrono::milliseconds(step_interval));
    }
    started = false;
    // cout << "pos: " << pos << endl;
    return 0;
}

int StepMtr::set_mode(Mode mod){
    if(unlikely(started)){
        busy_err();
        return 1;
    }
    if(mod == HALF) step_q = &half;
    else step_q = &full; 
    return 0;
}

// infinity run
int StepMtr::run(){
    if(unlikely(started)){
        busy_err();
        return 1;
    }
    started = true;
    {
        lock_guard<mutex>lk(notice_mx);
        notice.notify_one();
    }
    while(this->started){
        this->do_step();
        std::this_thread::sleep_for(std::chrono::milliseconds(step_interval));
    }
    started = false;
    return 0;
}

void StepMtr::stop(){
    started = false;
    for(auto &pin : pins){
        pin.reset();
    }
}