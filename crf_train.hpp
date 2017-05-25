#ifndef CRF_TRAIN
#define CRF_TRAIN

#include "example.hpp"
#include "model.hpp"
#include "corpus.hpp"
#include "param.hpp"

#include "lbfgs/lbfgs.h"

void crf_train(const std::string& train_file,
        const std::string& template_file, 
        const std::string& model_file,
        CRFPP::Param& param);

double get_psi_bf(const Token& tj,
        const int label1,
        const int label2,
        const lbfgsfloatval_t* weight,
        const size_t weight_size,
        LabelMap& labelmap,
        bool isTest = false);

double get_psi_uf(const Token& tj,
        const int label, 
        const lbfgsfloatval_t* weight,
        const size_t weight_size,
        bool isTest = false);

#endif
