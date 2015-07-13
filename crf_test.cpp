#include<string>
#include<fstream>

#include"crf_test.hpp"
#include"template.hpp"
#include"model.hpp"
#include"corpus.hpp"
#include"example.hpp"
#include"matrix.hpp"
#include"cube.hpp"
#include"crf_train.hpp"

void crf_test(const std::string& test_file, const std::string& model_file, const std::string& output_file)
{
	Model model(model_file);
	Example example(test_file);
	Corpus corpus(model, example);

	const int labelSize = model.labelMap.getMaxId() + 1;
	std::vector<std::vector<double>> psi_uf(corpus.sentence_max_length);
	for(auto& item:psi_uf)
		item.resize(labelSize);
	std::vector<std::vector<std::vector<double>>> psi(corpus.sentence_max_length);
	for (auto& item : psi)
	{
		item.resize(labelSize);
		for (auto& sub : item)
			sub.resize(labelSize);
	}

	std::vector<std::vector<double>> previous(corpus.sentence_max_length);
	for (auto& item : previous)
		item.resize(labelSize);
	std::vector<std::vector<int>> from(corpus.sentence_max_length);
	for (auto& item : from)
		item.resize(labelSize);

	std::vector<int> labelpath(corpus.sentence_max_length);
	std::ofstream out(output_file);
	if (!out.is_open()){
		std::cout << "failed to open file " << output_file << std::endl;
		exit(1);
	}
	for (auto& sentence : corpus.sentences)
	{
		auto& tokens = sentence.tokens;
		for (int j = 0; j < tokens.size(); ++j)
		{
			for (int label = 2; label <= model.labelMap.getMaxId(); ++label)
			{
				psi_uf[j][label] = get_psi_uf(tokens[j], label, model, true);
			}
		}

		for (int j = 0; j < tokens.size(); j++)
		{
			double max = -std::numeric_limits<double>::max();
			for (int lr = 2; lr <= model.labelMap.getMaxId(); ++lr)
			{
				if (0 == j)
				{
					const int ll = model.labelMap.get_start_id();
					psi[j][ll][lr] = psi_uf[j][lr] + get_psi_bf(tokens[j], ll, lr, model, true);
					max = std::max(max, psi[j][ll][lr]);
				}
				else
				{
					for (int ll = 2; ll <= model.labelMap.getMaxId(); ++ll)
					{
						psi[j][ll][lr] = psi_uf[j][lr] + get_psi_bf(tokens[j], ll, lr, model, true);
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
				else{
					for (int ll = 2; ll <= model.labelMap.getMaxId(); ++ll)
						psi[j][ll][lr] = exp(psi[j][ll][lr] - max);
				}
			}
		}
		//Viterbi algorithm
		for (int i = 0; i < tokens.size(); ++i)
		{
			for (int lr = 2; lr < labelSize; ++lr)
			{
				if (0==i)
				{
					const int ll = model.labelMap.get_start_id();
					previous[i][lr] = psi[i][ll][lr];
					from[i][lr] = ll;
				}
				else{
					double max = -std::numeric_limits<double>::max();
					int label;
					for (int ll = 2; ll < labelSize; ++ll)
					{
						double tmp = previous[i-1][ll] * psi[i][ll][lr];
						if (max < tmp)
						{
							max = tmp;
							label = ll;
						}
					}
					previous[i][lr] = max;
					from[i][lr] = label;
				}
			}
//			std::swap(present, previous);
		}

		int label;
		double max = -std::numeric_limits<double>::max();
		for (int i = 2; i < labelSize; ++i)
		{
			if (max < previous[tokens.size() - 1][i])
			{
				label = i;
				max = previous[tokens.size() - 1][i];
			}
		}

		labelpath[tokens.size() - 1] = label;
		for (int i = tokens.size() - 2; i >= 0; --i)
		{
			labelpath[i] = from[i + 1][labelpath[i + 1]];
		}

		for (int i = 0; i < tokens.size(); ++i)
		{
			out << model.labelMap.get_label(labelpath[i]) << std::endl;
		}
		out << std::endl;
	}
	out.close();
}