#ifndef _MODEL_HPP
#define _MODEL_HPP

#include <fstream>
#include <cstring>
#include <set>

#include <assert.h>

#include "idmap.hpp"
#include "template.hpp"
#include "labelmap.hpp"
#include "param.hpp"

#include "lbfgs/lbfgs.h"

class Model
{
public:
	IdMap idMap;
	Template CRFtemplate;
	LabelMap labelMap;
	lbfgsfloatval_t* weight;
	size_t weight_size;
public:
	Model(){}
	Model(const std::string& model_file)
	{
		std::ifstream in(model_file);
		if (!in.is_open()){
			std::cout << "Failed to open file " << model_file << std::endl;
			exit(1);
		}
		int size;
		// read label map
		in >> size;
		while (size-- > 0)
		{
			std::string label;
			int id;
			in >> label >> id;
			this->labelMap.add(label, id);
		}
		// read template
		in >> size;
		std::vector<std::string> lines;
		while (size-- > 0)
		{
			std::string line;
			do{
				getline(in, line);
			} while (line == "");
			lines.push_back(line);
		}
		this->CRFtemplate = Template(lines);

		// read id map
		in >> size;
		while (size-- > 0)
		{
			std::string feature;
			int id;
			in >> feature >> id;
			this->idMap.add(feature, id);
		}
		this->idMap.setStep(this->labelMap.labelMap.size());

		// read weights
		in >> size;
		this->weight_size = size;
		// this->weight = new double[size];
        this->weight = lbfgs_malloc(this->weight_size);
		
		for (double* p = this->weight; size-- > 0; p++)
		{
			in >> *p;
		}

		in.close();
	}

	Model(IdMap& idMap, Template& CRFtemplate, LabelMap& labelMap)
	{
		this->idMap = idMap;
		this->CRFtemplate = CRFtemplate;
		this->labelMap = labelMap;
		this->weight_size = this->idMap.getMaxId();
        int size = (this->idMap.idmap.size() - 1)*this->labelMap.labelMap.size() +
            + (this->labelMap.labelMap.size() + 1)*(this->labelMap.labelMap.size());
        // std::cout << weight_size << "\t" << size << std::endl;
        assert(size == weight_size);
        this->weight = lbfgs_malloc(this->weight_size);
		// this->weight = new double[this->weight_size];
		memset(this->weight, 0, this->weight_size*sizeof(lbfgsfloatval_t));
		// std::cout << this->weight[0] << std::endl;
		assert(this->weight[0]<std::numeric_limits<double>::min());
		assert(this->weight[0]>-std::numeric_limits<double>::min());
	}
	/*
	Model(Model&& tmp)
	{
		this->idMap = std::move(tmp.idMap);
		this->CRFtemplate = std::move(tmp.CRFtemplate);
		this->labelMap = std::move(tmp.labelMap);
		this->weight = tmp.weight;
		this->weight_size = tmp.weight_size;
		tmp.weight = nullptr;
	}*/

	Model(const Model& tmp)
	{
		this->idMap = tmp.idMap;
		this->CRFtemplate = tmp.CRFtemplate;
		this->labelMap = tmp.labelMap;
		this->weight = tmp.weight;
		this->weight_size = tmp.weight_size;
	}

	size_t get_weight_size()const
	{
		return this->weight_size;
	}
	
	void update_weight(double* delta, double lambda, double step, CRFPP::Param& param)
	{
		for (int i = 0; i < this->weight_size; i++)
		{
            if (param.get_cost2() > 0) {
                this->weight[i] += step*(delta[i] - param.get_cost2()*this->weight[i]);
            }
            if (param.get_cost1() > 0) {
                double w = 0;
                if (this->weight[i] > std::numeric_limits<double>::min()) {
                    w = 1;
                }
                else if (this->weight[i] < -(std::numeric_limits<double>::min())) {
                    w = -1;
                }
                double delta_tmp = step*(delta[i] - param.get_cost1() * w);
                if (this->weight[i] < 0) {
                    this->weight[i] = std::min(0.0, this->weight[i] + delta_tmp);
                }
                else if (this->weight[i] > 0){
                    this->weight[i] = std::max(0.0, this->weight[i] + delta_tmp);
                }
                else {
                    this->weight[i] += delta_tmp;
                }
            }
            else {
                std::cout << "error algorithm" << std::endl;
                exit(0);
            }
		}
	}

	~Model()
	{
		delete[] this->weight;
        this->weight = nullptr;
	}

	void write(const std::string& filename, bool istxt)
	{
		// std::cout << "Writing model to file " << filename << std::endl;
		std::ofstream out(filename);
		if (!out.is_open()) {
			std::cout << "Failed to open file " << filename << std::endl;
			exit(1);
		}

		//write label map
		out << labelMap.labelMap.size() << std::endl;
		for (auto&item : this->labelMap.labelMap)
		{
			out << item.first << "\t" << item.second << std::endl;
		}

		//write template
		out << this->CRFtemplate.bigrams.size() + this->CRFtemplate.unigrams.size() + 2 << std::endl;
		out << "# Unigram" << std::endl;
		for (auto& line : this->CRFtemplate._unilines)
		{
			out << line << std::endl;
		}
		out << "# Bigram" << std::endl;
		for (auto& line : this->CRFtemplate._bilines)
		{
			out << line << std::endl;
		}
		
		//write id map
		out << idMap.idmap.size() << std::endl;
		std::set<size_t> ids;
		for (auto& item : this->idMap.idmap)
		{
			out << item.first << "\t" << item.second << std::endl;
			for (int label = 0; label <= this->labelMap.getMaxId(); label++)
			{
				if (ids.find(label) != ids.end())
				{
					std::cout << label << " is already in ids, id overload!" << std::endl;
					exit(0);
				}
			}
		}

		//write weights
		out << weight_size << std::endl;
		for (int i = 0; i < this->weight_size; ++i)
		{
			out << weight[i] << std::endl;
		}

		out.close();
	}
};

#endif
