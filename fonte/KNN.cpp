#include "KNN.h"
#include "Matematica.h"

#define TAG "KNN"

KNN::KNN(Statistics* st, int K, bool boolOpt): usingContentCredibility(false), usingGraphCredibility(false), usingKNNOptimize(boolOpt), stats(st), Normalizer(10.0), microF1(0), macroF1(0), K(K) 
{
    usingNormalEstimator = stats->getNormalEstimator();
}

bool KNN::valuesSaved = false;
map<string, map< string, double> > KNN::saveValues;

KNN::~KNN(){
}

double KNN::getContentCredibility(string term, string docClass){
 
	if(!usingContentCredibility) return stats->getIDF(term);
	if(contentCredibility.size() == 0) return stats->getIDF(term);
	string idx = getCompIndex(term,docClass);

	if(contentCredibility.find(idx) != contentCredibility.end()) return contentCredibility[idx] * Normalizer;
	return stats->getIDF(term);
}

double KNN::getGraphCredibility(int g, string id, string classId){
	
    if(!usingGraphCredibility) return 1.0;
	if(graphsCredibility.size() == 0) return 1.0;
	string idx = getCompIndex(id,classId);

	if( graphsCredibility[g].count(idx) > 0) return graphsCredibility[g][idx] * Normalizer;
	return 1.0;
}

void KNN::setGraphCredibilityMaps(vector< map<string, double> >& gCred){
	graphsCredibility = gCred;
    usingGraphCredibility = true;
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
    
    //Maybe we didnt calculate this before...
    stats->calculateIDF();
    
	for(ExampleIterator e = exs.getBegin(); e != exs.getEnd(); e++){
   
        vector<string> textTokens = (e)->getTextTokens();
		string exampleClass = (e)->getClass();
        string eId = (e)->getId();
        double docSize = 0.0;

//      cout<<" Tokens categoricos  =  " << tokens.size() << endl;
		for(unsigned int i = 3; i < textTokens.size()-1; i+=2){
			int tf = atoi(textTokens[i+1].c_str());
			string termId = textTokens[i];
            
            double tfidf = tf * stats->getIDF(termId);

            docSize += (tfidf * tfidf);

            docWeighted dw(eId, tfidf);
            termDocWset[termId].insert(dw);
		}

        vector<double> numTokens = (e)-> getNumericalTokens();
        exTrain[eId] = numTokens;
        
        docTrainSizes[eId] = docSize;
    }

}

string KNN::getPredictedClass(set<docWeighted, docWeightedCmp>& trainExamples){
    
    map<string, double> votes;

    int counter = 0;
    for(set<docWeighted>::iterator ex = trainExamples.begin(); ex != trainExamples.end() && counter < K; ex++, counter++){
        string classId = stats->getTrainClass((ex)->docId);
        votes[classId] += ex->weight;
    }
    
    string predictedClass;
    double max = 0.0;
    for(map<string, double>::iterator it = votes.begin(); it != votes.end(); it++){
        if(greaterThan(it->second, max)){
            max = it->second;
            predictedClass = it->first;
        }
    }

    return predictedClass;
}

void KNN::test(Examples& exs){

    TRACE_V(TAG,"test");

    //Statistics:
    map<string,unsigned long long> classHits;
    map<string,unsigned long long> classMiss;
    map<string,unsigned long long> mappedDocs;
    map<string,unsigned long long> docsPerClass;
    
    int numExamples = 0;
    for(ExampleIterator it = exs.getBegin(); it != exs.getEnd(); it++){
        numExamples++;
        if(numExamples % 100 == 0)
            cout<<"Evaluated: " << numExamples<<endl;

        Example ex = *it;


        vector<string> textTokens = ex.getTextTokens();	
        vector<double> numTokens = ex.getNumericalTokens();

        string eId = ex.getId();
        string classId = ex.getClass();
        
        map<string, double> examplesTestSize;
//      cout<<"vs = "<<valuesSaved<<endl;
        //credibility to each class
        if((usingKNNOptimize && !valuesSaved )  || !usingKNNOptimize){
//          cout<<"AAAAAAA calculando..."<<endl;
            for(unsigned int i = 3; i < textTokens.size(); i+=2){
                string termId = textTokens[i];
                int tf = atoi(textTokens[i+1].c_str());

                for(set<string>::iterator classIt = stats->getClasses().begin(); classIt != stats->getClasses().end(); classIt++) {
                    double tfidf = tf * getContentCredibility(termId, *classIt);
                    examplesTestSize[*classIt] += (tfidf * tfidf);
                }
            }
        }
        map<string, double> similarity;

        if(usingKNNOptimize && valuesSaved){
            similarity = saveValues[eId];
        }
        else{
//          cout<<"BBBBB calculando..."<<endl;
            for(unsigned int i = 3; i < textTokens.size();i+=2){
                string termId = textTokens[i];
                int tf = atoi(textTokens[i+1].c_str());

                for(set<docWeighted, docWeightedCmp>::iterator termIt = termDocWset[termId].begin(); termIt != termDocWset[termId].end(); termIt++){
                    string trainClass = stats-> getTrainClass(termIt->docId);

                    double trainDocSize = docTrainSizes[termIt->docId];
                    double trainTermWeight = termIt->weight;
                    double testTermWeight = tf * getContentCredibility(termId, trainClass);

                    similarity[termIt->docId] += ( trainTermWeight / sqrt(trainDocSize)  * testTermWeight / sqrt(examplesTestSize[trainClass]) );
                }
            }

            for(map<string, vector<double> >::iterator trainIt  = exTrain.begin(); trainIt != exTrain.end(); trainIt++){
                double sim = 0.0;
                for(unsigned int i = 0; i < numTokens.size(); i++){
                    sim += ((numTokens[i] - exTrain[trainIt->first][i]) * ( numTokens[i] - exTrain[trainIt->first][i]));
                }
                similarity[trainIt->first] += 1.0/sim;
            }
        }
        
        if(!valuesSaved)
            saveValues[eId] = similarity;

        //sim of each example in test set
        set<docWeighted, docWeightedCmp> sim;
        for(map<string, double>::iterator testIt = similarity.begin(); testIt != similarity.end(); testIt++){
            
            //calculating graph credibility....if so
            vector<double> graphsCreds(graphsCredibility.size());
            double similarityValue = testIt->second;

            for(unsigned int g = 0 ; g < graphsCredibility.size(); g++){
                double gsim = getGraphCredibility(g, eId, stats->getTrainClass(testIt->first));
//                cout<<gsim<< " eid = " << eId << " eclass = " << classId << " traindocclass = " << stats->getTrainClass(trainIt->first) << " similarit = " << similarity << " final = " << similarity * gsim << endl;
                similarityValue *= (0.5+gsim);
            } 
           
            //never change this, it is necessary
            docWeighted dw(testIt->first, similarityValue);
            sim.insert(dw);
        }
        
        string predictedLabel = getPredictedClass(sim);

        computeConfusionMatrix(classId, predictedLabel);

        //        if(io->usingPredictionsFile)
        savePrediction(eId, classId, predictedLabel);

        if(predictedLabel == classId){
            classHits[classId] ++;			
        }
        else{
            classMiss[classId]++;
        }

        mappedDocs[predictedLabel]++;
        docsPerClass[classId]++;
    }
    if(valuesSaved == false){
        valuesSaved = true;
        cout<<"changin valuesSaved para verdadeiro!! " << valuesSaved<<endl;
    }
    calculateF1(classHits,classMiss,docsPerClass, mappedDocs);
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


