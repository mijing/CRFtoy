/***************************************************************************
 * 
 * Copyright (c) 2017 Wang Yiwen. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file param.cpp
 * @author yiwenwang(@aliyun.com)
 * @date 2017/03/23 21:41:59
 * @brief 
 *  
 **/

#include "param.hpp"

namespace CRFPP {

const std::string Param::CRF_L1 = "CRF-L1";
const std::string Param::CRF_L2 = "CRF-L2";

void Param::usage() {
    std::cout << "CRFtoy: Yet Another CRF Tool Kit\n\
Copyright (C) 2015-2025 Wang Yiwen, All rights reserved.\n\
\n\
Usage: " << _system_name << " [options] trainFile templateFile modelFile\n\
 -f, --freq=INT              use features that occuer no less than INT (default 1)\n\
 -m, --maxiter=INT           set INT for max iterations in LBFGS routine (default 10k)\n\
 -d, --cost1=FLOAT           set FLOAT for cost parameter of L1 regularization (default .0)\n\
 -c, --cost2=FLOAT           set FLOAT for cost parameter of L2 regularization (default 1.0)\n\
 -e, --eta=FLOAT             set FLOAT for termination criterion (default 0.0001)\n\
 -C, --convert               convert text model to binary model\n\
 -t, --textmodel             build also text model file for debugging\n\
 -p, --thread=INT            number of threads (default auto-detect)\n\
 -H, --shrinking-size=INT    set INT for number of iterations variable needs to  be optimal before considered for shrinking. (default 20)\n\
 -v, --version               show the version and exit\n\
 -h, --help                  show this help and exit" << std::endl;

    //-a, --algorithm=(CRF|MIRA)  select training algorithm (CRF-L1 or CRF-L2)\n
}

void Param::debug_info () {
    std::cout << "_freq = " << _freq << std::endl;
    std::cout << "_maxiter = " << _maxiter << std::endl;
    std::cout << "_cost1 = " << _cost1 << std::endl;
    std::cout << "_cost2 = " << _cost2 << std::endl;
    std::cout << "_eta = " << _eta << std::endl;
    std::cout << "_convert = " << _convert << std::endl;
    std::cout << "_textmodel = " << _textmodel << std::endl;
    // std::cout << "_algorithm = " << _algorithm << std::endl;
    std::cout << "_thread = " << _thread << std::endl;
    std::cout << "_shrinking_size = " << _shrinking_size << std::endl;
}

bool Param::read(int argc, char* argv[]) {
    _system_name = argv[0];
    if (argc < 4) {
        usage(); exit(0); return false;
    }
    for (int i = 1; i < argc - 3; ++i) {
        if ('-' != argv[i][0]) {
            usage();  exit(0); return false;
        }
        if ('-' != argv[i][1]) { // short options
            if (argv[i][1] != '\0' && argv[i][2] != '\0') {
                usage();  exit(0); return false;
            }
            switch (argv[i][1]) {
                case 'f': _freq = std::stoi(argv[++i]); break;
                case 'm': _maxiter = std::stoi(argv[++i]); break;
                case 'c': _cost2 = std::stof(argv[++i]); break;
                case 'd': _cost1 = std::stof(argv[++i]); break;
                case 'e': _eta = std::stof(argv[++i]); break;
                case 'C': _convert = true; break;
                case 't': _textmodel = true; break;
                // case 'a': _algorithm = argv[++i]; break;
                case 'p': _thread = std::stoi(argv[++i]); break;
                case 'H': _shrinking_size = std::stoi(argv[++i]); break;
                case 'v': show_version(); break;
                case 'h': usage(); exit(0); return true;
                default: std::cout << "unknow option [" << argv[i] << std::endl; usage(); exit(0); return false;
            }
        } // long options
        else if (0 == strcmp(argv[i] + 2, "freq")) {
            _freq = std::stoi(argv[++i]);
        } else if (0 == strcmp(argv[i] + 2, "maxiter")) {
            _maxiter = std::stoi(argv[++i]);
        } else if (0 == strcmp(argv[i] + 2, "cost1")) {
            _cost1 = std::stof(argv[++i]);
        } else if (0 == strcmp(argv[i] + 2, "cost2")) {
            _cost2 = std::stof(argv[++i]);
        } else if (0 == strcmp(argv[i] + 2, "eta")) {
            _eta = std::stof(argv[++i]);
        } else if (0 == strcmp(argv[i] + 2, "convert")) {
            _convert = true;
        } else if (0 == strcmp(argv[i] + 2, "textmodel")) {
            _textmodel = true;
        // } else if (0 == strcmp(argv[i] + 2, "algorithm")) {
        //    _algorithm = argv[++i];
        } else if (0 == strcmp(argv[i] + 2, "thread")) {
            _thread = std::stoi(argv[++i]);
        } else if (0 == strcmp(argv[i] + 2, "shrinking_size")) {
            _shrinking_size = std::stoi(argv[++i]);
        } else if (0 == strcmp(argv[i] + 2, "version")) {
            show_version();
        } else if (0 == strcmp(argv[i] + 2, "help")) {
            usage(); exit(0);
        }
        else {
            usage(); exit(0); return false;
        }
        if (i >= argc - 3) {
            usage(); exit(0); return false;
        }
    }
    return true;
}

} // namespace CRFPP

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
