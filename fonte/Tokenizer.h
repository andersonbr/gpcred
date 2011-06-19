#ifndef __TOKENIZER__H__
#define __TOKENIZER__H__

#include<vector>
#include<cstring>
#include<cstdio>
#include<iostream>

using namespace std;

class Tokenizer{

	public:
		static void stringTokenize(const string& str, vector<string>& tokens, const string& delimiters = " ");
		static string cleanClassToken(string& token);
        static void setNumericalTokens(vector<string>& tokens, vector<double>& numTokens, int numericalCollums);

};

#endif

