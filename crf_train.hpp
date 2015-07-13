#ifndef CRF_TRAIN
#define CRF_TRAIN
#include"example.hpp"
#include"model.hpp"
#include"corpus.hpp"
void crf_train(std::string train_file, std::string template_file, std::string model_file);
double get_psi_bf(const Token& tj, const int label1, const int label2, Model& model, bool isTest = false);
double get_psi_uf(const Token& tj, const int label, Model& model, bool isTest = false);

#endif