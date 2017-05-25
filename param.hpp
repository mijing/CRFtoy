/***************************************************************************
 * 
 * Copyright (c) 2017 Wang Yiwen. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file param.h
 * @author yiwenwang(@aliyun.com)
 * @date 2017/03/21 13:10:51
 * @brief 
 *  
 **/




#ifndef  __PARAM_H_
#define  __PARAM_H_

#include <thread>
#include <iostream>
#include <string.h>

namespace CRFPP {

class Param {
private:
    unsigned int _freq;
    unsigned int _maxiter;
    float _cost1; // L1 norm cost
    float _cost2; // L2 norm cost
    float _eta;
    bool _convert;
    bool _textmodel;
    std::string _algorithm;
    unsigned int _thread;
    unsigned int _shrinking_size;
    std::string _system_name;
    // std::string _version;
    // std::string _help;
public:
    const static std::string CRF_L1;
    const static std::string CRF_L2;

private:
    void usage();
    void show_version () {
        std::cout << "CRFtoy of 1.0" << std::endl;
    }

public:
    Param() : _freq(1),_maxiter(10000),_cost1(0.0),_cost2(1.0),
    _eta(0.0001),_convert(false),_textmodel(false),
    _algorithm("CRF-L2"),_shrinking_size(20),
    _thread(std::thread::hardware_concurrency()){
    }

    const std::string& get_algorithm() {
        return _algorithm;
    }

    const unsigned int& get_maxiter() {
        return _maxiter;
    }
    const float get_cost1() {
        return _cost1;
    }

    const float get_cost2() {
        return _cost2;
    }

    bool get_textmodel() {
        return _textmodel;
    }
    
    bool get_thread() {
        return _thread;
    }

    void debug_info ();

    bool read(int argc, char* argv[]);

};

}

#endif  //__PARAM_H_

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
