#include <iostream>
#include <string>
#include "crf_train.hpp"
#include "param.hpp"

int main(int argc, char** argv)
{
    // read parameters
    CRFPP::Param param;
    param.read(argc, argv);
    // param.debug_info();

    if (argc < 3) exit(0);
    std::string train_file = argv[argc - 3];
    std::string template_file = argv[argc - 2];
    std::string model_file = argv[argc - 1];
	crf_train(train_file, template_file, model_file, param);

	return 0;
}
