/*#include<iostream>
#include<string>
#include "crf_train.hpp"
int main(int argc, char* argv[])
{
	if (argv[1] == "-h" || argv[1] == "--help"){
		std::cout << "train command line:" << std::endl;
		std::cout << "	crf_learn train_file template_file model_file" << std::endl;
		exit(1);
	}

	if (argc <= 3){
		std::cout << "wrong command line!!!" << std::endl;
		std::cout << "train command line:" << std::endl;
		std::cout << "	crf_learn train_file template_file model_file" << std::endl;
		exit(1);
	}
	std::string train_file = argv[1];
	std::string template_file = argv[2];
	std::string model_file = argv[3];

	crf_train(train_file, template_file, model_file);

	return 0;
}*/