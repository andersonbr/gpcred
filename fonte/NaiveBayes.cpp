#include <stdlib.h>
#include <map>

#include "NaiveBayes.h"
#include "Matematica.h"

#define TAG "NaiveBayes"

NaiveBayes::NaiveBayes(Statistics* st): usingContentCredibility(false), usingGraphCredibility(false), stats(st), Normalizer(10.0), microF1(0), macroF1(0) 
{
    usingNormalEstimator = stats ->getNormalEstimator();
}

NaiveBayes::~NaiveBayes(){
}

double NaiveBayes::getContentCredibility(string term, string docClass){

	if(!usingContentCredibility) return 1.0; //stats->getIDF(term);
	if(contentCredibility.size() == 0) return 1.0; //stats->getIDF(term);
	string idx = getCompIndex(term,docClass);

	if(contentCredibility.find(idx) != contentCredibility.end()) return contentCredibility[idx] * Normalizer;

	return 1.0;// stats->getIDF(term);
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
double NaiveBayes::getCategoricalCredibility(int i, string token, string classId){
   
    double occurrences = tupleValue[i][classId][token] + 1.0; //laplacian correction
    double freq = 1.0 * (stats->getSumDFperClass(classId) + tupleValue[i][classId].size());

    double apareceForaClasse = 0;
    double aparece = 0;
    for(set<string>::iterator classIt = stats->getClasses().begin(); classIt != stats->getClasses().end(); classIt++) {
        if(*classIt != classId)
            apareceForaClasse += tupleValue[i][*classIt][token];
        aparece += tupleValue[i][*classIt][token];
    }

    double PdeTeNaoC = apareceForaClasse / stats->getTotalDocs();


    double PdeC = stats->getSumDFperClass(classId) / ( 1.0 * stats->getTotalDocs());
    double PdenaoC = 1.0 - PdeC;
    
    double PdeT = 1.0 / (tupleValue[i][classId]).size();
    double PdenaoT = 1.0 - PdeT;

    double PdeTtalqueC = (occurrences/freq);
    double PdeTtalqueNaoC =  (PdeTeNaoC + 1.0) / (1.0-PdeC + 1.0) ; //suavizada
    double PdeNaoTtalqueNaoC = 1.0 - PdeTtalqueNaoC;
    double PdeNaoTtalqueC = 1.0 - PdeTtalqueC;
    
//    return PdeTtalqueC;
    double gss = PdeTtalqueC * PdeNaoTtalqueNaoC - PdeTtalqueNaoC * PdeNaoTtalqueC;
    double den = sqrt( PdeT*PdenaoT * PdeC *PdenaoC );
/*
    cout<<"P de t talque C = " << PdeTtalqueC <<endl;
    cout<<"P de naoT talque nao C = "<<  PdeNaoTtalqueNaoC <<endl;
    cout<<"P de T talque nao C = " << PdeTtalqueNaoC <<endl;
    cout<<"P de nao T talque C = " <<PdeNaoTtalqueC<<endl;
    cout<<"P de T = " << PdeT <<endl;
    cout<<"P de naoT = " << PdenaoT <<endl;
    cout<<"P de C = " << PdeC <<endl;
    cout<<"P de naoC = " << PdenaoC<<endl;

    cout<<gss/den<<endl;
*/
    double chi2 = gss/den;
    double am = occurrences / (aparece + tupleValue[i][classId].size()); 
    double ig = (PdeTtalqueC * log(PdeTtalqueC / (PdeT*PdeC))) + 
                (PdeNaoTtalqueC * log(PdeNaoTtalqueC / (PdenaoT*PdeC))) +
                (PdeTtalqueNaoC * log(PdeTtalqueNaoC / (PdeT*PdenaoC))) +
                (PdeNaoTtalqueNaoC * log(PdeNaoTtalqueNaoC / (PdenaoT*PdenaoC)));
//    cout<<"am = " <<am<<endl;
    return 0.5 + ig / occurrences;//0.5+(1.0/occurrences) + 0.001*am; //1.0/am;
}
void NaiveBayes::setGraphCredibilityMaps(vector< map<string, double> >& gCred){
    usingGraphCredibility = true;
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

		vector<string> tokens = (e)->getTextTokens();
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

        vector<string> catTokens = (e)->getCategoricalTokens();
        for(unsigned int i = 0; i < catTokens.size(); i++){
            tupleValue[i][exampleClass][catTokens[i]]++;   
            //cout<<"i = " << i << " class =  " << exampleClass << " " << catTokens[i]<< " "<< tupleValue[i][exampleClass][catTokens[i]]<<endl;   
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

        vector<string> textTokens = ex.getTextTokens();	
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
                double tmp2 = 0.0;
                double tmp3 = 0.0;

                if(!usingNormalEstimator){
                    tmp =  std::max(1e-75,gaussianDistribution(numTokens[i], means[i][*classIt], deviations[i][*classIt]));
                    tmp2 =  std::max(1e-75,exponentialDistribution(means[i][*classIt] + deviations[i][*classIt], 0.5));
                    tmp3 =  std::max(1e-75,weibullDistribution(means[i][*classIt] + deviations[i][*classIt], 1,1));

                }else{
                    tmp = std::max(1e-75, nEstimator[i][*classIt].getProbability(ex.getNumericalValue(i)));
                }                                                                                            
        //          cout<<"Tokens = " << i << " val = " << tmp<< " mean = " << means[i][*classIt] << " deviations = " << deviations[i][*classIt] << endl;
        //          cout<<"log(val) = " << my_log_avoid_zero(tmp)<<endl;
        //          cout<<"log(val) = " << log(tmp)<<endl<<endl;
                probCond += log(tmp);
            }

            //categorical tokens evaluation
            for(unsigned int i = 0; i < catTokens.size(); i++){
                string value = catTokens[i];
                double occurrences = tupleValue[i][*classIt][catTokens[i]] + 1.0; //laplacian correction
                double freq = 1.0 * (stats->getSumDFperClass(*classIt) + tupleValue[i][*classIt].size());
//                cout<<"classIt = " << *classIt << " count = " << stats->getSumDFperClass(*classIt) << endl;

                probCond += log(getCategoricalCredibility(i,catTokens[i],*classIt) * (occurrences/freq));
            }

            //text tokens evaluation
            for(unsigned int i = 3 ; i < textTokens.size(); i+=2){
                string termId = textTokens[i];
                int tf = atoi(textTokens[i+1].c_str());
                double numerator = computeConditionalNumerator(termId, *classIt);
                double fraction = numerator / condDenominator[*classIt];
                probCond += (tf * log(fraction)); 
                //probCond += ( log( tf * fraction)); 
            }

            vector<double> graphsCreds(graphsCredibility.size());

            for(unsigned int g = 0 ; g < graphsCredibility.size(); g++){
                graphsCreds[g] = 0.5 + getGraphCredibility(g,id,*classIt);
            }

//            cout<<"P( " << *classIt << " ) = " << aPriori[*classIt] << endl;

            double PofCGivenD = log(aPriori[*classIt]) + probCond;
            for(unsigned int g = 0 ; g < graphsCreds.size(); g++){
                PofCGivenD += log(graphsCreds[g]);
            }

            probMap[*classIt] = PofCGivenD;
//          cout<<" id = " << id << " class = " << *classIt << " prob = " << PofCGivenD <<endl;
            probNormalizer += PofCGivenD;

        }
        double greatestProb = -1e31;
        string predictedLabel;

        for(set<string>::iterator classIt = stats->getClasses().begin(); classIt != stats->getClasses().end(); classIt++){
            probMap[*classIt] = -1.0 * probMap[*classIt]/probNormalizer;
//            cout<<"Class = " << *classIt << " prob = " << -1*probMap[*classIt] << endl;

            if(greaterThan(probMap[*classIt], greatestProb)){
                greatestProb = probMap[*classIt];
                predictedLabel = *classIt;
            }
        }

/*
        for(unsigned int i = 0; i < exs.getNumberOfCategoricalAttibutes(); i++){
            for(map<string, map<string, int> >::iterator it = tupleValue[i].begin(); it!= tupleValue[i].end(); it++){
                for(map<string, int> ::iterator it2 = tupleValue[i][it->first].begin(); it2 != tupleValue[i][it->first].end(); it2++){
                    cout<<" att "<< i << " class= " << it->first << " val = " << it2->first << " p = " << (1.0+it2->second) * 1.0 / (stats->getSumDFperClass(it->first) + tupleValue[i][it->first].size())<<endl;              
                }
            }
            
        }
*/

        computeConfusionMatrix(classId, predictedLabel);

        //        if(io->usingPredictionsFile)
        savePrediction(id, classId, predictedLabel);

        if(predictedLabel == classId){
            classHits[classId] ++;			
        }
        else{
//          cout<<"errei " << id<<endl;
            classMiss[classId]++;
        }

        mappedDocs[predictedLabel]++;
        docsPerClass[classId]++;
    }

    showConfusionMatrix();
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

double NaiveBayes::weibullDistribution(double value, double gamma, int k){
    if(value < 0 ) return 0.0;

    return (k/gamma) * pow((value/gamma),k-1) * pow(exp(-1*value/gamma), k);

}

double NaiveBayes::exponentialDistribution(double value, double gamma){
    return gamma * exp(-1*gamma*value);
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

void NaiveBayes::printFinalOutFile(ostream& out, string msg){
    out<< msg<< "\t"<< microF1<< "\t"<< macroF1<< endl;
}

