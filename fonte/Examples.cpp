
#include "Examples.h"

#include <algorithm>

///
//  Example functions.
///

vector<string>& Example::getCategoricalTokens(){
	return categoricalTokens;
}

vector<double>& Example::getNumericalTokens(){
	return numericalTokens;
}

int Example::getNumberOfNumericalTokens(){
    return numericalTokens.size();
}

double Example::getNumericalValue(unsigned int index) const{
    if(index > numericalTokens.size()) return 0.0;
    return numericalTokens[index];
}

string Example::getClass(){
	return docClass;
}

string Example::getId(){
	return id;
}

bool Example::operator <(const Example& e) const{
	return id.compare(e.id);
}

bool Example::sortByNumericalAttribute(const Example& e, const unsigned int index) const{
    if(numericalTokens.size() < index) return false;

    else return numericalTokens[index] < e.getNumericalValue(index);
}

///
//  Examples functions.
///
void Examples::add(Example& e){
	sexamples.push_back(e);
}

ExampleIterator Examples::getBegin(){
	return sexamples.begin();
}

ExampleIterator Examples::getEnd(){
	return sexamples.end();
}

int Examples::size(){
	return sexamples.size();
}

void Examples::sort(unsigned int index){
   std::sort( sexamples.begin(), sexamples.end(), sorter(index) );
}

int Examples::getNumberOfNumericalAttibutes(){
    if(sexamples.size() == 0) return 0;
    return sexamples[0].getNumberOfNumericalTokens();

}

void Examples::clear(){
    sexamples.clear();
}

