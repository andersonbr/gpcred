#include <iostream>

#include "IGP.h"

using namespace std;

IGP::~IGP(){

}

void IGP::createInitialPopulation(){
	genCounter = 0;
}

int IGP::getActualGen(){
	return genCounter; 
}

void IGP::evolve(){
	genCounter++;
}

int IGP::getMaxGen(){
	return  maxGen;
}

void IGP::setSavePopFileName(std::string filename){
	saveFileName = filename;
}

void IGP::setLoadPopFileName(std::string filename){
	loadFileName = filename;
}


