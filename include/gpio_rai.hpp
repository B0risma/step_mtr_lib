#pragma once
#include "gpio.h"
#include "color_term.h"
#include <iostream>
#include <ostream>
#include <string>
#include <type_traits>
#include <unistd.h>

inline void pr_err(const std::string &errStr){
    std::cout << CLI_RED << errStr << CLI_RESET << std::endl;
}

class Gpio {
public:
    Gpio(const uint16_t pin, bool dir, const char * name = "\0"):_pin(pin), _name(name){
        _desc = gpio_new();
        if(_desc){
            gpio_open_sysfs(_desc, pin, dir ? GPIO_DIR_OUT : GPIO_DIR_IN);
        }else{
            pr_err("GPIO err");
        }
    }
    Gpio(){};
    Gpio(Gpio &&other){
        *this = std::move(other);
    }
    Gpio & operator = (Gpio && other){
        _desc = other._desc;
        _pin = other._pin;
        _name = other._name;
        
        other._desc = 0;
        other._pin = 0;
        other._name = "";
        return *this;
    }
    void write(const bool value){
        // std::cout << CLI_BLUE << "GPIO#" << _pin << "(" << _name << ") "<< value << CLI_RESET << std::endl;
        if(!_desc) return;
        auto err = gpio_write(_desc, value);
        if(err){
            pr_err(std::string("gpio_write err") + std::to_string(err));
        }
        usleep(200); //need it for stable pin write!!!!!
    }
    inline void set(){write(true);}
    inline void reset(){write(false);}
    ~Gpio(){
        if(!_desc) return;
        gpio_direction_t dir;
        gpio_get_direction(_desc, &dir);
        if(dir == GPIO_DIR_OUT){
            reset();
        }
        auto er =  gpio_close(_desc);
        gpio_free(_desc);
        _desc = 0;
    }
private:
    gpio_t *_desc;
    uint16_t _pin;
    const char* _name;
};