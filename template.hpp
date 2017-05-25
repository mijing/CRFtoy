#ifndef TEMPLATE
#define TEMPLATE

#include<iostream>
#include<fstream>
#include<vector>
#include<sstream>
#include<string>

class Item //class of  
{
public:
	int row;
	int column;
	Item() :row(0), column(-1){}
};

class Unigram
{
public:
	std::string prefix; //such as "U00"
	std::vector<Item> items;

public:
	Unigram(){ prefix = ""; }
	Unigram(std::string& line)
	{
		this->line2gram(line);
	}

private:
	void print_error_message(const std::string& line)
	{
		std::cout << "Illegal unigram: " + line << std::endl;
	}

protected:
	void line2gram(const std::string& line)
	{
		size_t index = line.find(":");
		if (index == std::string::npos)
		{
			this->prefix = line;
			return;
		}
		this->prefix = line.substr(0, index);
		std::stringstream ss(line.substr(index));
		for (char tmpc; ss >> tmpc;)
		{
			for (int count = 3; count > 0 && ss >> tmpc; count--);
			if (tmpc != '[')
			{
				this->print_error_message(line);
				return;
			}
			Item p;
			ss >> p.row >> tmpc >> p.column >> tmpc;
			this->items.push_back(p);
		}
	}
};

class Bigram :public Unigram
{
public:
	Bigram()
	{
		prefix = "";
	}

	Bigram(const std::string& line)
	{
		if (line.find(':') == std::string::npos) // only "B"
		{
			this->prefix = line;
		}
		else this->line2gram(line); // line starts with "B00:"
	}
};

class Template
{
private:
	size_t columnSize;
public:
	std::vector<Unigram> unigrams;
	std::vector<Bigram> bigrams;
    std::vector<std::string> _unilines;
    std::vector<std::string> _bilines;
private:
	void SetColumnSize()
	{
		this->columnSize = 0;
		for (auto& unigram : this->unigrams)
		{
			for (auto& item : unigram.items)
				if (item.column > this->columnSize)
					this->columnSize = item.column;
		}

		for (auto& bigram : this->bigrams)
		{
			for (auto& item : bigram.items) {
				if (item.column > this->columnSize)
					this->columnSize = item.column;
            }
		}
	}


public:
	Template(){}
	Template(const std::string& filename)
	{
		this->ReadTemplateFile(filename);
		this->bigrams.shrink_to_fit();
		this->unigrams.shrink_to_fit();
		this->SetColumnSize();
	}

	Template(std::vector<std::string>& lines)
	{
		bool isBigram = false;
		for (std::string& line:lines)
		{
			if (line == "# Unigram")
				isBigram = false;
			else if (line == "# Bigram")
				isBigram = true;
			else if (line == "")
				continue;
			else if (isBigram)
				this->bigrams.push_back(Bigram(line));
			else
				this->unigrams.push_back(Unigram(line));
		}
		this->bigrams.shrink_to_fit();
		this->unigrams.shrink_to_fit();
		this->SetColumnSize();
	}

	size_t GetColumnSize()
	{
		return this->columnSize;
	}

private:
	void ReadTemplateFile(const std::string& filename)
	{
		std::ifstream infp(filename);
		if (!infp.is_open()) {
			std::cout << "Failed to open file " << filename << std::endl;
			return;
		}

        int num = 0;
		for (std::string tmp; getline(infp, tmp); ++num)
		{
            if (tmp.size() == 0) {
                continue;
            }
            switch (tmp[0]) {
                case '#':
                    break;
                case 'U':
                    _unilines.push_back(tmp);
                    break;
                case 'B':
                    _bilines.push_back(tmp);
                    break;
                default:
                    std::cout << "line " << num <<
                        " in template file is wrong: " << tmp << std::endl;
                    break;
            }
		}

		//get unigrams and bigrams
        this->unigrams.reserve(_unilines.size());
		for (auto& line : _unilines)
		{
			this->unigrams.push_back(Unigram(line));
		}

        this->bigrams.reserve(_bilines.size());
		for (auto& line : _bilines)
		{
			this->bigrams.push_back(Bigram(line));
		}
		infp.close();

		return;
	}
};

#endif
