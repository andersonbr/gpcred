
#include "KNN.h"
#include "Matematica.h"

#define TAG "KNN"

KNN::KNN(Statistics* st): usingContentCredibility(false), stats(st), Normalizer(10.0), microF1(0), macroF1(0) 
{
    usingNormalEstimator = stats->getNormalEstimator();
}

KNN::~KNN(){
}

double KNN::getContentCredibility(string term, string docClass){

	if(!usingContentCredibility) return 1.0;
	if(contentCredibility.size() == 0) return 1.0;
	string idx = getCompIndex(term,docClass);

	if(contentCredibility.find(idx) != contentCredibility.end()) return contentCredibility[idx] * Normalizer;
	return 1.0;
}

double KNN::getGraphCredibility(int g, string id, string classId){
	//TODO: using graph credibility 
	//if(!usingGraphCredibility) return 1.0;
	if(graphsCredibility.size() == 0) return 1.0;
	string idx = getCompIndex(id,classId);

	if( graphsCredibility[g].count(idx) > 0) return graphsCredibility[g][idx] * Normalizer;

	return 1.0;
}

void KNN::setGraphCredibilityMaps(vector< map<string, double> >& gCred){
	graphsCredibility = gCred;
}

void KNN::setContentCredibilityMap(map<string, double>& contentCred){
	contentCredibility = contentCred;
}

void KNN::useContentCredibility(bool usingCred){

	TRACE_V(TAG,"useContentCredibility");
	usingContentCredibility = usingCred;	
}

void  KNN::setCredibilityConfigurations(bool termCredibility, vector<string> graphsNames){

	TRACE_V(TAG,"setCredibilityConfigurations");
	usingContentCredibility = termCredibility;
	graphsNames = graphsNames;
}

void KNN::train(Examples& exs){

	TRACE_V(TAG,"train");
}

void KNN::test(Examples& exs){

    TRACE_V(TAG,"test");

 
}

void KNN::calculateF1(map<string, unsigned long long> hits, map<string, unsigned long long> misses, map<string, unsigned long long> docsPerClass, map<string, unsigned long long> mappedDocs){

	macroF1 = 0.0;
	microF1 = 0.0;

	unsigned long totalHits = 0;
	unsigned long totalDocs = 0;

	for(set<string>::iterator classIt = stats->getClasses().begin(); classIt != stats->getClasses().end(); classIt++) {

		double precision = (mappedDocs[*classIt] == 0) ? 0.0 : ((double) hits[*classIt] / (double) mappedDocs[*classIt]);
		double recall = (docsPerClass[*classIt] == 0) ? 0.0 : ((double) hits[*classIt] / (double)(docsPerClass[*classIt]));

		totalHits += hits[*classIt];
		totalDocs += docsPerClass[*classIt];

		double F1 = (precision+recall == 0) ? 0.0 : (2.0 * precision * recall) / (precision + recall);
		macroF1 += F1;
	}

	macroF1 /= stats->getClasses().size();
	microF1 = ((double)totalHits / (double)totalDocs);

	cerr << "MicroF1: " << microF1 << endl;
	cerr << "1 - MicroF1: " << 1.0 -  microF1 << endl;

	cerr << "MacroF1: " << macroF1 << endl;
	cerr << "1 - MacroF1: " << 1.0 -  macroF1 << endl;
}

double KNN::getMicroF1(){
	return microF1;
}

double KNN::getMacroF1(){
	return macroF1;
}

void KNN::savePrediction(string exampleId, string correctClass, string predictedClass){
    prediction[exampleId] = guess(correctClass,predictedClass);
}

void KNN::computeConfusionMatrix(string actual, string predicted){
    confusionMatrix[actual][predicted] ++;
}

void KNN::showConfusionMatrix(){

    map<string, int> totalCollums;
    int totalLine = 0;


    cout<<"Confusion Matrix: "<<endl;
    cout<<"\t\t";
	for(set<string>::iterator class1 = stats->getClasses().begin(); class1 != stats->getClasses().end(); class1++){
        cout<<*class1<<"\t";
    }
    cout<<endl;

	for(set<string>::iterator class1 = stats->getClasses().begin(); class1 != stats->getClasses().end(); class1++){
        cout<<*class1<<"\t\t";
        totalLine = 0;

	    for(set<string>::iterator class2 = stats->getClasses().begin(); class2 != stats->getClasses().end(); class2++){
            int qt = confusionMatrix[*class1][*class2];
            totalCollums[*class2] += qt;
            totalLine += qt;
            cout<< qt << "\t";   
        }
        cout<<totalLine<<endl;
    }
	
    cout<<"Pred->\t\t";
    for(set<string>::iterator class1 = stats->getClasses().begin(); class1 != stats->getClasses().end(); class1++){
        cout<<totalCollums[*class1]<<"\t";
    }
    cout<<endl;
}

void KNN::setPrintPrediction(bool pred){
    printPrediction = pred;
}

void KNN::showPredictions(){
    cout<<"Predictions:"<<endl;
    for(map<string,guess>::iterator it = prediction.begin(); it!= prediction.end(); it++){
        guess g = it->second;
        cout<<it->first<< " from " << g.correctClass << "  ->>> " << g.predictedClass<< endl;
    }
}

void KNN::printFinalOutFile(ostream& out, string msg){
    out<< msg<< "\t"<< microF1<< "\t"<< macroF1<< endl;
}


