#include<iostream>
#include"example.hpp"
#include"template.hpp"
#include"corpus.hpp"
#include"crf_train.hpp"
#include"model.hpp"
#include "assert.h"
#include<math.h>
#include"matrix.hpp"
#include"cube.hpp"
#include<thread>
/*
 *get the inner product of weight and features f*w
 *not exp(f*w)
*/
double get_psi_uf(const Token& tj, const int label, Model& model, bool isTest)
{	//can be optimize
	//assert(label2==tj.label);
	double result = 0;
	for (auto& uf : tj.ugramFeatures)
	{
		int id = uf + label;
		if (id >= model.get_weight_size())
		{
			if (isTest)
				continue;
			else
			{
				std::cout << "id is bigger than the max feature id" << std::endl;
				exit(1);
			}
		}
		result += model.weight[id];
	}
	return result;
}
double get_psi_bf(const Token& tj, const int label1, const int label2, Model& model, bool isTest)
{
	double result = 0;
	for (auto& bf : tj.bgramFeatures)
	{
		int id = bf + model.labelMap.getId(label1, label2);
		if (id >= model.get_weight_size())
		{
			if (isTest)
				continue;
			else
			{
				std::cout << "id is bigger than the max feature id" << std::endl;
				exit(1);
			}
		}
		result += model.weight[id];
	}
	return result;
}

void get_delta(Model& model, Corpus& corpus, double* weight, const int tid,const size_t thread_num)
{
	memset(weight, 0, sizeof(double)*model.get_weight_size());
	assert(weight[0]<std::numeric_limits<double>::min());
	assert(weight[0]>-std::numeric_limits<double>::min());

	const int labelSize = model.labelMap.getMaxId() + 1;
	std::vector<std::vector<double>> psi_uf(corpus.sentence_max_length);
	for(auto& item: psi_uf)
		item.resize(labelSize);

	std::vector<std::vector<std::vector<double>>> psi(corpus.sentence_max_length);
	for (auto&item : psi)
	{
		item.resize(labelSize);
		for(auto& array:item)
			array.resize(labelSize);
	}

	std::vector<std::vector<double>> alpha(corpus.sentence_max_length);
	for(auto& item:alpha)
		item.resize(labelSize);

	std::vector<std::vector<double>> beta(corpus.sentence_max_length);
	for(auto& item:beta)
		item.resize(labelSize);

	for (int sid = tid; sid < corpus.sentences.size(); sid+=thread_num)
	{
		auto& tokens = corpus.sentences[sid].tokens;
		for (int j = 0; j < tokens.size(); ++j)
		{
			for (int label = 2; label <= model.labelMap.getMaxId(); ++label)
			{
				psi_uf[j][label] = get_psi_uf(tokens[j], label, model);
			}
		}
		//compute entropy of token j
		for (int j = 0; j < tokens.size(); j++)
		{
			double max = -std::numeric_limits<double>::max();
			for (int lr = 2; lr <= model.labelMap.getMaxId(); ++lr)
			{// lr is the label of tokens[j];
				if (0 == j)
				{
					const int ll = model.labelMap.get_start_id();
					psi[j][ll][lr] = psi_uf[j][lr] + get_psi_bf(tokens[j], ll, lr, model);
					max = std::max(max, psi[j][ll][lr]);
				}
				else
				{
					for (int ll = 2; ll <= model.labelMap.getMaxId(); ++ll)
					{
						psi[j][ll][lr] = psi_uf[j][lr] + get_psi_bf(tokens[j], ll, lr, model);
						max = std::max(max, psi[j][ll][lr]);
					}
				}
			}

			for (int lr = 2; lr <= model.labelMap.getMaxId(); ++lr)
			{
				if (0 == j)
				{
					const int ll = model.labelMap.get_start_id();
					psi[j][ll][lr] = exp(psi[j][ll][lr] - max);
				}
				else
				{
					for (int ll = 2; ll <= model.labelMap.getMaxId(); ++ll)
						psi[j][ll][lr] = exp(psi[j][ll][lr] - max);
				}
			}
		}
		//compute forward probility
		for (int j = 0; j < tokens.size(); ++j)
		{
			for (int lr = 2; lr <= model.labelMap.getMaxId(); ++lr)
			{
				if (0 == j)
				{
					const int ll = model.labelMap.get_start_id();
					alpha[j][lr] = psi[j][ll][lr];
				}
				else
				{
					double w=0;
					for (int ll = 2; ll <= model.labelMap.getMaxId(); ++ll)
					{
						w += alpha[j - 1][ll]*psi[j][ll][lr];
					}
					alpha[j][lr] = w;
				}
			}
		}
		//compute backword probility
		for (int j = tokens.size() - 1; j >= 0; --j)
		{
			for (int ll = 2; ll <= model.labelMap.getMaxId(); ++ll)
			{
				if (tokens.size() - 1 == j)
				{
					beta[j][ll]=1.0;
				}
				else
				{
					double w = 0;
					for (int lr = 2; lr <= model.labelMap.getMaxId(); ++lr)
					{
						w += psi[j + 1][ll][lr]*beta[j + 1][lr];
					}
					beta[j][ll] = w;
				}
			}
		}
		//compute grediant
		for (int i = 0; i < tokens.size(); ++i)
		{
			auto& token = tokens[i];
			for (auto& uf : token.ugramFeatures)
			{
				int id = uf + token.label;
				weight[id] += 1;
			}
		
			auto pre_label = model.labelMap.get_start_id();
			if (i>0)pre_label = tokens[i - 1].label;
			for (auto& bf : token.bgramFeatures)
			{
				int id = bf + model.labelMap.getId(pre_label, token.label);
				weight[id] += 1;
			}
		}

		double z = 0;
		for (int label = 2; label<=model.labelMap.getMaxId(); ++label)
		{
			z += alpha[tokens.size() - 1][label];
		}

		for (int i = 0; i < tokens.size(); ++i)
		{
			for (int lr = 2; lr <= model.labelMap.getMaxId(); ++lr)
			{
				if (0 == i)
				{
					double prob = 0;
					if (z > 0)
					{
						const int ll = model.labelMap.get_start_id();
/*						if (rand() % 100000 == 0)
							std::cout << psi[i][0][lr] << " " << beta[i][lr] << " " << z << std::endl;
*/						prob = psi[i][ll][lr] / z * beta[i][lr];
					}
//					assert(prob > -std::numeric_limits<double>::min());
					if (!(prob > -std::numeric_limits<double>::min()))
					{
						std::cout << prob << " prob is to small" << std::endl;
						exit(1);
					}
					auto& token = tokens[i];
					for (auto& uf : token.ugramFeatures)
					{
						int id = uf + lr;
						weight[id] -= prob;
					}
					for (auto& bf : token.bgramFeatures)
					{
						int id = bf + model.labelMap.getId(0, lr);
						weight[id] -= prob;
					}
				}
				else
				{
					for (int ll = 2; ll <= model.labelMap.getMaxId(); ++ll)
					{
						double prob = 0;
						if (z > 0)
						{
/*							if(rand()%100000==0)
								std::cout << alpha[i - 1][ll] << " " << psi[i][ll][lr] << " " << beta[i][lr] << " " << z << std::endl;
*/							prob = alpha[i-1][ll] / z * psi[i][ll][lr] * beta[i][lr];
						}
//						assert (prob > -std::numeric_limits<double>::min());
						if (!(prob > -std::numeric_limits<double>::min()))
						{
							std::cout << prob << std::endl;
							exit(1);
						}
						auto& token = tokens[i];
						for (auto& uf : token.ugramFeatures)
						{
							int id = uf + lr;// it is lr, not ll
							weight[id] -= prob;
						}
						for (auto& bf : token.bgramFeatures)
						{
							int id = bf + model.labelMap.getId(ll, lr);
							weight[id] -= prob;
						}
					}
				}
			}
		}
	}

}



void train(Corpus& corpus, Model& model)
{
	std::cout << "model.get_weight_size()=" << model.get_weight_size() << std::endl;
	std::cout << "corpus.idMap.getMaxId()=" << corpus.idMap.getMaxId() << std::endl;
	const size_t thread_num = std::thread::hardware_concurrency();
	std::vector<double*> gradients(thread_num);
	for (auto& gradient : gradients)
	{
		gradient = new double[model.get_weight_size()];
	}

	for (int i = 0; i < 50; ++i)
	{
		std::cout << "iter " << i << std::endl;
		std::vector<std::thread> threads;
		for (int tid = 0; tid < thread_num; ++tid)
		{
			threads.push_back(std::thread(get_delta, std::ref(model), std::ref(corpus), gradients[tid], tid, thread_num));
		}
		for (auto&td : threads)
			td.join();
		for (int tid = 1; tid < thread_num; ++tid)
		{
			for (int fid = 0; fid < model.get_weight_size(); ++fid)
				gradients[0][fid] += gradients[tid][fid];
		}
		const double lambda = 1;
		const double step = 0.01;
		model.update_weight(gradients[0], lambda, step);
	}

	for (auto& gradient : gradients)
	{
		delete[] gradient;
	}
}

void crf_train(std::string train_file, std::string template_file, std::string model_file)
{
	Example example(train_file);
	Template crf_template(template_file);
	Corpus corpus(crf_template, example);
	Model model(corpus.idMap, crf_template, corpus.labelMap);
	train(corpus, model);
	model.write(model_file);
	return;
}