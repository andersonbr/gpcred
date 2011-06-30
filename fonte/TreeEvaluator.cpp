#include "TreeEvaluator.h"

#include "Tokenizer.h"
#include "Matematica.h"
#include "NaiveBayes.h"
#include "KNN.h"

#include<cstdarg> 
#include<map>

TreeEvaluator::TreeEvaluator(Statistics* stats, InOut* io) : stats(stats),io(io){
}

TreeEvaluator::~TreeEvaluator(){
}

void TreeEvaluator::evaluateFromFile(string fileName){

    //baseline test
    ifstream file(fileName.c_str(), ifstream::in);

    ICredibilityClassifier *classifier;
    if(stats->getUsingKNN())
        classifier = new KNN(stats);
    else
        classifier = new NaiveBayes(stats);

    classifier->useContentCredibility(true);

    classifier->train(io->getTrain());
    classifier->test(io->getTest());
    
    ostream &out = io->getEvaluateFile();
    out.setf(ios::fixed,ios::floatfield);
    out.precision(5);
    out<<"Baseline"<<"\t"<< classifier->getMicroF1() << "\t" << classifier->getMacroF1()<<endl;
    double blmicro = classifier->getMicroF1();
    double blmacro = classifier->getMacroF1();

    delete classifier;
    
    //tests from file....
    if(file){

        string line = "";
        while(std::getline(file,line)){
            vector<string> tokens;
            Tokenizer::stringTokenize(line, tokens, " ");
            if(tokens.size() == 0) continue;

            cout<<"line = " << line <<endl;
            std::map<string, double> credibilityMap;
            std::vector< std::map<string, double> > graphsCredibility(stats->getNumberOfGraphs());
            
            if(stats->getUsingTermCredibility()){

               for( set<string>::iterator vocIt = (stats->getVocabulary()).begin(); vocIt != (stats->getVocabulary()).end(); vocIt++) {
                    for(set<string>::iterator clIt = (stats->getClasses()).begin();clIt != (stats->getClasses()).end(); clIt++) {
                
                        std::stack<double> values;

                        for(vector<string>::reverse_iterator rit = tokens.rbegin(); rit != tokens.rend(); rit++){
//                            cout<<*rit<<endl;

                            if(isOperator(*rit)){
//                               cout<<"operator = " << *rit <<endl;
                                double result = getResult(*rit, values);
                                values.push(result);
                            }
                            else if(isOperand(*rit)){
//                                cout<<"operand = " << *rit << endl;
                                values.push( getOperandValue(*rit, *vocIt, *clIt) );
                            }
                            else{
                                cerr<<"Error..."<< *rit << " is not known."<<endl;
                                exit(1);
                            }
                        }
//                      cout<< endl;
                        double result = values.top();
                        values.pop();
//                      cout << result <<endl;
                        credibilityMap[ getCompIndex(*vocIt,*clIt) ]  = result;
                    }
                }
            }

            if(stats->getNumberOfGraphs()){
                cout<<"Using Graph credibility"<<endl;
                for(int g = 0; g < stats->getNumberOfGraphs(); g++){

                    for(ExampleIterator it = io->getTest().getBegin(); it != io->getTest().getEnd(); it++){
                        for(set<string>::iterator classIt = stats->getClasses().begin(); classIt != stats->getClasses().end(); classIt++){
                            string docId = it->getId();
                            string classId = *classIt;
                            std::stack<double> values;

                            for(vector<string>::reverse_iterator rit = tokens.rbegin(); rit != tokens.rend(); rit++){
                                //                            cout<<*rit<<endl;

                                if(isOperator(*rit)){
                                    //                               cout<<"operator = " << *rit <<endl;
                                    double result = getResult(*rit, values);
                                    values.push(result);
                                }
                                else if(isOperand(*rit)){
                                    //                                cout<<"operand = " << *rit << endl;
                                    values.push( getOperandValue(*rit, docId, classId, g) );
                                }
                                else{
                                    cerr<<"Error..."<< *rit << " is not known."<<endl;
                                    exit(1);
                                }
                            }
                            //cout<< endl;
                            double result = values.top();
                            values.pop();
                            //                      cout << result <<endl;
                            graphsCredibility[g][getCompIndex(docId, classId)]  = result;
                        }
                    }
                }
            }
            
            if(stats->getUsingKNN())
                classifier = new KNN(stats);
            else
                classifier = new NaiveBayes(stats);
            classifier->useContentCredibility(true);
            
            if(stats->getUsingTermCredibility())
                classifier->setContentCredibilityMap(credibilityMap);

            if(stats->getNumberOfGraphs())
                classifier->setGraphCredibilityMaps(graphsCredibility);

            classifier->train(io->getTrain());
            classifier->test(io->getTest());

            cout<<"BL = " << blmicro << "\t" << blmacro <<endl;
            cout<<"Resultado = " << classifier->getMicroF1()<< "\t" << classifier->getMacroF1()<<endl;
            cout.setf(ios::fixed,ios::floatfield);
            cout.precision(4);
            cout<<"Melhorias = " << ((classifier->getMicroF1()/blmicro) -1.0) *100.0 << "\t" << ((classifier->getMacroF1()/blmacro) -1.0) *100.0 <<endl;

            ostream &out = io->getEvaluateFile();
            out.setf(ios::fixed,ios::floatfield);
            out.precision(5);
            out<<line<<"\t"<< classifier->getMicroF1() << "\t" << classifier->getMacroF1()<<endl;
    
            delete classifier;
        }
    }
    else{
        cerr<<"Error! Evaluate file \""<<fileName<<"\"not found."<<endl;
        exit(1);
    }

    file.close();
}

double TreeEvaluator::getResult(string operatorID, std::stack<double>& values){

    double a = values.top();
    values.pop();
      
    //TODO: conferir numero de operadores...
    double b = values.top();
    values.pop();

    if(operatorID == "Pow"){
        return my_pow(a,b);
    }
    else if(operatorID == "Log"){
        return my_div(my_log(a), my_log(b));
    }
    else if(operatorID == "+"){
        return my_sum(a,b);
    }
    else if(operatorID == "-"){
        return my_sub(a,b);
    }
    else if(operatorID == "*"){
        return my_mult(a,b);
    }
    else if(operatorID == "%"){
        return my_div(a,b);
    }
                    
    return 0.0;
}

bool TreeEvaluator::isOperand(string val){
    return true;
}


bool TreeEvaluator::isOperator(string val){
    return has(6, val.c_str(),"Pow","+","*","-","%","Log");
}

bool TreeEvaluator::has(int limit, const char *word, ...)
{
	va_list argp;
	va_start(argp, word);

	for( ; limit; limit--){
	    const char *s = va_arg(argp, char*); 
	    
	    if(!strcmp(s,word)){
    		va_end(argp);
	    	return true;
	    }
	}

	va_end(argp);
	return false;
}

double TreeEvaluator::getOperandValue(string operand, string id, string className, int graphId){

    if(operand == "AM"){   
        return stats->getAM(id, className);
    }
    else if(operand == "P"){
        return my_div(stats->getTFperClass(id, className) , stats->getTFperTerm(id));
    }
    else if(operand == "P'"){
        return 1.0 - my_div(stats->getTFperClass(id, className) , stats->getTFperTerm(id));
    }
    else if(operand == "GINI"){ 
        return stats->getGini(id);
    }
    else if(operand == "DOM"){
        return stats->getDominance(id, className);
    }
    else if(operand == "IG"){
        return stats->getIG(id, className);
    }
    else if(operand == "CHI"){
        return stats->getCHI(id, className);
    }
    else if(operand == "GSS"){
        return stats->getGSS(id, className);
    }    
    else if(operand == "CE"){
        return stats->getCE(id);
    }
    else if(operand == "CC"){
        return stats->getCC(id, className);
    }
    else if(operand == "OR"){
        return stats->getOR(id, className);
    }
    else if(operand == "CTD"){
        return stats->getCTD(id, className);
    }
    else if(operand == "IDF"){
        return stats->getCTD(id, className);
    }
    else if(operand == "TFICF"){
        return stats->getTFICF(id, className);
    }
    else if(operand == "TFIDF"){
        return stats->getTFIDF(id, className);
    }
    else if(operand == "TF"){
        return 1.0 + my_log(stats->getTFperClass(id, className));
    }
    else if(operand == "sumTF"){
        return 1.0 + my_log(stats->getTFperTerm(id));
    }
    else if(operand == "DF"){
        return 1.0 + my_log(stats->getDFperClass(id, className));
    }
    else if(operand == "sumDF"){
        return 1.0 + my_log(stats->getDFperTerm(id));
    }
    else if(operand == "MaxDom"){
        return stats->getMaxDominance(id);
    }
    else if(operand == "MaxIG"){
        return stats->getMaxIG(id);
    }
    else if(operand == "MaxAM"){
        return stats->getMaxAM(id);
    }
    else if(operand == "MaxCHI"){
        return stats->getMaxCHI(id);
    }
    else if(operand == "MaxGSS"){
        return stats->getMaxGSS(id);
    }
    else if(operand == "MaxCC"){
        return stats->getMaxCC(id);
    }
    else if(operand == "MaxOR"){
        return stats->getMaxOR(id);
    }
    else if(operand == "MaxCTD"){
        return stats->getMaxCTD(id);
    }
    else if(operand == "MaxTFIDF"){
        return stats->getMaxTFIDF(id);
    }
    else if(operand == "MaxTFICF"){
        return stats->getMaxTFICF(id);
    }

    else if(operand == "Neighborhood1"){
        return stats->getNeighborhoodSize1(id,className,graphId);
    }
    else if(operand == "Neighborhood2"){
        return stats->getNeighborhoodSize2(id,className,graphId);
    }
    else if(operand == "Neighborhood3"){
        return stats->getNeighborhoodSize3(id,className,graphId);
    }
    else if(operand == "HubScore"){
        return stats->getHubScore(id, className, graphId);
    }
    else if(operand == "Authority"){
        return stats->getAuthority(id, className, graphId);
    }
    else if(operand == "EigenVector"){
        return stats->getEigenVectorCentrality(id, className, graphId);
    }
    else if(operand == "Closeness"){
        return stats->getCloseness(id, className, graphId);
    }
    else if(operand == "Strength"){
        return stats->getStrength(id, className, graphId);
    }
    else if(operand == "Constraint"){
        return stats->getConstraint(id, className, graphId);	
    }
    else if(operand == "PageRank"){
        return stats->getPageRank(id, className, graphId);
    }
    else if(operand == "Betweenness"){
        return stats->getBetweenness(id, className, graphId);
    }
    else if(operand == "BibCoupling"){
        return stats->getBibCoupling(id, className, graphId);
    }
    else if(operand == "CoCitation"){
        return stats->getCoCitation(id, className, graphId);
    }
    else if(operand == "JaccardSimilarity"){
        return stats->getJaccardSimilarity(id, className, graphId);
    }
    else if(operand == "DiceSimilarity"){
        return stats->getDiceSimilarity(id, className, graphId);
    }
    else if(operand == "InverseLigSimilarity"){
        return stats->getInverseLogSimilarity(id, className, graphId);
    }
    else if(operand == "AvgNeighborHoodDegree"){
        return stats->getAvgNearstNeighborDegree(id, className, graphId);
    }

    else{
        cerr<<"Error!! Invalid operand: " << operand << endl;
        exit(1);
    }

    return 0.0;
}


