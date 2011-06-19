

#include <stdlib.h>
#include <map>

#include "NaiveBayes.h"
#include "Matematica.h"

#define TAG "NaiveBayes"

NaiveBayes::NaiveBayes(Statistics* st): usingContentCredibility(false), stats(st), Normalizer(10.0), microF1(0), macroF1(0) 
{
    usingNormalEstimator = stats ->getNormalEstimator();
}

NaiveBayes::~NaiveBayes(){
}

double NaiveBayes::getContentCredibility(string term, string docClass){

	if(!usingContentCredibility) return 1.0;
	if(contentCredibility.size() == 0) return 1.0;
	string idx = getCompIndex(term,docClass);

	if(contentCredibility.find(idx) != contentCredibility.end()) return contentCredibility[idx] * Normalizer;

	return 1.0;
}

void NaiveBayes::setContentCredibilityMap(map<string, double>& contentCred){
	contentCredibility = contentCred;
}

double NaiveBayes::getGraphCredibility(int g, string id, string classId){
	//TODO: using graph credibility 
	//if(!usingGraphCredibility) return 1.0;
	if(graphsCredibility.size() == 0) return 1.0;
	string idx = getCompIndex(id,classId);

	if( graphsCredibility[g].count(idx) > 0) return graphsCredibility[g][idx] * Normalizer;

	return 1.0;
}

void NaiveBayes::setGraphCredibilityMaps(vector< map<string, double> >& gCred){
	graphsCredibility = gCred;
}

void NaiveBayes::useContentCredibility(bool usingCred){

	TRACE_V(TAG,"useContentCredibility");
	usingContentCredibility = usingCred;	
}

void  NaiveBayes::setCredibilityConfigurations(bool termCredibility, vector<string> graphsNames){

	TRACE_V(TAG,"setCredibilityConfigurations");
	usingContentCredibility = termCredibility;
	graphsNames = graphsNames;
}

void NaiveBayes::useNormalEstimator(bool usingNEstimator){
	
    TRACE_V(TAG,"useNormalEstimator");
    usingNormalEstimator = usingNEstimator;
    
}

void NaiveBayes::train(Examples& exs){

	TRACE_V(TAG,"train");

	for(ExampleIterator e = exs.getBegin(); e != exs.getEnd(); e++){

//      cout<<" Example id = " << (e)->getId()<<endl;

		vector<string> tokens = (e)->getCategoricalTokens();
		string exampleClass = (e)->getClass();

//      cout<<" Tokens categoricos  =  " << tokens.size() << endl;
		for(unsigned int i = 3; i < tokens.size()-1; i+=2){
			int tf = atoi(tokens[i+1].c_str());
			string termId = tokens[i];
			sumTF[exampleClass] += tf * (getContentCredibility(termId,exampleClass));
		}

        vector<double> numTokens = (e)->getNumericalTokens();
//      cout<<" Tokens numericos  =  " << numTokens.size() <<endl;

        if(!usingNormalEstimator){
            for(unsigned int i = 0 ; i < numTokens.size(); i++){
//              cout<<"Inserindo token " << numTokens[i] << "  class = " << exampleClass << endl;
                means[i][exampleClass] += numTokens[i]; 
            }
        }
	}
    if(!usingNormalEstimator){
        for(unsigned int i = 0; i < means.size(); i++){
            for(set<string>::iterator classIt = stats->getClasses().begin(); classIt != stats->getClasses().end(); classIt++){
                //          cout<<" Class = " <<*classIt << " sum  =  " << means[i][*classIt] << endl;
                means[i][*classIt] /= stats->getSumDFperClass(*classIt);
            }
        }

        //Calculate deviations:
        for(ExampleIterator e = exs.getBegin(); e != exs.getEnd(); e++){

            vector<double> numTokens = (e)->getNumericalTokens();
            string exampleClass = (e)->getClass();

            for(unsigned int i = 0 ; i < numTokens.size(); i++){
                deviations[i][exampleClass] += ((numTokens[i] - means[i][exampleClass]) * (numTokens[i] - means[i][exampleClass])); 
            }
        }

        for(unsigned int i = 0; i < deviations.size(); i++){
            for(set<string>::iterator classIt = stats->getClasses().begin(); classIt != stats->getClasses().end(); classIt++){
                if( isZero(deviations[i][*classIt]))
                    cerr<<"WARNING: Deviations is 0 or less for attibute " << i << " in class "<< *classIt<<endl;
                else
                    deviations[i][*classIt] = sqrt(deviations[i][*classIt] / (stats->getSumDFperClass(*classIt) - 1));
            }
        }
    }
    //Using normal estimator
    else{

        int numberOfNumericalAttibutes = exs.getNumberOfNumericalAttibutes();
        for(int i = 0; i < numberOfNumericalAttibutes; i++){
            
            double numPrecision = 0.01; //Default Precision value
            exs.sort(i);
            

            ExampleIterator it = exs.getBegin();
		    Example e = *it;
            double lastValue = e.getNumericalValue(i);
            double currentValue , deltaSum = 0.0;
            int distinct = 0;
    
            for(;it != exs.getEnd(); it++){

                e = *it;
                currentValue = e.getNumericalValue(i);
                if (currentValue != lastValue) {
                    deltaSum += currentValue - lastValue;
                    lastValue = currentValue;
                    distinct++;
                }
            }
            if (distinct > 0){
                numPrecision = deltaSum / distinct;
            }

            for(set<string>::iterator classIt = stats->getClasses().begin(); classIt != stats->getClasses().end(); classIt++){
                NormalEstimator ne(numPrecision);
                nEstimator[i][*classIt] = ne;
            }
        }
        for(ExampleIterator it = exs.getBegin();it != exs.getEnd(); it++){
		    Example e = *it;

            int numberOfNumericalAttibutes = exs.getNumberOfNumericalAttibutes();
            for(int i = 0; i < numberOfNumericalAttibutes; i++){
                nEstimator[i][e.getClass()].addValue(e.getNumericalValue(i), 1.0);
            }
        }
    }
/*TESTE: 
    cout<<"PARA CONFERIR: "<<endl; 
    for(unsigned int i = 0; i < means.size(); i++){
		for(set<string>::iterator classIt = stats->getClasses().begin(); classIt != stats->getClasses().end(); classIt++){
            cout<< means[i][*classIt] <<  " ";
        }
    }
    cout<<endl;
    for(unsigned int i = 0; i < deviations.size(); i++){
		for(set<string>::iterator classIt = stats->getClasses().begin(); classIt != stats->getClasses().end(); classIt++){
            cout<< deviations[i][*classIt] <<  " ";
        }
    }
*/
}

void NaiveBayes::test(Examples& exs){

    TRACE_V(TAG,"test");

    map<string,unsigned long> totalDocsPerClass;
    map<string,double> condDenominator;
    map<string,double> aPriori;

    //Statistics:
    map<string,unsigned long long> classHits;
    map<string,unsigned long long> classMiss;
    map<string,unsigned long long> mappedDocs;
    map<string,unsigned long long> docsPerClass;


    for(set<string>::iterator classIt = stats->getClasses().begin(); classIt != stats->getClasses().end(); classIt++){
        condDenominator[*classIt] = computeConditionalDenominator(*classIt);
        aPriori[*classIt] = computeApriori(*classIt);
    }

    for(ExampleIterator it = exs.getBegin(); it != exs.getEnd(); it++){
        Example ex = *it;

        vector<string> catTokens = ex.getCategoricalTokens();	
        vector<double> numTokens = ex.getNumericalTokens();	
        string id = ex.getId();
        string classId = ex.getClass();

        map<string, double> probMap;
        double probNormalizer = 0.0;

        for(set<string>::iterator classIt = stats->getClasses().begin(); classIt != stats->getClasses().end(); classIt++){

            double probCond = 0.0;

            //cout<<"id = " << id << " class = " << *classIt<<endl;
            //numeracal tokens evaluate
            //cout<<"usingNormalEstimator = " << usingNormalEstimator <<endl;
            for(unsigned int i = 0; i < numTokens.size(); i++){
                double tmp = 0.0;

                if(!usingNormalEstimator){
                  tmp =  std::max(1e-75,gaussianDistribution(numTokens[i], means[i][*classIt], deviations[i][*classIt]));
                }else{
                      tmp = std::max(1e-75, nEstimator[i][*classIt].getProbability(ex.getNumericalValue(i)));
                      //tmp *= Normalizer;
        //            cout<<"Tokens = " << i <<" original = " << nEstimator[i][*classIt].getProbability(i) << " class =  " << *classIt << " val = " << tmp<< " mean = " << nEstimator[i][*classIt].getMean() << " deviations = " << nEstimator[i][*classIt].getStdDev() << " precision = " << nEstimator[i][*classIt].getPrecision()<< " sumofweights = " << nEstimator[i][*classIt].getSumOfWeights() << endl;
                }                                                                                            
                
        //          cout<<"Tokens = " << i << " val = " << tmp<< " mean = " << means[i][*classIt] << " deviations = " << deviations[i][*classIt] << endl;
        //          cout<<"log(val) = " << my_log_avoid_zero(tmp)<<endl;
        //          cout<<"log(val) = " << log(tmp)<<endl<<endl;
                probCond += log(tmp);
            }

            //categorical tokens evaluate
            for(unsigned int i = 3 ; i < catTokens.size(); i+=2){
                string termId = catTokens[i];
                int tf = atoi(catTokens[i+1].c_str());
                double numerator = computeConditionalNumerator(termId, *classIt);
                double fraction = numerator / condDenominator[*classIt];
                probCond += (tf * log(fraction)); 
            }

            vector<double> graphsCreds(graphsCredibility.size());

            for(unsigned int g = 0 ; g < graphsCredibility.size(); g++){
                graphsCreds[g] = 0.5 + getGraphCredibility(g,id,*classIt);
            }

            double PofCGivenD = log(aPriori[*classIt]) + probCond;
            for(unsigned int g = 0 ; g < graphsCreds.size(); g++){
                PofCGivenD += log(graphsCreds[g]);
            }

            probMap[*classIt] = PofCGivenD;
            //cout<<" id = " << id << " class = " << *classIt << " prob = " << PofCGivenD <<endl;
            probNormalizer += PofCGivenD;

        }
        double greatestProb = -1e31;
        string predictedLabel;

        for(set<string>::iterator classIt = stats->getClasses().begin(); classIt != stats->getClasses().end(); classIt++){
            probMap[*classIt] = -1.0 * probMap[*classIt]/probNormalizer;
            //cout<<"Class = " << *classIt << " prob = " << probMap[*classIt] << endl;

            if(greaterThan(probMap[*classIt], greatestProb)){
                greatestProb = probMap[*classIt];
                predictedLabel = *classIt;
            }
        }

        computeConfusionMatrix(classId, predictedLabel);

        //        if(io->usingPredictionsFile)
        savePrediction(id, classId, predictedLabel);

        if(predictedLabel == classId){
            classHits[classId] ++;			
        }
        else{
            classMiss[classId]++;
        }

        mappedDocs[predictedLabel]++;
        docsPerClass[classId]++;
    }

    calculateF1(classHits,classMiss,docsPerClass, mappedDocs);
}

void NaiveBayes::calculateF1(map<string, unsigned long long> hits, map<string, unsigned long long> misses, map<string, unsigned long long> docsPerClass, map<string, unsigned long long> mappedDocs){

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


double NaiveBayes::computeApriori(string classId){
	return (getValue(stats->getSumDFPerClass(), classId) * 1.0) / (stats->getTotalDocs() * 1.0);
}

double NaiveBayes::computeConditionalDenominator(string classId){
	return getValue(sumTF, classId) + stats->getVocabularySize(); 
}

double NaiveBayes::computeConditionalNumerator(string termId, string classId){
	string idx =  getCompIndex(termId,classId);

	return (getValue(stats->getTFPerClass(),idx) * getContentCredibility(termId, classId)) + 1.0;
}

double NaiveBayes::getMicroF1(){
	return microF1;
}

double NaiveBayes::getMacroF1(){
	return macroF1;
}

double NaiveBayes::gaussianDistribution(double value, double mean, double deviation){

    if(isZero(deviation)){
    //  cerr<<"WARNING: deviation is 0! Shouldn't you ignore this attibute?"<<endl;
        return 1.0;
    }
    double PI = 3.141592653589793238462643383279502884197169399375;
    
//    cout<<"value = " <<value<< " mean = " <<mean << " deviation = " << deviation <<endl;

    return (1.0/(sqrt(2.0 * PI) * deviation)) * ( exp(-1.0 * (value - mean) * (value - mean) / (2.0 * deviation * deviation)));

}

void NaiveBayes::savePrediction(string exampleId, string correctClass, string predictedClass){
    prediction[exampleId] = guess(correctClass,predictedClass);
}

void NaiveBayes::computeConfusionMatrix(string actual, string predicted){
    confusionMatrix[actual][predicted] ++;
}

void NaiveBayes::showConfusionMatrix(){

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

void NaiveBayes::setPrintPrediction(bool pred){
    printPrediction = pred;
}

void NaiveBayes::showPredictions(){
    cout<<"Predictions:"<<endl;
    for(map<string,guess>::iterator it = prediction.begin(); it!= prediction.end(); it++){
        guess g = it->second;
        cout<<it->first<< " from " << g.correctClass << "  ->>> " << g.predictedClass<< endl;
    }
}

