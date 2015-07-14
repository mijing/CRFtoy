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
			this->print_error_message(line);
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
			for (auto& item : bigram.items)
				if (item.column > this->columnSize)
					this->columnSize = item.column;
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
		if (!infp.is_open()){
			std::cout << "Failed to open file " << filename << std::endl;
			return;
		}
		//get unilines and bilines
		std::vector<std::string> unilines;
		std::vector<std::string> bilines;
		for (std::string tmp; getline(infp, tmp);)
		{
			if (tmp == "# Unigram")
				break;
		}
		for (std::string tmp; getline(infp, tmp);)
		{
			if (tmp == "# Bigram")
				break;
			if (tmp != "")
				unilines.push_back(tmp);
		}
		for (std::string tmp; getline(infp, tmp);)
		{
			std::cout << tmp << std::endl;
			if (tmp != "")
				bilines.push_back(tmp);
		}
		//get unigrams and bigrams
		for (auto& line:unilines)
		{
			this->unigrams.push_back(Unigram(line));
		}
		for (auto& line:bilines)
		{
			this->bigrams.push_back(Bigram(line));
		}
		infp.close();

		return;
	}
};

#endif