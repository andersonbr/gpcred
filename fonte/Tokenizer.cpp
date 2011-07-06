#include <cstdlib>

#include "Tokenizer.h"

void Tokenizer::stringTokenize(const string& str, vector<string>& tokens, const string& delimiters)
{
	//Skip delimiters at beginning.
	string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	
	// Find first "non-delimiter".
	string::size_type pos = str.find_first_of(delimiters, lastPos);
	
	while (string::npos != pos || string::npos != lastPos) {

		// Found a token, add it to the vector.
		tokens.push_back(str.substr(lastPos, pos - lastPos));

		// Skip delimiters.  Note the "not_of"
		lastPos = str.find_first_not_of(delimiters, pos);

		// Find next "non-delimiter"
		pos = str.find_first_of(delimiters, lastPos);
	}
}

string Tokenizer::cleanClassToken(string& token){
	return token.replace(0,6,"");
}

void Tokenizer::setCategoricalTokens(vector<string>& tokens, vector<string>& catTokens, int categoricalCollums){

    if(tokens.size() < 3 || categoricalCollums == 0) return;
    
    int catColl = categoricalCollums;
    for(unsigned int i = 3; i < tokens.size() && catColl-- ; i++){
       catTokens.push_back( tokens[i].c_str() );
    }
    tokens.erase(tokens.begin() + 3, tokens.begin() + 3 + categoricalCollums);
}

void Tokenizer::setNumericalTokens(vector<string>& tokens, vector<double>& numTokens, int numericalCollums){

    if(tokens.size() < 3 || numericalCollums == 0) return;

    int numColl = numericalCollums;
    for(unsigned int i = 3; i < tokens.size() && numColl-- ; i++){
       numTokens.push_back( atof(tokens[i].c_str()) );
    }
    tokens.erase(tokens.begin() + 3, tokens.begin() + 3 + numericalCollums);

/*    
    cout<<"Tokens categoricos lidos:"<<endl;
    for(unsigned int i = 0; i < tokens.size(); i++){
        cout<<" i = " << i << " " << tokens[i]<<endl;
    }
    
    cout<<"Tokens numericos lidos:"<<endl;
    for(unsigned int i = 0; i < numTokens.size(); i++){
        cout<<" i = " << i << " " << numTokens[i]<<endl;
    }
*/
}


