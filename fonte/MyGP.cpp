#include "MyGP.h"
#include "Matematica.h"

#include "NaiveBayes.h"
#include "KNN.h"
#include <cstdlib>

int MyGP::gennum = 0;
int MyGP::indCounter = 0;

void MyGP::evaluate(bool printFinalOutfile, std::string msg){

    // Evaluate main tree
    // Iterate through every term/class, computing all
    // [<term, class> -> credibilityScore] mappings. Then, 
    // write it to a credibility file, and call Naive Bayes! 

    //TODO: voltar para onde estava  
    ExampleIterator testItBegin;
    ExampleIterator testItEnd;

    testItBegin = io->getTest().getBegin();
    testItEnd = io->getTest().getEnd();

    map<string, double> credibilities;

    cout << "-----------------  Gen: " << MyGP::gennum  <<" ------------- Ind: "<< ++MyGP::indCounter <<" ----------------"<<endl;

    int termCredibilityOffset = stats->getUsingTermCredibility()? 1 : 0;

    if(stats->getUsingTermCredibility()){
        for(set<string>::iterator vocIt = stats->getVocabulary().begin(); vocIt != stats->getVocabulary().end(); vocIt++) {
            for(set<string>::iterator clIt = (stats->getClasses()).begin(); clIt != (stats->getClasses()).end(); clIt++) {

                double credibility = NthMyGene(0)->evaluate(*this, *vocIt, *clIt);
                string idx = getCompIndex(*vocIt, *clIt);
                credibilities[idx] = credibility;
            }
        }
    }

    vector<map<string, double> > credibilityGraph(stats->getNumberOfGraphs());

    for(ExampleIterator testIt = testItBegin; testIt != testItEnd; testIt++){ 
        for(set<string>::iterator classIt = stats->getClasses().begin(); classIt != stats->getClasses().end(); classIt++){
            for(int g = 0; g < stats->getNumberOfGraphs(); g++){

                Example e = *testIt;

                double credibility = NthMyGene(termCredibilityOffset + g)-> evaluate(*this, e.getId(), *classIt, g);

                string idx = getCompIndex(e.getId(), *classIt);
                credibilityGraph[g][idx] = credibility;
            }
        }
    } 

    //TODO: devo treinar antes de setar o mapa de credibilidades?
    ICredibilityClassifier *classifier;
    if(stats->getUsingKNN())
        classifier = new KNN(stats, stats->getK());
    else
        classifier = new NaiveBayes(stats);
    
    classifier->useContentCredibility(true);
    classifier->setContentCredibilityMap(credibilities);
    classifier->setGraphCredibilityMaps(credibilityGraph);
    
    classifier->train(io->getTrain());
    classifier->test(io->getTest());

    if(printFinalOutfile)
        classifier->printFinalOutFile(io->getFinalOutFile(), msg);

    // Print trees related to solutions generated (for debugging purposes).
    for(int i = 0; i < termCredibilityOffset + stats->getNumberOfGraphs(); i++){
        cout<<"Adfs: "; NthMyGene(i)->printOn(cout); cout<<endl;
    }

    /**
      Print to file 
      fstats.open(statsFileName,ios_base::app);
      for(int i = 0 ; i < numVisions; i++)
      NthMyGene(i)->printOn(fstats);

      fstats <<"\t#\t"<<(1.0-microF1) <<endl;
      fstats.close();
     **/

    stdFitness = (1.0 - classifier->getMicroF1())*100.0;
    delete classifier;
}

double MyGene::evaluate(MyGP &gp, string id, string className = "", int graphId){


    double returnValue = 0.0;

    if (isFunction()) {
        switch (node->value())
        {
            case SUM:
                returnValue = my_sum(NthMyChild(0)->evaluate(gp, id, className) , NthMyChild(1)->evaluate(gp, id, className));
                break;
            
            case SUB:
                returnValue = my_sub(NthMyChild(0)->evaluate(gp, id, className), NthMyChild(1)->evaluate(gp, id, className));
                break;

            case MULT:
                returnValue = my_mult(NthMyChild(0)->evaluate(gp, id, className), NthMyChild(1)->evaluate(gp, id, className));
                break;

            case DIV:
                returnValue = my_div(NthMyChild(0)->evaluate(gp, id, className),NthMyChild(1)->evaluate(gp, id, className));
                break;

            case LOG:  // log de tree0 na base tree1
                returnValue = my_div(my_log(NthMyChild(0)->evaluate(gp, id, className)), my_log(NthMyChild(1)->evaluate(gp, id, className)))  ;
                break;

            case POW:  // pow de tree0 na base tree1
                returnValue = my_pow(NthMyChild(0)->evaluate(gp, id, className), NthMyChild(1)->evaluate(gp, id, className))  ;
                break;

            default:
                printf("Funcao: %c", node->value());
                GPExitSystem ((char*) "MyGene::evaluate", (char*)"Undefined node value (function).");
        }
    }
    if (isTerminal()) {
        switch(node->value()) {

            case DF_PER_TERM:
                returnValue = 1.0 + my_log(stats->getDFperTerm(id));
                break;

            case DF_PER_CLASS:
                returnValue = 1.0 + my_log(stats->getDFperClass(id, className));
                break;

            case TF_PER_TERM:
                returnValue = 1.0 + my_log(stats->getTFperTerm(id));
                break;

            case TF_PER_CLASS: 
                returnValue = 1.0 + my_log(stats->getTFperClass(id, className)); //bao!
                break;

            case AM:
                returnValue = stats->getAM(id, className);
                break;

            case MAX_AM:
                returnValue = stats->getMaxAM(id);
                break;

            case DOMINANCE:
                returnValue = stats->getDominance(id, className);
                break;

            case MAX_DOMINANCE:
                returnValue = stats->getMaxDominance(id);
                break;

            case PROB_COND:
                returnValue = my_div(stats->getTFperClass(id, className) , stats->getTFperTerm(id));
                break;

            case PROB_COND_NEG:
                returnValue = 1.0 - my_div(stats->getTFperClass(id, className) , stats->getTFperTerm(id));
                break;

            case GINI:
                returnValue = stats->getGini(id);
                break;

            case IG:
                returnValue = stats->getIG(id, className);
                break;

            case MAX_IG:
                returnValue = stats->getMaxIG(id);
                break;

            case OR:
                returnValue = stats->getOR(id, className);
                break;

            case MAX_OR: 
                returnValue = stats->getMaxOR(id);
                break;

            case IDF:
                returnValue = stats->getIDF(id);
                break;

            case TFIDF:
                returnValue = stats->getTFIDF(id, className);
                break;

            case MAX_TFIDF: 
                returnValue = stats->getMaxTFIDF(id);
                break;

            case TFICF:
                returnValue = stats->getTFICF(id, className);
                break;

            case MAX_TFICF: 
                returnValue = stats->getMaxTFICF(id);
                break;

            case CTD: 
                returnValue = stats->getCTD(id, className);
                break;

            case MAX_CTD: 
                returnValue = stats->getMaxCTD(id);
                break;

            case GSS:
                returnValue = stats->getGSS(id, className);
                break;

            case MAX_GSS: 
                returnValue = stats->getMaxGSS(id);
                break;

            case CHI: 
                returnValue = stats->getCHI(id, className);
                break;

            case MAX_CHI: 
                returnValue = stats->getMaxCHI(id);
                break;

            case CC:  
                returnValue = stats->getCC(id, className);
                break;

            case MAX_CC: 
                returnValue = stats->getMaxCC(id);
                break;

            case CE:
                returnValue = stats->getCE(id);
                break;

            case NEIGHBORHOOD1:
                returnValue = stats->getNeighborhoodSize1(id,className,graphId);
                break;

            case NEIGHBORHOOD2:
                returnValue = stats->getNeighborhoodSize2(id,className,graphId);
                break;

            case NEIGHBORHOOD3:
                returnValue = stats->getNeighborhoodSize3(id,className,graphId);
                break;

            case HUBSCORE:
                returnValue = stats->getHubScore(id, className, graphId);
                break;

            case AUTHORITY:
                returnValue = stats->getAuthority(id, className, graphId);
                break;

            case EIGENVECTOR:
                returnValue = stats->getEigenVectorCentrality(id, className, graphId);
                break;

            case CLOSENESS:
                returnValue = stats->getCloseness(id, className, graphId);
                break;

            case STRENGTH:
                returnValue = stats->getStrength(id, className, graphId);
                break;

            case CONSTRAINT:
                returnValue = stats->getConstraint(id, className, graphId);	
                break;

            case PAGERANK:
                returnValue = stats->getPageRank(id, className, graphId);
                break;

            case BETWEENNESS:
                returnValue = stats->getBetweenness(id, className, graphId);
                break;

            case BIBCOUPLING:
                returnValue = stats->getBibCoupling(id, className, graphId);
                break;

            case COCITATION:
                returnValue = stats->getCoCitation(id, className, graphId);
                break;

            case JACCARDSIMILARITY:
                returnValue = stats->getJaccardSimilarity(id, className, graphId);
                break;

            case DICESIMILARITY:
                returnValue = stats->getDiceSimilarity(id, className, graphId);
                break;

            case INVERSELOGSIMILARITY:
                returnValue = stats->getInverseLogSimilarity(id, className, graphId);
                break;

            case AVGNEIGHBORHOODDEGREE:
                returnValue = stats->getAvgNearstNeighborDegree(id, className, graphId);
                break;

            default:
                cout << "   Terminal: " << node->value() << endl;
                GPExitSystem ((char*)"MyGene::evaluate",(char*) "Undefined node value (terminal).");
        }
    }
    if(isnan(returnValue)){
        cerr<<"Not a number found! Stop it!"<<endl;
        cerr<<"Valor do nodo = " << node->value()<<endl;
        returnValue=0;
        exit(-1);
    }

    //DEBUG(cout<<"  valor = "<<returnValue <<endl);;
    return returnValue;
}


