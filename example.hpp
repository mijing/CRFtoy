#ifndef EXAMPLE
#define EXAMPLE

#include<iostream>
#include<string>
#include<vector>
#include<sstream>
#include<fstream>

#include"lib/print.hpp"
#include"lib/string_lib.hpp"


class Word
{
public:
	std::vector<std::string> _row_features;
    static int ROW_FEATURES_SIZE;
public:
	Word(std::string& line)
	{
        _row_features.reserve(ROW_FEATURES_SIZE);
		std::stringstream ss(line);
        int num = 0;
		for (std::string tmp; ss >> tmp; _row_features.push_back(tmp))
            ++num;
        if (ROW_FEATURES_SIZE == 0) {
            ROW_FEATURES_SIZE = num;
        }
        else if (ROW_FEATURES_SIZE != num) {

        }
	}

	std::vector<std::string>& get_row_features()
	{
		return _row_features;
	}
};

class RowSentence
{
public:
	std::vector<Word> words;
public:
	RowSentence(std::vector<std::string>& lines)
	{
        words.reserve(lines.size()); // faster
		for (auto& line:lines)
		{
			this->words.push_back(Word(line));
		}
	}

	std::vector<Word>& get_words()
	{
		return this->words;
	}
};

class Example
{
public:
	std::vector<RowSentence*> sentences;
	int columnSize; //number of clumns of train data

public:
	Example(const std::string& file_name)
	{
		this->ReadExampleFile(file_name);
		this->set_columnSize();
	}
	~Example()
	{
		for (auto tmp : this->sentences)
			delete tmp;
	}

	void ReadExampleFile(const std::string& file_name)
	{
		std::ifstream fp(file_name);
		if (!fp.is_open()) {
			std::cout << "Failed to open file " << file_name << std::endl;
			exit(1);
		}
		std::vector<std::string> lines;
		for (std::string line; getline(fp, line);)
		{
			if (trim(line) != "")
				lines.push_back(line);
			else
			{
				this->sentences.push_back(new RowSentence(lines));
				lines.clear();
			}
		}
		if (lines.size() > 0)
		{
			this->sentences.push_back(new RowSentence(lines));
			lines.clear();
		}
		fp.close();
	}

	int get_columnSize() const
	{
		return this->columnSize;
	}
	/*
	set this->clumnNum and check format
	*/
	void set_columnSize()
	{
		this->columnSize = this->sentences[0]->get_words()[0].get_row_features().size();
		for (size_t i = 0; i < this->sentences.size(); i++)
		{
			std::vector<Word>& words = this->sentences[i]->get_words();
			for (size_t j = 0; j < words.size(); j++)
			{
				if (this->columnSize != words[j].get_row_features().size())
				{
					PrintError("file with wrong format, number of clumns is different. columnSize is "
                            + std::to_string(this->columnSize) +
                            ", and current column size is " +
                            std::to_string(words[j].get_row_features().size()));
					exit(1);
				}
			}
		}
	}

};

#endif
