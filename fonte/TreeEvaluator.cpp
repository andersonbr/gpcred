#include "TreeEvaluator.h"

#include "Tokenizer.h"
#include "Matematica.h"
#include "NaiveBayes.h"

#include<cstdarg> 
#include<map>

TreeEvaluator::TreeEvaluator(Statistics* stats, InOut* io) : stats(stats),io(io){
}

TreeEvaluator::~TreeEvaluator(){
}

void TreeEvaluator::evaluateFromFile(string fileName){

    //baseline test
    ifstream file(fileName.c_str(), ifstream::in);

    NaiveBayes *classifier = new NaiveBayes(stats);
    classifier->useContentCredibility(true);

    classifier->train(io->getTrain());
    classifier->test(io->getTest());
    
    ostream &out = io->getEvaluateFile();
    out.setf(ios::fixed,ios::floatfield);
    out.precision(5);
    out<<"Baseline"<<"\t"<< classifier->getMicroF1() << "\t" << classifier->getMacroF1()<<endl;
    
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

            for( set<string>::iterator vocIt = (stats->getVocabulary()).begin(); vocIt != (stats->getVocabulary()).end(); vocIt++) {
                for(set<string>::iterator clIt = (stats->getClasses()).begin();clIt != (stats->getClasses()).end(); clIt++) {
                
                    std::stack<double> values;

                    for(vector<string>::reverse_iterator rit = tokens.rbegin(); rit != tokens.rend(); rit++){
//                        cout<<*rit<<endl;

                        if(isOperator(*rit)){
//                            cout<<"operator = " << *rit <<endl;
                            double result = getResult(*rit, values);
                            values.push(result);
                        }
                        else if(isOperand(*rit)){
//                            cout<<"operand = " << *rit << endl;
                            values.push( getOperandValue(*rit, *vocIt, *clIt) );
                        }
                        else{
                            cerr<<"Error..."<< *rit << " is not known."<<endl;
                            exit(1);
                        }
                    }
//                    cout<< endl;
                    double result = values.top();
                    values.pop();
//                    cout << result <<endl;
                    credibilityMap[ getCompIndex(*vocIt,*clIt) ]  = result;
                }
            }
            
            NaiveBayes *classifier = new NaiveBayes(stats);
            classifier->useContentCredibility(true);
            classifier->setContentCredibilityMap(credibilityMap);
            
            classifier->train(io->getTrain());
            classifier->test(io->getTest());

            cout<<"Resultado = " << classifier->getMicroF1()<<endl;

            ostream &out = io->getEvaluateFile();
            out.setf(ios::fixed,ios::floatfield);
            out.precision(5);
            out<<line<<"\t"<< classifier->getMicroF1() << "\t" << classifier->getMacroF1()<<endl;

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

double TreeEvaluator::getOperandValue(string operand, string id, string className){

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
    else{
        cerr<<"Error!! Invalid operand: " << operand << endl;
        exit(1);
    }

    return 0.0;
}


