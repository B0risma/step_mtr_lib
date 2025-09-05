#pragma once
#include <array>
#include <bitset>
#include <initializer_list>
#include <mutex>
#include <ostream>
#include <thread>
#include <atomic>
#include "color_term.h"
#include "gpio_raii.hpp"
#include <condition_variable>


#define likely(x) __builtin_expect(!!x, 1)
#define unlikely(x) __builtin_expect(!!x, 0)

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

    // StepMtr(){};
    StepMtr(const uint16_t a1, const uint16_t a2, const uint16_t b1, const uint16_t b2);
    ~StepMtr(){
        stop();
    }
    //! step_range - maximum mtr steps
    int init(const int step_range);
    //! exec step
    int one_step();
    int run_pos(int pos);
    
    //! infinity run
    int run();
    //! stop mtr
    void stop();
    //! wait helper for async calls
    inline void wait_start(){
        std::unique_lock<std::mutex> lk(notice_mx);
        notice.wait(lk, [&](){return is_started();});
    }
    
    inline int assign_pos(const int pos){
        if(unlikely(started)){
            busy_err();
            return 1;
        } 
        _pos = pos;
        return 0;
    }
    //! check started
    inline int get_pos(){return _pos;}
    int set_mode(Mode mode);
    inline int set_pos_limit(int min, int max){
        if(unlikely(started)){
            busy_err();
            return 1;
        }
        pos_limit = {min,max};
        return 0;
    }
    //! for manual run
    inline int set_dir(Dir dir){
        if(unlikely(started)){
            busy_err();
            return 1;
        }
        _dir = dir;
        return 0;
    }
    //! swap directions
    inline int inverse_dir(bool inv = true){
        if(started) {
            busy_err();
            return 1;
        }
        inverse_move = inv ? -1 : 1;
        return 1;
    }
    //! steps/second
    inline int set_speed(int spd){
        if(unlikely(started)){
            busy_err();
            return 1;
        }
        step_interval = 1000/abs(spd);
        return 0;
    }

    inline bool is_busy() const {return started;}
    inline bool is_started() const{return started;}
    //! noticed when mtr starts continuous run
    std::condition_variable notice;
    std::mutex notice_mx;
    //! a1, a2, b1, b2
    using cmd_order = std::array<std::bitset<4>, 8>; ///< for one cmd A1 pin = MSB
private:
    inline void busy_err(){
        #ifndef NDEBUG
        std::cout << CLI_RED << "mtr is busy!" << CLI_RESET << std::endl;
        #endif
    }
    //! raw step without protection
    void do_step();

    cmd_order * step_q = 0;
    int step_n = 0;
    //! inverse movement dir without pos invertion
    int inverse_move = 1;
    int _dir = 1; //-1 1
    //B2 B1 A2 A1 - reverse order for direct bit access
    std::array<Gpio, 4> pins;
    std::atomic<int> _pos;
    
    //! in milisec = 1000ms/speed
    int step_interval = 1000;

    std::atomic<bool> started;
    std::pair<int, int> pos_limit;
};