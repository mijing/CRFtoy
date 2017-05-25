#ifndef IDMAP
#define IDMAP

#include <map>
#include <string>
#include <algorithm>

class IdMap
{
public:
	std::map<std::string, size_t> idmap;
private:
	size_t maxId; // the max feature id; 
	size_t step;
public:
	IdMap()
	{
		this->maxId = 0;
	}

	int getId(const std::string& feature, bool isBigram)
	{
		auto iter = this->idmap.find(feature);
		if (this->idmap.end() == iter)
		{
			this->idmap.insert(std::pair<std::string, size_t>(feature, this->maxId));
			int result = this->maxId;
			if (isBigram)
				this->maxId += (this->step+1) * this->step;
			else
				this->maxId += this->step;
			return result;
		}
		return iter->second;
	}

	void add(const std::string& feature, size_t id)
	{
		this->idmap.insert(std::make_pair(feature, id));
		this->maxId = std::max(this->maxId, id);
	}

	int getMaxId()
	{
		return this->maxId;
	}

	void setStep(int step)
	{
		this->step = step;
	}

	size_t get_step()
	{
		return this->step;
	}
};

#endif
