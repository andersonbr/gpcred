
#include <fstream>
#include <iostream>

#include "InOut.h"
#include "Tokenizer.h"
#include "Examples.h"
#include "Util.h"

using namespace std;
#define TAG "InOut"

InOut::InOut(string basename){

	this->basename = basename;
	
    string histf = this->basename + ".hist";
	string bestf = this->basename + ".best";

	histFile.open(histf.c_str());
	bestFile.open(bestf.c_str());
    
    _usingBrunoroFile = false;
	_usingPredictionsFile = false;
//    _usingValidationSet = false;
    _usingEvaluateFile = false;
    numericalCollums = 0;
}

InOut::~InOut(){
    clear();
}

void InOut::clear(){
    if(histFile)
        histFile.close();
    if(bestFile)
        bestFile.close();
    if(_usingBrunoroFile)
        brunoroFile.close();
    if(_usingPredictionsFile)
        predictionsFile.close();
    if(_usingEvaluateFile)
        evaluateFile.close();

    trainSet.clear();
//    validationSet.clear();
    testSet.clear();
}

void InOut::setBrunoroFile(string brunoroFilename){
    _usingBrunoroFile = true;
    brunoroFile.open(brunoroFilename.c_str());
}

bool InOut::isUsingBrunoroFile(){
    return _usingBrunoroFile;
}

ofstream& InOut::getBrunoroFile(){
    return brunoroFile;
}

void InOut::setPredictionsFile(string predictionsFilename){
    _usingPredictionsFile = true;
    predictionsFile.open(predictionsFilename.c_str());
}

bool InOut::isUsingPredictionsFile(){
    return _usingPredictionsFile;
}

ofstream& InOut::getPredictionsFile(){
    return predictionsFile;
}

ofstream& InOut::getHistFile(){
	return histFile;
}

ofstream& InOut::getBestFile(){
	return bestFile;
}

void InOut::read(const char* filename, int option){
	
	TRACE_V(TAG,"read method.");
	
	ifstream file(filename, ifstream::in);

	if(file){
		string line = "";
		
		while(std::getline(file, line)){

			vector<string> catTokens;
            vector<double> numTokens;
			Tokenizer::stringTokenize(line, catTokens, ";");
			
            //Blank line
			if(catTokens.size() < 3) continue;
            
            Tokenizer::setNumericalTokens(catTokens, numTokens, numericalCollums);

			string docClass = Tokenizer::cleanClassToken(catTokens[2]);
			string id = catTokens[0];

			Example e(id,docClass,catTokens,numTokens);

			switch(option){
			
				case train_option:
					trainSet.add(e);
				break;

//				case validation_option:
//					validationSet.add(e);
//				break;

				case test_option:
					testSet.add(e);
				break;

				default:
					cerr<<"Error. Invalid option."<<endl;
			}
		}
	}
	else{
		cerr<<"Error! File not found."<<endl;
		exit(1);
	}
	file.close();
}

void InOut::readTrain(const char* filename){
	TRACE_V(TAG,"readTrain method -> " <<  filename);
	
	read(filename,train_option);
}

/*void InOut::readValidation(const char* filename){
	TRACE_V(TAG,"readValidation method.");
    _usingValidationSet = true;
	read(filename, validation_option);
}
*/

void InOut::readTest(const char* filename){
	TRACE_V(TAG,"readTest method -> " << filename);
	
	read(filename, test_option);
}

void InOut::setNumericalCollums(int nc){
    numericalCollums= nc;
}

int InOut::getNumericalCollums(int nc){
    return numericalCollums;
}

Examples& InOut::getTrain(){
	return trainSet;
}

/*
Examples& InOut::getValidation(){
	return validationSet;
}
*/

Examples& InOut::getTest(){
	return testSet;
}

bool InOut::isUsingEvaluateFile(){
    return _usingEvaluateFile;
}

void InOut::setEvaluateFile(string evaluateFilename){
    _usingEvaluateFile = true;
    evaluateFile.open(evaluateFilename.c_str());
}

ofstream& InOut::getEvaluateFile(){
    return evaluateFile;
}

string InOut::getGPParameterConfigFileName(){
    return GPParameterConfigFileName;
}

void InOut::setGPParameterConfigFileName(string fileName){
    GPParameterConfigFileName = fileName;
}

