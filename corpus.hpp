#ifndef CORPUS
#define CORPUS

#include<map>
#include<algorithm>
#include<iostream>

#include "lib/print.hpp"
#include "labelMap.hpp"
#include "idMap.hpp"
#include "model.hpp"
#include "template.hpp"
#include "example.hpp"
class Token
{
public:
	std::vector<int> ugramFeatures;
	std::vector<int> bgramFeatures;
	int label;
};

class Sentence
{
public:
	std::vector<Token> tokens;
};

class Corpus
{
public:
	std::vector<Sentence> sentences;
	LabelMap labelMap;
	IdMap idMap;
	size_t column_szie;
	size_t sentence_max_length;
	Corpus(const Corpus&) = delete;
	Corpus operator=(const Corpus&) = delete;
	void check(Template& CRFtemplate)
	{
		int maxy = CRFtemplate.GetColumnSize();
		for (size_t i = 0; i < CRFtemplate.bigrams.size(); i++)
		{
			auto& bigram = CRFtemplate.bigrams[i];
			for (size_t j = 0; j < bigram.items.size(); j++)
			{
				auto& item = bigram.items[j];
				if (item.column > maxy)
				{
					PrintError("Template is wrong, its column is out of row feature size. Column size is "
						+ std::to_string(maxy) + ", current column is "	+ std::to_string(item.column));
					exit(1);
				}
			}
		}

		for (size_t i = 0; i < CRFtemplate.unigrams.size(); i++)
		{
			auto& unigram = CRFtemplate.unigrams[i];
			for (size_t j = 0; j < unigram.items.size(); j++)
			{
				auto& item = unigram.items[j];
				if (item.column > maxy)
				{
					PrintError("Template is wrong, its column is out of row feature size. Column size is " 
						+ std::to_string(maxy) + ", current column is " + std::to_string(item.column));
					exit(1);
				}
			}
		}
	}

	int getFeatureId(const RowSentence& rSentence, Sentence& sentence, const Unigram& ugram, size_t i, bool isBigram=false)
	{
		std::string feature = ugram.prefix + ":";
		for (size_t k = 0; k < ugram.items.size(); k++)
		{
			const Item& item = ugram.items[k];
			if (item.row + i >= 0 && item.row + i < rSentence.words.size() &&
				item.column >= 0 && item.column < rSentence.words[item.row + i].row_features.size())
			{
				feature += "%x" + rSentence.words[item.row + i].row_features[item.column];
			}
			else
			{
				feature += "%xNULL";
			}
		}
		return this->idMap.getId(feature, isBigram);
	}

	void RowSentence2Sentence(const RowSentence& rSentence, Sentence& sentence, const Template& CRFtemplate)
	{
		sentence.tokens.resize(rSentence.words.size());
		for (size_t i = 0; i < rSentence.words.size(); i++)
		{
			sentence.tokens[i].ugramFeatures.resize(CRFtemplate.unigrams.size());
			for (size_t j = 0; j < CRFtemplate.unigrams.size(); j++)
			{
				sentence.tokens[i].ugramFeatures[j] = this->getFeatureId(rSentence, sentence, CRFtemplate.unigrams[j], i);
			}

			sentence.tokens[i].bgramFeatures.resize(CRFtemplate.bigrams.size());
			for (size_t j = 0; j < CRFtemplate.bigrams.size(); j++)
			{
				sentence.tokens[i].bgramFeatures[j] = this->getFeatureId(rSentence, sentence, CRFtemplate.bigrams[j], i, true);
			}
			sentence.tokens[i].label = this->labelMap.getId(rSentence.words[i].row_features[this->column_szie-1]);
		}
	}

	void set_sentence_max_length()
	{
		int max_length= 0;
		for (auto& sentence : this->sentences)
			max_length = std::max(max_length, int(sentence.tokens.size()));
		this->sentence_max_length = max_length;
	}

public:
	Corpus(Template& CRFtemplate, const Example& example)
	{
		this->check(CRFtemplate);
		this->column_szie = example.get_columnSize();
		for (auto& sentence:example.sentences)
		{
			for (auto& word:sentence->words)
			{
				const std::string& label = *(word.row_features.end() - 1);
				this->labelMap.getId(label,true);
			}
		}
		this->idMap.setStep(this->labelMap.getMaxId()+1);
		this->sentences.resize(example.sentences.size());
		for (size_t i = 0; i < example.sentences.size(); i++)
		{
			this->RowSentence2Sentence(*(example.sentences[i]), this->sentences[i], CRFtemplate);
		}
		this->labelMap.getId("*");
		this->labelMap.getId("#");
		this->set_sentence_max_length();
	}

	Corpus(Model& model, Example&example)
	{
		auto& CRFtemplate = model.CRFtemplate;
		this->idMap = model.idMap;
		this->labelMap = model.labelMap;
		this->check(CRFtemplate);
		this->column_szie = example.get_columnSize();
		for (auto& sentence : example.sentences)
		{
			for (auto& word : sentence->words)
			{
				const std::string& label = *(word.row_features.end() - 1);
				this->labelMap.getId(label, true);
			}
		}
		this->idMap.setStep(this->labelMap.getMaxId() + 1);
		this->sentences.resize(example.sentences.size());
		for (size_t i = 0; i < example.sentences.size(); i++)
		{
			this->RowSentence2Sentence(*(example.sentences[i]), this->sentences[i], CRFtemplate);
		}
		this->labelMap.getId("*");
		this->labelMap.getId("#");
		this->set_sentence_max_length();

	}

};

#endif
