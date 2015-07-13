#ifndef MODEL
#define MODEL

#include "idMap.hpp"
#include "template.hpp"
#include "labelMap.hpp"
#include<assert.h>
#include<fstream>
#include<cstring>
#include<set>

class Model
{
public:
	IdMap idMap;
	Template CRFtemplate;
	LabelMap labelMap;
	double* weight;
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
		//read label map
		in >> size;
		while (size-- > 0)
		{
			std::string label;
			int id;
			in >> label >> id;
			this->labelMap.add(label, id);
		}
		//read id map
		in >> size;
		while (size-- > 0)
		{
			std::string feature;
			int id;
			in >> feature >> id;
			this->idMap.add(feature, id);
		}
		this->idMap.setStep(this->labelMap.labelMap.size());
		//read weights
		in >> size;
		this->weight_size = size;
		this->weight = new double[size];
		
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
		std::cout << weight_size << "\t" << (this->idMap.idmap.size() - 1)*this->labelMap.labelMap.size() + (this->labelMap.labelMap.size())*(this->labelMap.labelMap.size()) << std::endl;
		this->weight = new double[this->weight_size];
		memset(this->weight, 0, this->weight_size*sizeof(double));
		std::cout << this->weight[0] << std::endl;
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
	
	void update_weight(double* delta, double lambda, double step)
	{
		for (int i = 0; i < this->weight_size; i++)
		{
			this->weight[i] += step*(delta[i] - lambda*this->weight[i]);
		}
	}

	~Model()
	{
		delete[] this->weight;
	}

	void write(const std::string& filename)
	{
		std::cout << "Writing model to file " << filename << std::endl;
		std::ofstream out(filename);
		if (!out.is_open()){
			std::cout << "Failed to open file " << filename << std::endl;
			exit(1);
		}
		//write label map;
		out << labelMap.labelMap.size() << std::endl;
		for (auto&item : this->labelMap.labelMap)
		{
			out << item.first << "\t" << item.second << std::endl;
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

		/*
		for (auto& item : this->idMap.idmap)
		{
			if (item.first[0] == 'U')
			{
				for (auto& label : this->labelMap.labelMap)
				{
					size_t id = item.second + label.second;
					out << item.first << "%x" << label.first << ": " << weight[id] << endl;
					if (ids.find(id) != ids.end())
					{
						cout << item.first << "%x" << label.first << ": " << endl;
						cout << id << endl;
					}
					ids.insert(id);
				}
			}
			else
			{
				for (auto& ll : this->labelMap.labelMap)
				{
					for (auto& lr : this->labelMap.labelMap)
					{
						size_t id = item.second + labelMap.getId(ll.second, lr.second);
						out << item.first << "%x" << ll.first << "%x"
							<< lr.first << ": " << weight[id] << endl;
						if (ids.find(id) != ids.end()){
							cout << item.first << "%x" << ll.first << "%x" << lr.first << ": " << endl;
							cout << id << endl;
						}
						ids.insert(id);
					}
				}
			}
		}*/
		out.close();
	}
};

#endif
