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
    enum Mode{
        FULL,
        HALF
    };
    enum Dir {
        FWD = 1,
        BWD = -1
    };

    StepMtr(){};
    StepMtr(const std::initializer_list<int> &list);
    ~StepMtr(){
        stop();
    }
    void init(const int step_range);
    //! exec next step
    void do_step();
    void run_pos(int pos);
    
    //! infinity run
    void run();
    //! stop mtr
    void stop();
    //! wait helper for async calls
    inline void wait_start(){
        std::unique_lock<std::mutex> lk(notice_mx);
        notice.wait(lk, [&](){return is_started();});
    }
    
    inline void assign_pos(const int pos){_pos = pos;}
    inline int get_pos(){return _pos;}
    void set_mode(Mode mode);
    inline void set_pos_limit(int min, int max){pos_limit = {min,max};}
    inline void set_dir(Dir dir){this->_dir = dir;}
    //! steps/second
    inline void set_speed(int spd){step_interval = 1000/abs(spd);}

    inline bool is_busy() const {return busy;}
    inline bool is_started() const{return started;}
    //! noticed when mtr starts continuous run
    std::condition_variable notice;
    std::mutex notice_mx;

    using cmd_order = std::array<std::bitset<4>, 8>; ///< for one cmd A1 pin = MSB
private:
    // a1, a2, b1, b2
    cmd_order * step_q = 0;
    int step_n = 0;
    bool mtr_started = false;
    int _dir = 1; //-1 1
    //B2 B1 A2 A1 - reverse order for direct bit access
    std::array<Gpio, 4> pins;
    std::atomic<int> _pos = 0;
    
    //! in milisec = 1000ms/speed
    int step_interval = 1;

    std::atomic<bool> started;
    std::atomic<bool> busy;
    std::pair<int, int> pos_limit;
};