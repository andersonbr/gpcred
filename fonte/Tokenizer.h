#ifndef __TOKENIZER__H__
#define __TOKENIZER__H__

#include<vector>
#include<cstring>
#include<cstdio>
#include<iostream>

using namespace std;

class Tokenizer{

	public:
        static void stringTokenize(const string& str, int numericalCollums, int categoricalCollums, vector<string>& tokens, vector<int>& freqTokens, const string& delimiters = " ");
//        static void stringTokenize(const string& str, vector<string>& tokens, vector<int>& freqTokens, const string& delimiters =" ");
        static void stringLineTokenize(const string& str, vector<string>& tokens, const string& delimiters =" ");
		static string cleanClassToken(string& token);
        static void setNumericalTokens(vector<string>& tokens, vector<double>& numTokens, int numericalCollums);
        static void setCategoricalTokens(vector<string>& tokens, vector<string>& catTokens, int categoricalCollums);

};

#endif

