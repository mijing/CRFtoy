#include <iostream>
#include <thread>

#include <math.h>

#include "example.hpp"
#include "template.hpp"
#include "corpus.hpp"
#include "crf_train.hpp"
#include "model.hpp"
#include "assert.h"
#include "matrix.hpp"
#include "cube.hpp"
#include "lbfgs/lbfgs.h"

/*
 * get the inner product of weight and features f*w
 * not exp(f*w)
*/
double get_psi_uf(const Token& tj,
        const int label,
        const lbfgsfloatval_t* weight,
        const size_t weight_size,
        bool isTest)
{
    // can be optimize
	// assert(label2==tj.label);
	double result = 0;
	for (auto& uf : tj.ugramFeatures)
	{
		int id = uf + label;
		if (id >= weight_size)
		{
			if (isTest)
				continue;
			else
			{
				std::cout << "id is bigger than the max feature id" << std::endl;
				exit(1);
			}
		}
		result += weight[id];
	}
	return result;
}

double get_psi_bf(const Token& tj,
        const int label1,
        const int label2,
        const lbfgsfloatval_t* weight,
        const size_t weight_size,
        LabelMap& labelmap,
        bool isTest)
{
	double result = 0;
	for (auto& bf : tj.bgramFeatures)
	{
		int id = bf + labelmap.getId(label1, label2);
		if (id >= weight_size)
		{
			if (isTest)
				continue;
			else
			{
				std::cout << "id is bigger than the max feature id" << std::endl;
				exit(1);
			}
		}
		result += weight[id];
	}
	return result;
}

void get_delta(const lbfgsfloatval_t* weight,
        const size_t weight_size,
        Corpus& corpus,
        lbfgsfloatval_t* gradient,
        const int tid,
        const size_t thread_num,
        lbfgsfloatval_t* loss)
{
	memset(gradient, 0, sizeof(lbfgsfloatval_t) * weight_size);
	assert(gradient[0] < std::numeric_limits<double>::min());
	assert(gradient[0] > -std::numeric_limits<double>::min());

	const int labelSize = corpus.labelMap.getMaxId() + 1;
    // id of the special start label '*'
    const int special_start_id = corpus.labelMap.getMaxId() + 1;
    const int max_label_id = corpus.labelMap.getMaxId();

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
	for(auto& item : alpha)
		item.resize(labelSize);

	std::vector<std::vector<double>> beta(corpus.sentence_max_length);
	for(auto& item:beta)
		item.resize(labelSize);

    *loss = 0;
	for (int sid = tid; sid < corpus.sentences.size(); sid += thread_num)
	{
		auto& tokens = corpus.sentences[sid].tokens;
		for (int j = 0; j < tokens.size(); ++j)
		{
			for (int label = 0; label <= max_label_id; ++label)
			{
				psi_uf[j][label] = get_psi_uf(tokens[j], label, weight, weight_size);
			}
		}

		// compute entropy of token j
		for (int j = 0; j < tokens.size(); j++)
		{
			double max = -std::numeric_limits<double>::max();
			for (int lr = 0; lr <= max_label_id; ++lr)
			{
                // lr is the label of tokens[j];
				if (0 == j)
				{
					const int ll = 0; // model.labelMap.get_start_id();
					psi[j][ll][lr] = psi_uf[j][lr] + 
                        get_psi_bf(tokens[j], special_start_id,
                                lr, weight, weight_size, corpus.labelMap);
					max = std::max(max, psi[j][ll][lr]);
				}
				else
				{
					for (int ll = 0; ll <= max_label_id; ++ll)
					{
						psi[j][ll][lr] = psi_uf[j][lr] + 
                            get_psi_bf(tokens[j], ll, lr, weight, weight_size, corpus.labelMap);
						max = std::max(max, psi[j][ll][lr]);
					}
				}
			}

			for (int lr = 0; lr <= max_label_id; ++lr)
			{
				if (0 == j)
				{
					const int ll = 0; // model.labelMap.get_start_id();
                    psi[j][ll][lr] -= max;
                    if (lr == tokens[j].label) {
                        *loss -= psi[j][ll][lr];
                    }
					psi[j][ll][lr] = exp(psi[j][ll][lr]);
				}
				else
				{
					for (int ll = 0; ll <= max_label_id; ++ll) {
                        psi[j][ll][lr] -= max;
                        if (ll == tokens[j-1].label && lr == tokens[j].label) {
                            *loss -= psi[j][ll][lr];
                        }
						psi[j][ll][lr] = exp(psi[j][ll][lr]);
                    }
				}
			}
		}

		//compute forward probility
		for (int j = 0; j < tokens.size(); ++j)
		{
			for (int lr = 0; lr <= max_label_id; ++lr)
			{
				if (0 == j)
				{
					const int ll = 0; // model.labelMap.get_start_id();
					alpha[j][lr] = psi[j][ll][lr];
				}
				else
				{
					double w = 0;
					for (int ll = 0; ll <= max_label_id; ++ll)
					{
						w += alpha[j - 1][ll] * psi[j][ll][lr];
					}
					alpha[j][lr] = w;

				}
//                std::cout << "alpha[j][lr] = " << alpha[j][lr] << std::endl;
			}
		}

		//compute backword probility
		for (int j = tokens.size() - 1; j >= 0; --j)
		{
			for (int ll = 0; ll <= max_label_id; ++ll)
			{
				if (tokens.size() - 1 == j)
				{
					beta[j][ll] = 1.0;
				}
				else
				{
					double w = 0;
					for (int lr = 0; lr <= max_label_id; ++lr)
					{
						w += psi[j + 1][ll][lr] * beta[j + 1][lr];
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
				gradient[id] += 1;
			}
		
			auto pre_label = special_start_id; // model.labelMap.get_start_id();
			if (i > 0) pre_label = tokens[i - 1].label;
			for (auto& bf : token.bgramFeatures)
			{
				int id = bf + corpus.labelMap.getId(pre_label, token.label);
				gradient[id] += 1;
			}
		}

		double z = 0;
		for (int label = 0; label <= max_label_id; ++label)
		{
			z += alpha[tokens.size() - 1][label];
		}
        *loss += log(z);
        // std::cout << z << std::endl;

		for (int i = 0; i < tokens.size(); ++i)
		{
			for (int lr = 0; lr <= max_label_id; ++lr)
			{
				if (0 == i)
				{
					double prob = 0;
					if (z > 0)
					{
						const int ll = 0; // model.labelMap.get_start_id();
/*						if (rand() % 100000 == 0)
							std::cout << psi[i][0][lr] << " " << beta[i][lr]
                            << " " << z << std::endl;
*/						prob = psi[i][ll][lr] / z * beta[i][lr];
					}
//					assert(prob > -std::numeric_limits<double>::min());
					if (!(prob > -std::numeric_limits<double>::min()))
					{
						std::cout << prob << " prob is too small" << std::endl;
						exit(1);
					}
					auto& token = tokens[i];
					for (auto& uf : token.ugramFeatures)
					{
						int id = uf + lr;
						gradient[id] -= prob;
					}
					for (auto& bf : token.bgramFeatures)
					{
						int id = bf + corpus.labelMap.getId(special_start_id, lr);
						gradient[id] -= prob;
					}
				}
				else
				{
					for (int ll = 0; ll <= max_label_id; ++ll)
					{
						double prob = 0;
						if (z > 0)
						{
/*							if(rand()%100000==0)
								std::cout << alpha[i - 1][ll] << " " << psi[i][ll][lr] << " "
                                << beta[i][lr] << " " << z << std::endl;
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
							int id = uf + lr; // it is lr, not ll
							gradient[id] -= prob;
						}
						for (auto& bf : token.bgramFeatures)
						{
							int id = bf + corpus.labelMap.getId(ll, lr);
							gradient[id] -= prob;
						}
					}
				}
			}
		}
	}
}

typedef struct Instance {
    Corpus* corpus;
    CRFPP::Param* param;
} Instance;

static lbfgsfloatval_t evaluate(
        void *instance,
        const lbfgsfloatval_t *weight,
        lbfgsfloatval_t *g,
        int weight_size,
        const lbfgsfloatval_t step) {

    Instance* ins = static_cast<Instance*>(instance);
    Corpus* corpus = ins->corpus;
    CRFPP::Param* param = ins->param;

    const size_t thread_num = std::thread::hardware_concurrency();
	std::vector<double*> gradients(thread_num);
    lbfgsfloatval_t fx_sum = 0;
	for (auto& gradient : gradients)
	{
		gradient = new double[weight_size];
	}

    std::vector<std::thread> threads;
    threads.reserve(thread_num);
    std::vector<lbfgsfloatval_t> losses(thread_num);
    for (int tid = 0; tid < thread_num; ++tid)
    {
        threads.push_back(std::thread(get_delta,
                    weight,
                    weight_size,
                    std::ref(*corpus),
                    gradients[tid],
                    tid,
                    thread_num,
                    &losses[tid]));
    }
    for (auto&td : threads)
        td.join();

    memset(g, 0, weight_size * sizeof(lbfgsfloatval_t));
    for (int fid = 0; fid < weight_size; ++fid)
    {

        for (int tid = 0; tid < thread_num; ++tid)
            g[fid] -= gradients[tid][fid];
    }

    for (auto& gradient : gradients) {
        delete[] gradient;
        gradient = nullptr;
    }

    lbfgsfloatval_t loss = 0;
    for (int tid = 0; tid < thread_num; ++tid) {
        loss += losses[tid];
    }
    if (param->get_cost2() > 0) {
        float c_value = param->get_cost2() / 2.0;
        for (int fid = 0; fid < weight_size; ++fid) {
            g[fid] += param->get_cost2() * weight[fid];
            loss += c_value * weight[fid] * weight[fid];
        }
    }
    return loss;
}

static int progress(
    void *instance,
    const lbfgsfloatval_t *x,
    const lbfgsfloatval_t *g,
    const lbfgsfloatval_t fx,
    const lbfgsfloatval_t xnorm,
    const lbfgsfloatval_t gnorm,
    const lbfgsfloatval_t step,
    int n,
    int k,
    int ls) {

    std::cout << "iter=" << k << ", obj=" << fx << ", eval_num=" << ls << std::endl;
    return 0;
}

void train_lbfgs(Corpus& corpus, Model& model, CRFPP::Param& param) {
	std::cout << "Dimension of weight: " << model.get_weight_size() << std::endl;
	std::cout << "Number of features: " << corpus.idMap.getMaxId() << std::endl;
    std::cout << "Number of sentences: " << corpus.sentences.size() << std::endl;
    std::cout << "Number of labels: " << model.labelMap.labelMap.size() << std::endl;

    lbfgs_parameter_t lbfgs_param;
    lbfgs_parameter_init(&lbfgs_param);
    if (param.get_cost1() > 0) {
        lbfgs_param.orthantwise_c = param.get_cost1();
        lbfgs_param.linesearch = LBFGS_LINESEARCH_BACKTRACKING;
    }
    else {
        lbfgs_param.orthantwise_c = 0;
    }

    lbfgsfloatval_t loss;
    Instance instance;
    instance.corpus = &corpus;
    instance.param = &param;
    lbfgs(model.weight_size, model.weight, &loss, evaluate, progress, &instance, &lbfgs_param);
}

void train_bgd(Corpus& corpus, Model& model, CRFPP::Param& param)
{
	std::cout << "Dimension of weight: " << model.get_weight_size() << std::endl;
	std::cout << "Number of features: " << corpus.idMap.getMaxId() << std::endl;
    std::cout << "Number of sentences: " << corpus.sentences.size() << std::endl;
    std::cout << "Number of labels: " << model.labelMap.labelMap.size() << std::endl;

	size_t thread_num = param.get_thread();
    if (thread_num <= 0) {
        thread_num = 1;
    }

	std::vector<double*> gradients(thread_num);
	for (auto& gradient : gradients)
	{
		gradient = new double[model.get_weight_size()];
	}

    double init_step = 0.001;
	for (int i = 0; i < param.get_maxiter(); ++i)
	{
		if (param.get_maxiter() > 20 && i%(param.get_maxiter()/10) == 0) {
            std::cout << "iter " << i << std::endl;
        }
        std::vector<lbfgsfloatval_t> losses(thread_num);
		std::vector<std::thread> threads;
        threads.reserve(thread_num);
		for (int tid = 0; tid < thread_num; ++tid)
		{
			threads.push_back(std::thread(get_delta,
                        model.weight,
                        model.get_weight_size(),
                        std::ref(corpus),
                        gradients[tid],
                        tid,
                        thread_num,
                        &losses[tid]));
		}
		for (auto&td : threads)
			td.join();

        double gradient_norm = 0;
		for (int fid = 0; fid < model.get_weight_size(); ++fid)
		{
		    for (int tid = 1; tid < thread_num; ++tid)
				gradients[0][fid] += gradients[tid][fid];

            gradient_norm += gradients[0][fid] * gradients[0][fid];
		}
        if (gradient_norm < 0.001) {
            // break;
        }

		const double lambda = 1;
		const double step = init_step;// * 2 / (1 + sqrt(i));
		model.update_weight(gradients[0], param.get_cost2(), step, param);
	}

	for (auto& gradient : gradients)
	{
		delete[] gradient;
	}
}

void crf_train(const std::string& train_file,
        const std::string& template_file,
        const std::string& model_file,
        CRFPP::Param& param)
{
	std::cout << "start reading files" << std::endl;
	Example example(train_file);

	// std::cout << "reading template file" << std::endl;
	Template crf_template(template_file);

	// std::cout << "example to corpus" << std::endl;
	Corpus corpus(crf_template, example);
	Model model(corpus.idMap, crf_template, corpus.labelMap);

	std::cout << "start training model" << std::endl;
	// train_bgd(corpus, model, param);
	train_lbfgs(corpus, model, param);

	std::cout << "writing model to file: " << model_file << std::endl;
	model.write(model_file, param.get_textmodel());
    
	return;
}
