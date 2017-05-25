#ifndef __LABELMAP__
#define __LABELMAP__

#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>

#include "lib/common.h"

class LabelMap
{
public:
	std::map<std::string, int> labelMap;
	// const static std::string special_start;// special start label
	// const static std::string special_end;// special end label
private:
	// int special_start_id;// special start label
	// int special_end_id;// special end label
	std::vector<std::string> labelById;
	int maxId;
public:
	LabelMap()
	{
		maxId = -1;
		// special_start_id = this->getId(special_start, true);
		// special_end_id = this->getId(special_end, true);
	}

	std::string get_label(int id)
	{
		if (this->labelById.size() == 0)
		{
			this->labelById.resize(this->labelMap.size());
			for (auto& item : this->labelMap)
			{
				this->labelById[item.second] = item.first;
			}
		}
		return this->labelById[id];
	}

	int getId(const std::string& label, bool autoIncrease = false)
	{
		if (this->labelMap.find(label) == this->labelMap.end())
		{
			if (autoIncrease)
			{
				this->maxId++;
				this->labelMap.insert(std::pair<std::string, int>(label, this->maxId));
				return this->labelMap[label];
			}
			else
            {	
                CHECK(false) << "label id is not found " << label << std::endl;
                exit(0);
            }


		}
		else
		{
			return this->labelMap[label];
		}
	}

	void add(const std::string& label, size_t id)
	{
		// if (label == "#" || label == "*")
		//	return; //"#" and "*" are in label map;
		if (this->labelMap.find(label) != this->labelMap.end())
		{
			CHECK(false) << "label is already in label map: " << label << std::endl;
			exit(1);
		}
		else
		{
			this->labelMap.insert(std::make_pair(label, id));
			this->maxId = std::max(this->maxId, int(id));
		}
	}

	int getId(int llid, int lrid)
	{
		return llid * (this->getMaxId() + 1) + lrid;
	}
	/* int get_start_id()
	{
		return special_start_id;
	}
	int get_end_id()
	{
		return special_end_id;
	} */
	int getMaxId()
	{
		return maxId;
	}
};

#endif
