#ifndef __EXAMPLE__H__
#define __EXAMPLE__H__

#include<vector>
#include<iostream>

#define ExampleIterator vector<Example>::iterator

using namespace std;

class Example{
	private:
		string id;
		string docClass;
		vector<string> textTokens;
		vector<string> categoricalTokens;
		vector<double> numericalTokens;
/*
		int difficult;
		int age;
		
		bool used;
		static int age_power;
		static int difficult_power;
*/
	public:
		Example() {}
		Example(string id, string dclass, vector<string> textToks, vector<string> catTokens, vector<double> numTokens) : id(id), docClass(dclass), textTokens(textToks), categoricalTokens(catTokens), numericalTokens(numTokens)
//			{ age= 1000; difficult = 1000;}
			{}

		virtual ~Example(){}
		
        vector<string>& getTextTokens();
        vector<string>& getCategoricalTokens();
        vector<double>& getNumericalTokens();
        int getNumberOfNumericalTokens();
        int getNumberOfCategoricalTokens();
		string getClass();
		string getId();
 
		bool operator <(const Example& e) const;
        bool sortByNumericalAttribute(const Example& e, const unsigned int index) const;
        double getNumericalValue(unsigned int index) const;

/*		
		long long int getScore() const;
		string getId(){return id;}
		int getDifficult(){ return difficult;}
		void increaseDifficult();
		void setDifficult(int d);
		void release();
		void use();
		void doNotUse();
		bool isUsed();
		void print() const;
		string getLine();
*/
};

class Examples{
	private:
		vector<Example> sexamples;

	public:
		Examples() {}
		virtual ~Examples(){}
		void add(Example);
		vector<Example>::iterator getBegin();
		vector<Example>::iterator getEnd();
		int size();
        void clear();

        void sort(unsigned int index);
    
        int getNumberOfNumericalAttibutes();
        int getNumberOfCategoricalAttibutes();
        ExampleIterator erase(ExampleIterator it);
};


class sorter {
    unsigned int index;
    public:
        sorter(unsigned int i) : index(i) {}
    bool operator()(const Example& o1, const Example& o2) {
        return o1.sortByNumericalAttribute(o2, index);
    }
};

#endif

