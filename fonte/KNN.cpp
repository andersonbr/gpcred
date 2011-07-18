#include "KNN.h"
#include "Matematica.h"

#include <limits>

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

double KNN::getCategoricalCredibility(int i, string token, string classId){

	if(!usingContentCredibility) return 1.0; 
	if(contentCredibility.size() == 0) return 1.0;
	
	string idxa = getCompIndex(i,token);
	string idx = getCompIndex(idxa,classId);

    if(contentCredibility.find(idx) != contentCredibility.end()) return contentCredibility[idx] * Normalizer;

	return 1.0;
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
 
    for(int i = 0; i < exs.getNumberOfNumericalAttibutes(); i++){
        maxv[i] = numeric_limits<double>::min();
        minv[i] = numeric_limits<double>::max();
    }
    
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
        
        vector<double> numTokens = (e)->getNumericalTokens();
       
        for(unsigned int i = 0; i < numTokens.size(); i++){
            if(greaterThan(numTokens[i], maxv[i])){
                maxv[i] = numTokens[i];
            }
            if(lesserThan(numTokens[i], minv[i])){
                minv[i] = numTokens[i];
            }
        }

        exNumTrain[eId] = numTokens;
        exCatTrain[eId] = (e)->getCategoricalTokens();
        
        docTrainSizes[eId] = docSize;
    }

}

double KNN::minMaxNorm(double value, int index){
//    cout<<"maxv["<<index<<"] = " << maxv[index] << "  min = " << minv[index] <<endl;
    return (value - minv[index] ) / (maxv[index] - minv[index]);
}

string KNN::getPredictedClass(set<docWeighted, docWeightedCmp>& trainExamples){
    
    map<string, double> votes;
    map<string, double> sum;

    int counter = 0;
    for(set<docWeighted>::iterator ex = trainExamples.begin(); ex != trainExamples.end() && counter < K; ex++, counter++){
        string classId = stats->getTrainClass((ex)->docId);
        votes[classId] += 1;//ex->weight;
        sum[classId] += ex->weight;

//        cout<<" k = " << counter << " docID = " << (ex)->docId<< " classe = " <<  classId << "\tsim = " << ex->weight <<endl;
    }
    
    string predictedClass = "";
    double max = 0;
    for(map<string, double>::iterator it = votes.begin(); it != votes.end(); it++){
        if(greaterThan(it->second, max)){
            max = it->second;
            predictedClass = it->first;
        }
        else if(equals(it->second, max) && max > 0.0){

            if(lesserThan(sum[it->first] , sum[predictedClass])){
                max = it->second;
                predictedClass = it->first;
            }
        }
//        cout<<"classe = " << it->first << "\tsim = " << it->second<< " sum = " << sum[it->first] << endl;
    }
//    cout<<"predicter = " << predictedClass << "  -> " << max<<endl;
//    cout<<"====================================="<<endl;
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
        vector<string> catTokens = ex.getCategoricalTokens();

        string eId = ex.getId();
        string classId = ex.getClass();
        
        map<string, double> examplesTestSize;
        //credibility to each class
        if((usingKNNOptimize && !valuesSaved )  || !usingKNNOptimize){
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
            for(unsigned int i = 3; i < textTokens.size();i+=2){
                string termId = textTokens[i];
                int tf = atoi(textTokens[i+1].c_str());

                for(set<docWeighted, docWeightedCmp>::iterator termIt = termDocWset[termId].begin(); termIt != termDocWset[termId].end(); termIt++){
                    string trainClass = stats-> getTrainClass(termIt->docId);

                    double trainDocSize = docTrainSizes[termIt->docId];
                    double trainTermWeight = termIt->weight;
                    double testTermWeight = tf * getContentCredibility(termId, trainClass);
                    
                    similarity[termIt->docId] +=  ( - ( trainTermWeight / sqrt(trainDocSize)  * testTermWeight / sqrt(examplesTestSize[trainClass]) ));
//                    cout<<"sim = " << similarity[termIt->docId] <<endl;
                }
            }

            //numerical KNN
            for(map<string, vector<double> >::iterator trainIt  = exNumTrain.begin(); trainIt != exNumTrain.end(); trainIt++){
                double dist = 0.0;

                for(unsigned int i = 0; i < numTokens.size(); i++){
                    double a = minMaxNorm(numTokens[i],i);
                    double b = minMaxNorm(exNumTrain[trainIt->first][i],i);
                    double val = (a-b)*(a-b);
                    //double val = (numTokens[i] - exNumTrain[trainIt->first][i]) * ( numTokens[i] - exNumTrain[trainIt->first][i]);
                    //                    cout<<numTokens[i] << " - " <<exNumTrain[trainIt->first][i] <<endl;
                    //                    cout<<"a = " << a << " b = " << b << " val =" << val<<endl;
                    if( greaterThan(dist + val, numeric_limits<double>::max())){
                        //                        cerr<<"OOOOOOOOOOOOOOOPA!!!"<<endl;
                        //                        exit(0);
                        dist = numeric_limits<double>::max() - 1.0;
                        break;
                    }
                    dist += val;
                    //                    cout<<"dist =" << dist<<endl;
                }
                similarity[trainIt->first] += dist;
            }

            //categorical KNN
            for(map<string, vector<string> >::iterator trainIt  = exCatTrain.begin(); trainIt != exCatTrain.end(); trainIt++){
                double dist = 0.0;

                for(unsigned int i = 0; i < catTokens.size(); i++){
                    string trainTok = exCatTrain[trainIt->first][i];
                    string testTok = catTokens[i];

                    double catCred = getCategoricalCredibility(i, testTok, stats->getTrainClass(trainIt->first));
//                    cout<<"catCred = " <<catCred<<endl;
//                    cout<<" i = " << i << "teste = " << testTok<<" treino = " << trainTok<<endl;
                    if(trainTok != testTok){
//                        dist+= 1.0/(catCred+ 1.0) + 1.0;
                        dist+= 1.0/(catCred+ 1.0);
//                        cout<<"dist = " << dist<<endl;
                    }
                }
                similarity[trainIt->first] += dist;
            }
 //               cout<<"class = " << classId << " doc = " << trainIt->first<< " docClass = " << stats->getTrainClass(trainIt->first) << " dist="<<dist<< " 1/dist = " <<1.0/dist<< " sqrt = "<<sqrt(dist)<<endl;
        }

        if(!valuesSaved && usingKNNOptimize){
            saveValues[eId] = similarity;
        }

        //sim of each example in test set
        set<docWeighted, docWeightedCmp> sim;
        for(map<string, double>::iterator testIt = similarity.begin(); testIt != similarity.end(); testIt++){
            
            //calculating graph credibility....if so
            vector<double> graphsCreds(graphsCredibility.size());
            double similarityValue = testIt->second;

//            cout<< " eid = " << eId << " eclass = " << classId << " traindocclass = " << stats->getTrainClass(testIt->first) << " similarit = " << similarityValue<< endl;

            for(unsigned int g = 0 ; g < graphsCredibility.size(); g++){
                double gsim = getGraphCredibility(g, eId, stats->getTrainClass(testIt->first));
                similarityValue /= (0.5+gsim);
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
    }
    calculateF1(classHits,classMiss,docsPerClass, mappedDocs);
//    showConfusionMatrix();
}

void KNN::calculateF1(map<string, unsigned long long> hits, map<string, unsigned long long> misses, map<string, unsigned long long> docsPerClass, map<string, unsigned long long> mappedDocs){

	macroF1 = 0.0;
	microF1 = 0.0;

	unsigned long totalHits = 0;
	unsigned long totalDocs = 0;
    set<string> classUsed;

	for(set<string>::iterator classIt = stats->getClasses().begin(); classIt != stats->getClasses().end(); classIt++) {

        double precision = 0.0;
        double recall = 0.0;

        if( mappedDocs[*classIt] > 0) {
            precision = ((double) hits[*classIt] / (double) mappedDocs[*classIt]);
            classUsed.insert(*classIt);
        }
        if( docsPerClass[*classIt] > 0){ 
		    recall = ((double) hits[*classIt] / (double)(docsPerClass[*classIt]));
            classUsed.insert(*classIt);
        }
		totalHits += hits[*classIt];
		totalDocs += docsPerClass[*classIt];

		double F1 = (precision+recall == 0) ? 0.0 : (2.0 * precision * recall) / (precision + recall);
		macroF1 += F1;
	}

	macroF1 /= classUsed.size();
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
    int total = 0;

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
        total += totalCollums[*class1];
    }
    cout<<total<<endl;
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

void KNN::cleanStaticFields(){
    for(map<string, map<string, double> >::iterator it = saveValues.begin(); it != saveValues.end(); it++){
        saveValues[it->first].clear();
    }
    saveValues.clear();

    valuesSaved = false;
}

