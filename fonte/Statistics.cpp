#include "Statistics.h"
#include "Tokenizer.h"

#include <fstream>

#define TAG "Statistics"

using namespace std;

Statistics::Statistics(): usingTermCredibility(false), totalDocs(0), graphNumberCounter(0), sumTF(0), normalEstimator(false), usingNomalizePerGreatestClassValue(false), optimizeGraphMetrics(false), usingKNN(false)
{

}

Statistics::~Statistics(){
    clear();
}

void Statistics::setUsingKNN(bool usingKNN, int K, bool usingKNNOptimize){
    this->usingKNN = usingKNN;
    KNNK = K;
    this->usingKNNOptimize = usingKNNOptimize;
}

bool Statistics::getUsingKNN(){
    return usingKNN;
}
bool Statistics::getUsingKNNOptimize(){
    return usingKNNOptimize;
}

int Statistics::getK(){
    return KNNK;
}

void Statistics::readGraph(string filename){

    ifstream file(filename.c_str());
    string line;

    if(file) {

        cout<<"Getting informations from "<<filename<<endl;
        map<string, Graph > classGraph;

        while(getline(file, line)) {

            vector<string> tokens;
            Tokenizer::stringTokenize(line, tokens, " ");
            string classv1 = "";
            string classv2 = "";
            string id1 = tokens[0];
            string id2 = tokens[1];
            double value = 1.0; 

            bool id1IsInTrain = false, id2IsInTrain = false;
            if( trainIdClass.count(id1) > 0 ){
                id1IsInTrain = true; 
                classv1 = trainIdClass[id1];
            }
            if( trainIdClass.count(id2) > 0 ){
                id2IsInTrain = true; 
                classv2 = trainIdClass[id2];
            }
            if( tokens.size() > 2 ) {
                value = atof( tokens[2].c_str() );
            }

            //TODO: experimental feature
            bool usaInfo = true;
//            if( DFperClass[classv1] < 10 || DFperClass[classv2] < 10) usaInfo = true;
//          cerr<<"lidos : " << id1 << "(" << classv1 << ")  and " << id2 << " ("<< classv2 << ") -> " << value<<endl;
            //We have both ids in the train and both have the same class
            if(usaInfo && id1IsInTrain && id2IsInTrain && classv1 == classv2){
                classGraph[classv1].addEdge(id1,id2,value);
//				cout<<"Inserting... class = "<< classv1 << " ids : " << id1 << " " << id2 << endl;
// 				cout<<"Count edges "<< classGraph[classv1].countEdges()<<endl;
            }
            else if(usaInfo && id1IsInTrain && !id2IsInTrain){
                //Someone (id1) inside our test set points to someone inside train (id2). We do nothing!
                classGraph[classv1].addEdge(id1,id2,value);
            }else if(usaInfo && !id1IsInTrain && id2IsInTrain){
                //Someone (id1) belongs to test and points to a know guy from the train (id2).
                classGraph[classv2].addEdge(id1,id2,value);
            }else{ // doc1Test e doc2Test
                ; //nothing to do, id1 doesnt know id2 neither id2 knows id1.
            }
        }
        file.close();

        map<string, IGraph*> classIGraph;
        for(map<string, Graph>::iterator it = classGraph.begin(); it != classGraph.end(); it++){
            classIGraph[it->first] = new IGraph( &(it->second) );
        }
        iGraphs[graphNumberCounter] = classIGraph;
        graphNumberCounter++;
    }
    else {
        cout << "Error while opening graph file named " << filename<< endl;
        exit(1);
    }
}

void Statistics::retrieveGraphMetrics(Examples& test) {
    
    if(!optimizeGraphMetrics) return;

    idClassNeighborhoodSize1.resize(graphNumberCounter);
    idClassNeighborhoodSize2.resize(graphNumberCounter);
    idClassNeighborhoodSize3.resize(graphNumberCounter);
    idClassHubScore.resize(graphNumberCounter);
    idClassAuthority.resize(graphNumberCounter);
    idClassEigenVectorCentrality.resize(graphNumberCounter);
    idClassCloseness.resize(graphNumberCounter);
    idClassStrength.resize(graphNumberCounter);
    idClassConstraint.resize(graphNumberCounter);
    idClassPageRank.resize(graphNumberCounter);
    idClassBetweenness.resize(graphNumberCounter);
    idClassBibCoupling.resize(graphNumberCounter);
    idClassCoCitation.resize(graphNumberCounter);
    idClassJaccardSimilarity.resize(graphNumberCounter);
    idClassDiceSimilarity.resize(graphNumberCounter);
    idClassInverseLogSimilarity.resize(graphNumberCounter);
    //	idClassAvgNearstNeighborDegree.resize(graphNumberCounter);

    for(map<int, map<string,IGraph*> >::iterator graphIt = iGraphs.begin(); graphIt != iGraphs.end(); graphIt++){
        int totalNumberClasses = graphIt->second.size();
        int counter = 0;
        for(map<string,IGraph*>::iterator classGraphIt = graphIt->second.begin(); classGraphIt != graphIt->second.end(); classGraphIt++, counter++){ 

            cout<<"graphId = " << graphIt->first << " class= "<<classGraphIt->first<< " " << counter<<"/"<<totalNumberClasses<<endl;

            GraphInterface *igraph = classGraphIt->second;

            igraph->calculateHubScore();
            igraph->calculateAuthority();
            igraph->calculateEigenVectorCentrality();

            for(ExampleIterator it = test.getBegin(); it != test.getEnd(); it++){
                Example e = *it;

                //				cout<<"Buscando por "<< e.getId() << " class = "<< classGraphIt->first;
                // [GId] [ExampleId] [ClassId]
                string idClassIdx = getCompIndex(e.getId(), classGraphIt -> first);
                idClassNeighborhoodSize1[graphIt->first][idClassIdx] = igraph->getNeighborhoodSize1(e.getId());
                idClassNeighborhoodSize2[graphIt->first][idClassIdx] = igraph->getNeighborhoodSize2(e.getId());
                idClassNeighborhoodSize3[graphIt->first][idClassIdx] = igraph->getNeighborhoodSize3(e.getId());
                idClassHubScore[graphIt->first][idClassIdx] = igraph->getHubScore(e.getId());
                idClassAuthority[graphIt->first][idClassIdx] = igraph->getAuthority(e.getId());
                idClassEigenVectorCentrality[graphIt->first][idClassIdx] = igraph->getEigenVectorCentrality(e.getId());
                idClassCloseness[graphIt->first][idClassIdx] = igraph->getCloseness(e.getId());
                idClassStrength[graphIt->first][idClassIdx] = igraph->getStrength(e.getId());
                idClassConstraint[graphIt->first][idClassIdx] = igraph->getConstraint(e.getId());
                idClassPageRank[graphIt->first][idClassIdx] = igraph->getPageRank(e.getId());
                idClassBetweenness[graphIt->first][idClassIdx] = igraph->getBetweenness(e.getId());

                idClassBibCoupling[graphIt->first][idClassIdx] = igraph->getBibCoupling(e.getId());
                idClassCoCitation[graphIt->first][idClassIdx] = igraph->getCoCitation(e.getId());
                idClassJaccardSimilarity[graphIt->first][idClassIdx] = igraph->getJaccardSimilarity(e.getId());
                idClassDiceSimilarity[graphIt->first][idClassIdx] = igraph->getDiceSimilarity(e.getId());
                idClassInverseLogSimilarity[graphIt->first][idClassIdx] = igraph->getInverseLogSimilarity(e.getId());
                //Nao descomentar				idClassAvgNearstNeighborDegree[graphIt->first][idClassIdx] = igraph->getAvgNearstNeighborDegree(e.getId());
               /* 
                   cout<<"  NeighborhoodSize1: " << idClassNeighborhoodSize1[graphIt->first][idClassIdx];
                   cout<<"  NeighborhoodSize2: " << idClassNeighborhoodSize1[graphIt->first][idClassIdx];
                   cout<<"  NeighborhoodSize3: " << idClassNeighborhoodSize1[graphIt->first][idClassIdx];
                   cout<<"  Hubscore: " << idClassHubScore[graphIt->first][idClassIdx];
                   cout<<"  Authority: " << idClassAuthority[graphIt->first][idClassIdx];
                   cout<<"  Eigen: " << idClassEigenVectorCentrality[graphIt->first][idClassIdx];
          cout<<"  Close: " << idClassCloseness[graphIt->first][idClassIdx];
                   cout<<"  Strength: " << idClassStrength[graphIt->first][idClassIdx];
                   cout<<"  Const: " << idClassConstraint[graphIt->first][idClassIdx];
                   cout<<"  PageRank: " << idClassPageRank[graphIt->first][idClassIdx];
                   cout<<"  Bet: " << idClassBetweenness[graphIt->first][idClassIdx];
                   cout<<"  BibCou: " <<idClassBibCoupling[graphIt->first][idClassIdx];
                   cout<<"  CoCit: " <<idClassCoCitation[graphIt->first][idClassIdx];
                   cout<<"  JaccSim: " <<idClassJaccardSimilarity[graphIt->first][idClassIdx];
                   cout<<"  DiceSim: " <<idClassDiceSimilarity[graphIt->first][idClassIdx];
                   cout<<"  InvLogSim: " <<idClassInverseLogSimilarity[graphIt->first][idClassIdx]<<endl;
                //				cout<<"  AvnNerNeiDeg: " <<idClassAvgNearstNeighborDegree[graphIt->first][idClassIdx]<<endl;
                */
            }
            //                       cout<<endl;
            delete igraph;
            cout<<"out of class "<<classGraphIt->first<<endl;
        }
    }
    cout<<"out of retrieve graphs metrics..."<<endl;
}

double Statistics::getGraphValue(int metric, int graph, string id, string classId){
    
    double returnValue = 0.0;
    switch(metric){
    
        case NEIGHBORHOOD1: 
            returnValue = (iGraphs[graph][classId])->getNeighborhoodSize1(id);
            break; 
		case NEIGHBORHOOD2: 
            returnValue = (iGraphs[graph][classId])->getNeighborhoodSize2(id);
            break; 
		case NEIGHBORHOOD3:
            returnValue = (iGraphs[graph][classId])->getNeighborhoodSize3(id);
            break; 
		case HUBSCORE:
            returnValue = (iGraphs[graph][classId])->getHubScore(id);
            break; 
		case AUTHORITY:
            returnValue = (iGraphs[graph][classId])->getAuthority(id);
            break; 
		case EIGENVECTOR:
            returnValue = (iGraphs[graph][classId])->getEigenVectorCentrality(id);
            break; 
		case CLOSENESS: 
            returnValue = (iGraphs[graph][classId])->getCloseness(id);
            break; 
        case STRENGTH: 
            returnValue = (iGraphs[graph][classId])->getStrength(id);
            break; 
        case CONSTRAINT: 
            returnValue = (iGraphs[graph][classId])->getConstraint(id);
            break; 
		case PAGERANK: 
            returnValue = (iGraphs[graph][classId])->getPageRank(id);
            break; 
        case BETWEENNESS:
            returnValue = (iGraphs[graph][classId])->getBetweenness(id);
            break; 
        case BIBCOUPLING:
            returnValue = (iGraphs[graph][classId])->getBibCoupling(id);
            break; 
        case COCITATION: 
            returnValue = (iGraphs[graph][classId])->getCoCitation(id);
            break; 
        case JACCARDSIMILARITY:
            returnValue = (iGraphs[graph][classId])->getJaccardSimilarity(id);
            break; 
        case DICESIMILARITY: 
            returnValue = (iGraphs[graph][classId])->getDiceSimilarity(id);
            break; 
        case INVERSELOGSIMILARITY: 
            returnValue = (iGraphs[graph][classId])->getInverseLogSimilarity(id);
            break; 
        case AVGNEIGHBORHOODDEGREE: 
            returnValue = (iGraphs[graph][classId])->getAvgNearstNeighborDegree(id);
            break; 

    }
    return returnValue;
}


// Trainment based on examples: update term frequencies for priori and cond.
void Statistics::readExamples(Examples exs) {

    TRACE_V(TAG,"readExamples");

    for(ExampleIterator it = exs.getBegin(); it != exs.getEnd(); it++){

        Example exp = *it;

        vector<string> textTokens = exp.getTextTokens();

        string id = exp.getId();
        string docClass = exp.getClass();

        totalDocs++;

        classes.insert(docClass);

        trainIdClass[id] = docClass;
        string docIdClassName = getCompIndex(id,docClass);
        sumDFperClass[docClass]++;

        // retrieve each term frequency and update occurrencies
        for (unsigned int i = 3; i < textTokens.size()-1; i+=2) {
            int tf = atoi(textTokens[i+1].c_str());
            string termId = textTokens[i];
            string idxTermClass = getCompIndex(termId, docClass);

            vocabulary.insert(termId);
            sumTFperClass[docClass] += tf;
            DFperTerm[termId]++;
            DFperClass[idxTermClass]++;
            TFperClass[idxTermClass] += tf;
            TFperTerm[termId] += tf;
            sumTF += tf;
            CFperTerm[termId].insert(docClass);
        }
    }
}


void Statistics::calculateIDF() {  

    if(IDF.size() != 0) return;

    for(set<string>::iterator it = vocabulary.begin(); it != vocabulary.end(); it++) {
        double idf = log10(((double) (totalDocs + 1.0)) / ((double) (getValue(DFperTerm, *it) + 1.0 )));
        IDF[*it] = idf;
    }
}


void Statistics::retrieveContentMetrics() {

    TRACE_V(TAG, "retrieveContentMetrics");

    double MinCE = 1e200;
    double MaxCE = -1e200;

    double MinIGForAll = 1e200;
    double MaxIGForAll = -1e200;

    double MinDomForAll = 1e200;
    double MaxDomForAll = -1e200;

    double MinORForAll = 1e200;
    double MaxORForAll = -1e200;

    double MinCTDForAll = 1e200;
    double MaxCTDForAll = -1e200;

    double MinGSSForAll = 1e200;
    double MaxGSSForAll = -1e200;

    double MinCHIForAll = 1e200;
    double MaxCHIForAll = -1e200;

    double MinCCForAll = 1e200;
    double MaxCCForAll = -1e200;

    double MinTFICFForAll = 1e200;
    double MaxTFICFForAll = -1e200;

    double MinTFIDFForAll = 1e200;
    double MaxTFIDFForAll = -1e200;

    double MinAMForAll = 1e200;
    double MaxAMForAll = -1e200;

    for(set<string>::iterator it = vocabulary.begin(); it != vocabulary.end(); it++) {

        double idf = log10(((double) (totalDocs + 1.0)) / ((double) (getValue(DFperTerm, *it) + 1.0 )));
        IDF[*it] = idf;

        //probabilidade para Gini, IG, CE, entre outros
        double Pt = (getValue(TFperTerm, *it) + 1.0) / (sumTF + 1.0);  //versao suavizada

        double giniVal  = 0;
        double sumEntropy = 0;

        double minAM = 1e200;
        double maxAM = -1e200;

        double minTFIDF = 1e200;
        double maxTFIDF = -1e200;

        double minTFICF = 1e200;
        double maxTFICF = -1e200;

        double minCC = 1e200;
        double maxCC = -1e200;

        double minCHI = 1e200;
        double maxCHI = -1e200;

        double minGSS = 1e200;
        double maxGSS = -1e200;

        double minCTD = 1e200;
        double maxCTD = -1e200;

        double minOR = 1e200;
        double maxOR = -1e200;

        double minDOM = 1e200;
        double maxDOM = -1e200;

        double minIG = 1e200;
        double maxIG = -1e200;

        for(set<string>::iterator classIt = classes.begin(); classIt != classes.end(); classIt++) {

            string idx = getCompIndex(*it, *classIt);

            ///calculo do AM
            double amVal = (getValue(TFperClass, idx) + 1.0) / (getValue(TFperTerm, *it) + 1.0); // versao suavisada

            AM[idx] = amVal;
            if ( lesserThan(amVal, minAM))  minAM = amVal;
            if ( greaterThan(amVal, maxAM)) maxAM = amVal;

            //probabilidades de uso geral:
            double Ptec = (getValue(TFperClass, idx) + 1.0) / (sumTF + 1.0);  //suavizada 
            double Pc = (getValue(sumTFperClass, *classIt) + 1.0) / (sumTF + 1.0 );//suavizada
            //double Pc = (getValue(sumDFperClass, *classIt) + 1.0) / (totalDocs + 1.0 );//suavizada
            
            double PdeTtalqueC  = my_div(Ptec,Pc) ;
            //de modo resumido double PdeTtalqueC =  TFperClass[idx] / sumTFperClass[*classIt];
            double PdeCtalqueT = my_div(Ptec,Pt) ;
            // de modo resumido: double PdeCtalqueT =  (double)(TFperClass[idx] * sumTF) / (double)(totalDocs * TFperTerm[*it] );

            ///calculo do coeficiente de GINI
            giniVal += PdeTtalqueC * PdeTtalqueC + PdeCtalqueT * PdeCtalqueT;

            double PdeTeNaoC = ( getValue(TFperTerm, *it) - getValue(TFperClass, idx) + 1.0) / ( 1.0 + sumTF); //suavizada
            double PdeTtalqueNaoC =  (PdeTeNaoC + 1.0) / (1.0-Pc + 1.0) ; //suavizada
            double PdeNaoTtalqueNaoC = 1.0 - PdeTtalqueNaoC;
            double PdeNaoTtalqueC = 1.0 - PdeTtalqueC;

            /* Calculo do IG: Information Gain

               IG = { (P(t\c) log2 P(t\c) / (P(t)*P(c))) + (P(~t\c) log2 (P(~t\c) / (P(~t)*P(c)))) + (P(t\~c) log2 (P(t\~c) / (P(t)*P(~c)))) + (P(~t\~c) log2 (P(~t\~c) / (P(~t)*P(~c)))) }

               log2 A = log(A) / log(2)
               P(t\c) log2 P(t\c) / (P(t)*P(c)) =  (double)(PdeTtalqueC * log(PdeTtalqueC) / log(2)) / (double)(Pc*Pt)
            */

            double Parcela1 = (PdeTtalqueC * (my_log(my_div(PdeTtalqueC,(Pc*Pt))) / log(2.0))) ;
            double Parcela2 = (PdeNaoTtalqueC * (my_log( my_div(PdeNaoTtalqueC,(Pc* (1.0 - Pt)))) / log(2.0)));
            double Parcela3 = (PdeTtalqueNaoC * (my_log(my_div(PdeTtalqueNaoC,((1.0-Pc)*Pt))) / log(2.0))); 
            double Parcela4 = ( PdeNaoTtalqueNaoC* (my_log(my_div( PdeNaoTtalqueNaoC,((1.0-Pc)*(1.0-Pt)))) / log(2.0))) ;

            double igVal = Parcela1 + Parcela2 + Parcela3 + Parcela4; 
            IG[idx] = igVal;
            if( greaterThan(igVal, maxIG) ) maxIG = igVal;
            if( lesserThan(igVal, minIG) ) minIG = igVal;

            /// OR: Odds Ratio  -> (P(t|c) * P(~t|~c) ) / ( P(~t|c) * P(t|~c) )
            double orVal = my_div((double)(PdeTtalqueC * PdeNaoTtalqueNaoC), (double) (PdeNaoTtalqueC * PdeTtalqueNaoC)); 
            OR[idx] = orVal;
            if( lesserThan(orVal,minOR)) minOR = orVal;
            if( greaterThan(orVal,maxOR)) maxOR = orVal;

            //ICF: Inverse class frequency
            double ICF = my_log( (classes.size()+1.0)/(CFperTerm[*it].size()+1.0)); // Suavizada

            /// TFICF 
            double tficfVal = (double)getValue(TFperClass, idx) * ICF;
            TFICF[idx] = tficfVal;
            if( lesserThan(tficfVal, minTFICF) ) minTFICF = tficfVal;
            if( greaterThan(tficfVal, maxTFICF) ) maxTFICF = tficfVal;

            /// TFIDF
            double tfidfVal = (double)getValue(TFperClass, idx) * idf;
            TFIDF[idx] = tfidfVal;
            if( lesserThan(tfidfVal, minTFIDF) ) minTFIDF = tfidfVal;
            if( greaterThan(tfidfVal, maxTFIDF) ) maxTFIDF = tfidfVal;

            ///CTD : Category term Descriptor
            double ctdVal = getValue(TFIDF, idx) * ICF * idf;
            CTD[idx] = ctdVal;
            if( lesserThan(ctdVal , minCTD) ) minCTD = ctdVal;
            if( greaterThan(ctdVal , maxCTD) ) maxCTD = ctdVal;

            double gss = PdeTtalqueC * PdeNaoTtalqueNaoC - PdeTtalqueNaoC * PdeNaoTtalqueC;

            //cout<< " gss = " <<gss<<endl;
            double den = Pt * ( 1.0 - Pt ) * Pc * (1.0 - Pc);

            ///GSS Coefficient
            GSS[idx] = gss;
            if( lesserThan( GSS[idx], minGSS) )  minGSS = GSS[idx];
            if( greaterThan( GSS[idx], maxGSS) ) maxGSS = GSS[idx];

            ///CHI-Square
            double chiVal = (sqrt(totalDocs) * ( gss * gss ) + 1.0) / ( den + 1.0 ); //suavizada
            CHI[idx] = chiVal; 
            if( lesserThan(chiVal, maxCHI)  ) minCHI = chiVal;
            if( greaterThan(chiVal, maxCHI) ) maxCHI = chiVal;

            ///CC: Corelated Coefficient
            double ccVal = (sqrt(totalDocs) * ( gss ) + 1.0) / ( sqrt(den) + 1.0 ); //versao suavizada
            //ccVal = lessOrEqualThanZero(ccVal <= 0) ? -ccVal : ccVal; //avoiding negative values
            CC[idx]  =  ccVal; //avoiding negative values
            if( lesserThan (CC[idx], minCC) ) minCC = CC[idx];
            if( greaterThan(CC[idx], maxCC) ) maxCC = CC[idx];

            ///CE: Cross Entropy
            sumEntropy += (PdeCtalqueT * ( my_log(my_div(PdeCtalqueT,(double)Pc)) /log(2) ) );

            ///Dominance
            double domVal = (getValue(DFperClass, idx) + 1.0) / (getValue(DFperTerm, *it) + 1.0); //versao suavizada.
            DOM[idx] = domVal;
            if( lesserThan(domVal, minDOM) ) minDOM = domVal;
            if( greaterThan(domVal, maxDOM) ) maxDOM = domVal;


            if( lessThanZero(Pc) ){    cerr << "Erro! Pc< 0; Pc = " << Pc<< endl;  exit(1);     }
            if( lessThanZero(PdeCtalqueT) ){cerr << "Erro! PdeCtalqueT< 0; PdeCtalqueT = "<< PdeCtalqueT<< endl; exit(1);     }

            /*
            //DEBUGING:
            //cout<< "" << << endl;
            cout<< "Class = " << *classIt<< endl;
            cout<< "Termo = " << *it << endl;

            cout<< "Pt = " << Pt << endl;
            cout<< "Pc = " << Pc << endl;
            cout<< "PdeTtalqueNaoC = " << PdeTtalqueNaoC << endl;
            cout<< "PdeCtalqueT = " << PdeCtalqueT << endl;
            cout<< "PdeTtalqueC = " << PdeTtalqueC << endl;
            cout<< "AM[idx]  = " <<AM[idx]  << endl;
            cout<< "IG[idx] =" <<IG[idx]  << endl;
            cout<< "OR[idx] =" <<OR[idx] << endl;
            cout<< "TFICF[idx] = " <<TFICF[idx] << endl;
            cout<< "TFIDF[idx] = " << TFIDF[idx] << endl;
            cout<< "CTD[idx] =" <<CTD[idx]  << endl;
            cout<< "GSS[idx] = " << GSS[idx]<< endl;
            cout<< "CHI[idx] = " << CHI[idx] << endl; 
            cout<< "CC[idx] = " << CC[idx]  << endl;

            cout<<"TFperClass[idx] = " << TFperClass[idx] <<endl ;
            cout<<"IDF = " << IDF <<endl;
            cout<<"totalDocs = " << totalDocs <<endl;
            cout<<"DFperTerm[*it] = " << DFperTerm[*it] <<endl;
            cout<<"*****************************************" <<endl;
            */   

        }

        MinIGForAll = lesserThan(minIG , MinIGForAll) ? minIG : MinIGForAll; 
        MaxIGForAll = greaterThan(maxIG , MaxIGForAll) ? maxIG : MaxIGForAll; 
        MaxIG[*it] = maxIG;

        MinDomForAll = lesserThan(minDOM, MinDomForAll) ? minDOM : MinDomForAll;
        MaxDomForAll = greaterThan(maxDOM, MaxDomForAll) ? maxDOM : MaxDomForAll;
        MaxDom[*it] = maxDOM;

        MinORForAll = lesserThan(minOR, MinORForAll) ? minOR : MinORForAll;
        MaxORForAll = greaterThan(maxOR, MaxORForAll) ? maxOR : MaxORForAll;
        MaxOR[*it] = maxOR;

        MinCTDForAll = lesserThan(minCTD, MinCTDForAll) ? minCTD : MinCTDForAll;
        MaxCTDForAll = greaterThan(maxCTD, MaxCTDForAll) ? maxCTD : MaxCTDForAll;
        MaxCTD[*it] = maxCTD;

        MinGSSForAll = lesserThan(minGSS, MinGSSForAll) ? minGSS : MinGSSForAll;
        MaxGSSForAll = greaterThan(maxGSS, MaxGSSForAll) ? maxGSS : MaxGSSForAll;
        MaxGSS[*it] = maxGSS;

        MinCHIForAll = lesserThan(minCHI, MinCHIForAll) ? minCHI : MinCHIForAll;
        MaxCHIForAll = greaterThan(maxCHI, MaxCHIForAll) ? maxCHI : MaxCHIForAll;
        MaxCHI[*it] = maxCHI;

        MinCCForAll = lesserThan(minCC, MinCCForAll) ? minCC : MinCCForAll;
        MaxCCForAll = greaterThan(maxCC, MaxCCForAll) ? maxCC : MaxCCForAll;
        MaxCC[*it] = maxCC;

        MinTFICFForAll = lesserThan(minTFICF, MinTFICFForAll) ? minTFICF : MinTFICFForAll;
        MaxTFICFForAll = greaterThan(maxTFICF, MaxTFICFForAll) ? maxTFICF : MaxTFICFForAll;
        MaxTFICF[*it] = maxTFICF;

        MinTFIDFForAll = lesserThan(minTFIDF, MinTFIDFForAll) ? minTFIDF : MinTFIDFForAll;
        MaxTFIDFForAll = greaterThan(maxTFIDF, MaxTFIDFForAll) ? maxTFIDF : MaxTFIDFForAll;
        MaxTFIDF[*it] = maxTFIDF;

        MinAMForAll = lesserThan(minAM, MinAMForAll) ? minAM : MinAMForAll;
        MaxAMForAll = greaterThan(maxAM, MaxAMForAll) ? maxAM : MaxAMForAll;
        MaxAM[*it] = maxAM;

        //Normalize values
        for(set<string>::iterator classIt = classes.begin(); classIt != classes.end(); classIt++) {
            string idx = getCompIndex(*it, *classIt);

            if(usingNomalizePerGreatestClassValue){
                IG[idx]      = maxIG;
                DOM[idx]     = maxDOM;
                OR[idx]      = maxOR;
                CTD[idx]     = maxCTD;
                GSS[idx]     = maxGSS;
                CHI[idx]     = maxCHI;
                CC[idx]      = maxCC;
                TFICF[idx]   = maxTFICF;
                TFIDF[idx]   = maxTFIDF;
                AM[idx]      = maxAM;

            }
            else{
                IG[idx]      = maxNormalization(IG[idx] , maxIG); 
 //               DOM[idx]     = minmaxNormalization(DOM[idx], minDOM, maxDOM); 
                OR[idx]      = maxNormalization(OR[idx] , maxOR); 
                CTD[idx]     = minmaxNormalization(CTD[idx], minCTD, maxCTD); 
                GSS[idx]     = maxNormalization(GSS[idx], maxGSS); 
                CHI[idx]     = maxNormalization(CHI[idx], maxCHI); 
                CC[idx]      = maxNormalization(CC[idx], maxCC); 
                TFICF[idx]   = minmaxNormalization(TFICF[idx], minTFICF, maxTFICF); 
                TFIDF[idx]   = minmaxNormalization(TFIDF[idx], minTFIDF, maxTFIDF); 
 //               AM[idx]      = minmaxNormalization(AM[idx], minAM, maxAM); 

            }
        }

        double ceVal = Pt * sumEntropy;
        CE[*it] = ceVal;
        if(greaterThan(ceVal,MaxCE)) MaxCE = ceVal;
        if(lesserThan(ceVal,MinCE)) MinCE = ceVal;

        Gini[*it] = giniVal;
    }

    //Normalize max values...
    for(set<string>::iterator it = vocabulary.begin(); it != vocabulary.end(); it++) {

        if(usingNomalizePerGreatestClassValue){
            CE[*it] = MaxCE;
            MaxIG[*it] =  MaxIGForAll;
            MaxDom[*it] = MaxDomForAll;
            MaxOR[*it] =  MaxORForAll;
            MaxCTD[*it] = MaxCTDForAll;
            MaxGSS[*it] = MaxGSSForAll;
            MaxCHI[*it] = MaxCHIForAll;
            MaxCC[*it] =  MaxCCForAll;
            MaxTFICF[*it] = MaxTFICFForAll;
            MaxTFIDF[*it] = MaxTFIDFForAll;
            MaxAM[*it] = MaxAMForAll;

        }
        else{
/*            CE[*it] = minmaxNormalization(CE[*it], MinCE, MaxCE);
            MaxIG[*it] = minmaxNormalization(MaxIG[*it], MinIGForAll, MaxIGForAll);
            MaxDom[*it] = minmaxNormalization(MaxDom[*it], MinDomForAll, MaxDomForAll);
            MaxOR[*it] = minmaxNormalization(MaxOR[*it], MinORForAll, MaxORForAll);
            MaxCTD[*it] = minmaxNormalization(MaxCTD[*it], MinCTDForAll, MaxCTDForAll);
            MaxGSS[*it] = minmaxNormalization(MaxGSS[*it], MinGSSForAll, MaxGSSForAll);
            MaxCHI[*it] = minmaxNormalization(MaxCHI[*it], MinCHIForAll, MaxCHIForAll);
            MaxCC[*it] = minmaxNormalization(MaxCC[*it], MinCCForAll, MaxCCForAll);
            MaxTFICF[*it] = minmaxNormalization(MaxTFICF[*it], MinTFICFForAll, MaxTFICFForAll);
            MaxTFIDF[*it] = minmaxNormalization(MaxTFIDF[*it], MinTFIDFForAll, MaxTFIDFForAll);
            MaxAM[*it] = minmaxNormalization(MaxAM[*it], MinAMForAll, MaxAMForAll);
*/
        }
    }
}

double Statistics::minmaxNormalization(double val, double minValue, double maxValue){
    
    if(isZero(maxValue - minValue)){
        return 0.0;
    }
    return (val - minValue) / (maxValue - minValue);
    
}

double Statistics::maxNormalization(double val, double maxValue){
    return val/maxValue;
}

void Statistics::setOptimizeGraphMetrics(bool optimize){
    optimizeGraphMetrics = optimize;
}

//TODO: delete it!
void Statistics::retrieveGraphMetricsDebug(Examples& test) {

    if(!optimizeGraphMetrics) return;
    
    idClassNeighborhoodSize1.resize(graphNumberCounter);
    idClassNeighborhoodSize2.resize(graphNumberCounter);
    idClassNeighborhoodSize3.resize(graphNumberCounter);
    idClassHubScore.resize(graphNumberCounter);
    idClassAuthority.resize(graphNumberCounter);
    idClassEigenVectorCentrality.resize(graphNumberCounter);
    idClassCloseness.resize(graphNumberCounter);
    idClassStrength.resize(graphNumberCounter);
    idClassConstraint.resize(graphNumberCounter);
    idClassPageRank.resize(graphNumberCounter);
    idClassBetweenness.resize(graphNumberCounter);
    idClassBibCoupling.resize(graphNumberCounter);
    idClassCoCitation.resize(graphNumberCounter);
    idClassJaccardSimilarity.resize(graphNumberCounter);
    idClassDiceSimilarity.resize(graphNumberCounter);
    idClassInverseLogSimilarity.resize(graphNumberCounter);
    //	idClassAvgNearstNeighborDegree.resize(graphNumberCounter);

    for(ExampleIterator it = test.getBegin(); it != test.getEnd(); it++){
        for(map<int, map<string,IGraph*> >::iterator graphIt = iGraphs.begin(); graphIt != iGraphs.end(); graphIt++){
            for(map<string,IGraph*>::iterator	classGraphIt = graphIt->second.begin(); classGraphIt != graphIt->second.end(); classGraphIt++){ 

                GraphInterface *igraph = classGraphIt->second;

                igraph->calculateHubScore();
                igraph->calculateAuthority();
                igraph->calculateEigenVectorCentrality();

                Example e = *it;

                cout<<"Buscando por "<< e.getId() << " class = "<< classGraphIt->first;

                // [GId] [ExampleId] [ClassId]
                string idClassIdx = getCompIndex(e.getId(), classGraphIt -> first);
                idClassNeighborhoodSize1[graphIt->first][idClassIdx] = igraph->getNeighborhoodSize1(e.getId());
                idClassNeighborhoodSize2[graphIt->first][idClassIdx] = igraph->getNeighborhoodSize2(e.getId());
                idClassNeighborhoodSize3[graphIt->first][idClassIdx] = igraph->getNeighborhoodSize3(e.getId());
                idClassHubScore[graphIt->first][idClassIdx] = igraph->getHubScore(e.getId());
                idClassAuthority[graphIt->first][idClassIdx] = igraph->getAuthority(e.getId());
                idClassEigenVectorCentrality[graphIt->first][idClassIdx] = igraph->getEigenVectorCentrality(e.getId());
                idClassCloseness[graphIt->first][idClassIdx] = igraph->getCloseness(e.getId());
                idClassStrength[graphIt->first][idClassIdx] = igraph->getStrength(e.getId());
                idClassConstraint[graphIt->first][idClassIdx] = igraph->getConstraint(e.getId());
                idClassPageRank[graphIt->first][idClassIdx] = igraph->getPageRank(e.getId());
                idClassBetweenness[graphIt->first][idClassIdx] = igraph->getBetweenness(e.getId());
                idClassBibCoupling[graphIt->first][idClassIdx] = igraph->getBibCoupling(e.getId());
                idClassCoCitation[graphIt->first][idClassIdx] = igraph->getCoCitation(e.getId());
                idClassJaccardSimilarity[graphIt->first][idClassIdx] = igraph->getJaccardSimilarity(e.getId());
                idClassDiceSimilarity[graphIt->first][idClassIdx] = igraph->getDiceSimilarity(e.getId());
                idClassInverseLogSimilarity[graphIt->first][idClassIdx] = igraph->getInverseLogSimilarity(e.getId());
                //				idClassAvgNearstNeighborDegree[graphIt->first][idClassIdx] = igraph->getAvgNearstNeighborDegree(e.getId());

                cout<<"  NeighborhoodSize1: " << idClassNeighborhoodSize1[graphIt->first][idClassIdx];
                cout<<"  NeighborhoodSize2: " << idClassNeighborhoodSize1[graphIt->first][idClassIdx];
                cout<<"  NeighborhoodSize3: " << idClassNeighborhoodSize1[graphIt->first][idClassIdx];
                cout<<"  Hubscore: " << idClassHubScore[graphIt->first][idClassIdx];
                cout<<"  Authority: " << idClassAuthority[graphIt->first][idClassIdx];
                cout<<"  Eigen: " << idClassEigenVectorCentrality[graphIt->first][idClassIdx];
                cout<<"  Close: " << idClassCloseness[graphIt->first][idClassIdx];
                cout<<"  Strength: " << idClassStrength[graphIt->first][idClassIdx];
                cout<<"  Const: " << idClassConstraint[graphIt->first][idClassIdx];
                cout<<"  PageRank: " << idClassPageRank[graphIt->first][idClassIdx];
                cout<<"  Bet: " << idClassBetweenness[graphIt->first][idClassIdx];
                cout<<"  BibCou: " <<idClassBibCoupling[graphIt->first][idClassIdx];
                cout<<"  CoCit: " <<idClassCoCitation[graphIt->first][idClassIdx];
                cout<<"  JaccSim: " <<idClassJaccardSimilarity[graphIt->first][idClassIdx];
                cout<<"  DiceSim: " <<idClassDiceSimilarity[graphIt->first][idClassIdx];
                cout<<"  InvLogSim: " <<idClassInverseLogSimilarity[graphIt->first][idClassIdx]<<endl;
                //				cout<<"  AvnNerNeiDeg: " <<idClassAvgNearstNeighborDegree[graphIt->first][idClassIdx]<<endl;

                delete igraph;
            }
            cout<<endl;
        }
    }
}

void Statistics::clear(){
//void Statistics::Statistics(): usingTermCredibility(false), totalDocs(0), graphNumberCounter(0), sumTF(0), normalEstimator(false)

    //terms:
    trainIdClass.clear();
    vocabulary.clear();
    classes.clear();
    sumDFperClass.clear();
    sumTFperClass.clear();
    DFperTerm.clear(); 
    DFperClass.clear(); 

    TFperClass.clear(); 
    TFperTerm.clear(); 

    IDF.clear();

    TFIDF.clear();
    MaxTFIDF.clear();

    MaxTFICF.clear();
    TFICF.clear();

    AM.clear();
    MaxAM.clear();

    DOM.clear();
    MaxDom.clear();

    IG.clear();
    MaxIG.clear(); 

    Gini.clear();

    OR.clear();
    MaxOR.clear(); 

    CE.clear();
    CFperTerm.clear();

    CTD.clear();
    MaxCTD.clear(); 

    GSS.clear();
    MaxGSS.clear();

    CHI.clear();
    MaxCHI.clear();

    CC.clear();
    MaxCC.clear(); 

    //Graphs:
    for(map<int, map<string, IGraph*> >::iterator graphIt = iGraphs.begin(); graphIt != iGraphs.end(); graphIt++){
        if(optimizeGraphMetrics){
            idClassNeighborhoodSize1[graphIt->first].clear();
            idClassNeighborhoodSize2[graphIt->first].clear();
            idClassNeighborhoodSize3[graphIt->first].clear();
            idClassHubScore[graphIt->first].clear();
            idClassAuthority[graphIt->first].clear();
            idClassEigenVectorCentrality[graphIt->first].clear();
            idClassCloseness[graphIt->first].clear();
            idClassStrength[graphIt->first].clear();
            idClassConstraint[graphIt->first].clear();
            idClassPageRank[graphIt->first].clear();
            idClassBetweenness[graphIt->first].clear();
            idClassBibCoupling[graphIt->first].clear();
            idClassCoCitation[graphIt->first].clear();
            idClassJaccardSimilarity[graphIt->first].clear();
            idClassDiceSimilarity[graphIt->first].clear();
            idClassInverseLogSimilarity[graphIt->first].clear();
            //idClassAvgNearstNeighborDegree[graphIt->first].clear();
        }
        else{
            for(map<string, IGraph*>::iterator it = iGraphs[graphIt->first].begin() ; it != iGraphs[graphIt->first].end(); it++){
                delete it->second;
            }
        }
        iGraphs[graphIt->first].clear();
    }
    idClassNeighborhoodSize1.clear();
    idClassNeighborhoodSize2.clear();
    idClassNeighborhoodSize3.clear();
    idClassHubScore.clear();
    idClassAuthority.clear();
    idClassEigenVectorCentrality.clear();
    idClassCloseness.clear();
    idClassStrength.clear();
    idClassConstraint.clear();
    idClassPageRank.clear();
    idClassBetweenness.clear();
    idClassBibCoupling.clear();
    idClassCoCitation.clear();
    idClassJaccardSimilarity.clear();
    idClassDiceSimilarity.clear();
    idClassInverseLogSimilarity.clear();
    idClassAvgNearstNeighborDegree.clear();
    
    graphNumberCounter = 0;
    sumTF = 0;
    totalDocs = 0;

    //TODO: terminar isso aqui!
}


