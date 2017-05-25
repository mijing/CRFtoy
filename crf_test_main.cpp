#include <iostream>
#include <string>
#include <string.h>
#include "crf_test.hpp"

static void usage(char* system_name) {
    std::cout << "CRFtoy: Yet Another CRF Tool Kit\n\
Copyright (C) 2015-2025 Wang Yiwen, All rights reserved.\n\ 
\n\
Usage: " << system_name << " [options] testFile modelFile outputFile\n\
        " << std::endl;
}

int main(int argc, char* argv[])
{
    if (argc!=4 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
        usage(argv[0]);
        exit(1);
    }

	std::string test_file = argv[1];
	std::string model_file = argv[2];
	std::string result_file = argv[3];

	crf_test(test_file, model_file, result_file);

	return 0;
}
