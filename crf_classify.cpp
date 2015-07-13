/*#include<iostream>
#include<string>
#include "crf_test.hpp"
int main(int argc, char* argv[])
{
	if (argv[1] == "-h" || argv[1] == "--help"){
		std::cout << "train command line:" << std::endl;
		std::cout << "	crf_classify test_file model_file output_file" << std::endl;
		exit(1);
	}
	if (argc <= 3){
		std::cout << "wrong command line!!!" << std::endl;
		std::cout << "train command line:" << std::endl;
		std::cout << "	crf_classify test_file model_file output_file" << std::endl;
		exit(1);
	}
	std::string test_file = argv[1];
	std::string model_file = argv[2];
	std::string result_file = argv[3];

	crf_test(test_file, model_file, result_file);

	return 0;
}*/