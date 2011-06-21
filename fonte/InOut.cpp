
#include <fstream>
#include <iostream>
#include <cstdlib>

#include "InOut.h"
#include "Tokenizer.h"
#include "Examples.h"
#include "Util.h"

using namespace std;
#define TAG "InOut"

InOut::InOut(string basename, int seed){

	this->basename = basename;
	
    string histf = this->basename + ".hist";
	string bestf = this->basename + ".best";

	histFile.open(histf.c_str());
	bestFile.open(bestf.c_str());
    
    _usingBrunoroFile = false;
	_usingPredictionsFile = false;
//    _usingValidationSet = false;
    _usingEvaluateFile = false;
    _usingFinalOutFile = false;
    numericalCollums = 0;
    numExamplesOriginalTrain  = 0;
    numExamplesOriginalTest   = 0;

    srand(seed);
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
    numExamplesOriginalTrain = 0;
    numExamplesOriginalTest = 0;
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
    numExamplesOriginalTrain += trainSet.size();
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
    numExamplesOriginalTest += testSet.size();
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

void InOut::setFinalOutFile(string finalOutFileName){
    _usingFinalOutFile = true;
    finalOutFile.open(finalOutFileName.c_str());
}

ofstream& InOut::getFinalOutFile(){
    return finalOutFile;
}

bool InOut::isUsingFinalOutFile(){
    return _usingFinalOutFile;
}

void InOut::mergeTrainAndTest(){

    for(ExampleIterator it = testSet.getBegin(); it != testSet.getEnd(); it++){
        trainSet.add(*it);
    }
    testSet.clear();
    cout<<"Train - old size = " << numExamplesOriginalTrain << " new size " << trainSet.size()<<endl;
    cout<<"Test - old size = " << numExamplesOriginalTest << " new size " << testSet.size()<<endl;
}

void InOut::makeNewTest(){
    int prob = (int)((100.0 * numExamplesOriginalTest) / (numExamplesOriginalTrain + numExamplesOriginalTest));

    for(ExampleIterator it = trainSet.getBegin(); it != trainSet.getEnd(); ){
        if( rand()%100 <= prob ){
            testSet.add(*it);
            it = trainSet.erase(it);
        }
        else{
            it++;
        }
    }
    
    cout<<"Train - newest size " << trainSet.size()<<endl;
    cout<<"Test - newest size " << testSet.size()<<endl;
}

