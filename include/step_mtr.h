#pragma once
#include <array>
#include <bitset>
#include <initializer_list>
#include <mutex>
#include <thread>
#include <atomic>
#include "gpio_rai.hpp"
#include <condition_variable>


class StepMtr{
public:
    StepMtr(){};
    StepMtr(const std::initializer_list<int> &list);
    ~StepMtr(){
        stop();
    }
    // step run
    void next_step();
    void run_pos(int pos);
    inline void reset(){
        pos = 0;
        step_n = 0;
    }

    // infinity run
    void run();
    void stop();
    enum Mode{
        FULL,
        WAVE,
        HALF
    };
    enum Dir {
        FWD = 1,
        BWD = -1
    };
    void set_mode(Mode mod);
    inline bool is_busy() const {return busy;}
    inline bool is_started() const{return started;}

    inline void set_dir(Dir dir){this->_dir = dir;}
    inline void set_speed(int spd){speed = abs(spd);}
    inline void wait_start(){
        std::unique_lock<std::mutex> lk(notice_mx);
        notice.wait(lk, [&](){return is_started();});
    }
    
    using step_cmd = std::bitset<4>; ///< A1 - MSB

    std::condition_variable notice;
    std::mutex notice_mx;
private:
    // a1, a2, b1, b2
    std::array<step_cmd, 4> step_q = {0b1010, 0b0110, 0b0101, 0b1001};
    int step_n = 0;
    bool mtr_started = false;
    int _dir = 1; //-1 1
    //B2 B1 A2 A1 - reverse order for direct bit access
    std::array<Gpio, 4> pins;
    int pos = 0;
    //! steps/second
    int speed = 1;

    std::atomic<bool> started;
    std::atomic<bool> busy;

    std::pair<int, int> pos_limit = {0, 100};
};