#include<iostream>
#include<cstdlib>
#include<cstdio>
#include<cstring>
#include<cstdarg>
#include<vector>

#include "IClassifier.h"
#include "NaiveBayes.h"
#include "KNN.h"
#include "Util.h"
#include "InOut.h"
#include "Statistics.h"
#include "IGP.h"
#include "gpcpp.h"
#include "TreeEvaluator.h"

#include "gp.h"


void getParameters(int argc, char** argv, string& trainFile, string& validationFile, bool& usingValidation, string& testFile, string& parameterConfigFileName, int &seed, bool& loadPop, string& loadFileName, bool& dontsave, string& saveFileName, bool& newSaveFileName, bool& useTermsCredibility, vector<string> &graphsNames, int& numCollums, string& predictionsFilename,  bool& printPredictionsFile, string& brunoroFilename, bool& printBrunoroFile, bool& normalEstimator, bool& normalizeTermsPerGreatestClassValue, string& evaluationFileName, bool& evaluateFromFile, string &evaluationDestiny, int& genToChange, bool& evalOnly, bool& optimizeGraphMetrics, bool& useKNN);
void configureTermCredibility(Statistics& stats, bool useTermCredibility, bool normalEstimator, bool normalizeTermsPerGreatestClassValue);
void configureGraphCredibility(Statistics& stats, InOut& io, vector<string>& graphsNames);

int main(int argc, char **argv)
{

	TRACE_V("MAIN","Starting program.");

	string trainFile = "", validationFile = "", testFile = "", loadFileName = "", saveFileName = "pop.dat";
    string predictionsFileName = "predictions.out", brunoroFileName = "brunoro", evaluationFileName = "";
    vector<string> graphsNames;
    string baseName = "cred", evaluationDestiny = "evaluate.out";
    string parameterConfigFileName = "gp.ini";
    string finalOutFileName = "final.out";

    int seed = 0, numericalCollums = 0;
    int genToChange = 0;
	bool loadPop = false, dontsave = false,  newSaveFileName = false, useTermCredibility = true, usingValidation = false;
    bool printPredictionsFile = true, printBrunoroFile = false, normalEstimator = false, evaluationFromFile = false, evalOnly = false;
    bool normalizeTermsPerGreatestClassValue = false;
    bool optimizeGraphMetrics = true;
    bool useKNN = false;

	getParameters(argc, argv, trainFile, validationFile, usingValidation, testFile, parameterConfigFileName, seed, loadPop, loadFileName, dontsave, saveFileName, newSaveFileName, useTermCredibility, graphsNames, numericalCollums, predictionsFileName, printPredictionsFile, brunoroFileName, printBrunoroFile, normalEstimator, normalizeTermsPerGreatestClassValue, evaluationFileName, evaluationFromFile, evaluationDestiny, genToChange, evalOnly, optimizeGraphMetrics, useKNN);

	//Get Files
	InOut io(baseName, seed);
	Statistics stats;

    stats.setOptimizeGraphMetrics(optimizeGraphMetrics);
    
    io.setGPParameterConfigFileName(parameterConfigFileName);
    io.setFinalOutFile(finalOutFileName);

    if(printBrunoroFile){
        io.setBrunoroFile(brunoroFileName);
    }

    if(printPredictionsFile){
        io.setPredictionsFile(predictionsFileName);
    }
    io.setNumericalCollums(numericalCollums);
	io.readTrain(trainFile.c_str());
	
    if((evaluationFromFile || evalOnly ) && usingValidation)
        io.readTrain(validationFile.c_str());
    
    //Calculate some statistics using the entries
    stats.readExamples(io.getTrain());
    configureTermCredibility(stats, useTermCredibility, normalEstimator, normalizeTermsPerGreatestClassValue);
	
    if(evaluationFromFile){

        io.setEvaluateFile(evaluationDestiny);
        io.readTest(testFile.c_str());
        configureGraphCredibility(stats, io, graphsNames); // it is necessary to know the testset before call this

        TreeEvaluator t(&stats, &io);
        t.evaluateFromFile(evaluationFileName);
        
        return 0;
    }
    else if(evalOnly){
        
        io.readTest(testFile.c_str());

    	ICredibilityClassifier *classifier; 
        if(useKNN)
    	    classifier = new KNN(&stats); 
    	else
            classifier = new NaiveBayes(&stats); 
	    classifier->train(io.getTrain());
        classifier->test(io.getTest());
	    return 0;
    }

    if(usingValidation){
	    io.readTest(validationFile.c_str());
    }
    else{
	    io.readTest(testFile.c_str());
    }
    configureGraphCredibility(stats, io, graphsNames); // it is necessary to know the testset before call this
    
	//Set classifier parameters
    ICredibilityClassifier *classifier;
    if(useKNN)
	    classifier = new KNN(&stats); 
    else
	    classifier = new NaiveBayes(&stats); 
        
	classifier->train(io.getTrain());
    classifier->test(io.getTest());
	classifier->setCredibilityConfigurations(useTermCredibility, graphsNames);
//	classifier->showConfusionMatrix();
//  classifier->showPredictions();
    classifier->printFinalOutFile(io.getFinalOutFile(), "TrainBaseline");

    /*
     *  If we are not using validation, we should not start the gp part.
     *  Test if we want to test a specific function or just leave the system.
     * */
    if(!usingValidation){
        return 0;
    }

	//GP functions
	IGP *gp = new gpcpp(&stats, &io, seed);

	if(loadPop){
		gp->setLoadPopFileName(loadFileName);
		gp->loadPop();
	}
	else{
		gp->createInitialPopulation();
	}
	gp->setSavePopFileName(saveFileName);

	//gp->setCredibilityConfigurations(useTermCredibility, graphsNames);
	
	int genMax = gp->getMaxGen();
	for( int gen = gp->getActualGen(); gen < genMax; gen++){

		cout<<"Atual gen ============================================================================= "<< gen<<endl;

		gp->evolve();	
		//double microF1 = gp->getBestFitness();
		//printf("MicroF1 do melhor individuo = %g\n", microF1);

		if(!dontsave)
			gp->savePop();

        if(genToChange && (gen % genToChange == 0)){
            io.mergeTrainAndTest();
            io.makeNewTest();
            
            stats.clear();
            stats.readExamples(io.getTrain());

            configureTermCredibility(stats, useTermCredibility, normalEstimator, normalizeTermsPerGreatestClassValue);
            configureGraphCredibility(stats, io, graphsNames);
        }

	}
    
    gp->printFinalOutFile();
	delete classifier;
    classifier = 0;

    /*
    //   Run the final test. Where i can find out how better my algorithm is! 
    */


    //Cleans IO files
    io.clear();
    stats.clear();
	
    io.setNumericalCollums(numericalCollums);
    io.readTrain(trainFile.c_str());
    io.readTrain(validationFile.c_str());
    io.readTest(testFile.c_str());
    
	stats.readExamples(io.getTrain());
    
    configureTermCredibility(stats, useTermCredibility, normalEstimator, normalizeTermsPerGreatestClassValue);
    configureGraphCredibility(stats, io, graphsNames);
    
    cout<<endl<<"Running final baseline:"<<endl;
	if(useKNN)
        classifier = new KNN(&stats); 
	else
        classifier = new NaiveBayes(&stats); 

	classifier->train(io.getTrain());
	//classifier->train(io.getValidation());
	classifier->test(io.getTest());
	classifier->showConfusionMatrix();
    classifier->printFinalOutFile(io.getFinalOutFile(), "TestBaseline");
    cout<<"[OK]"<<endl;

	classifier->setCredibilityConfigurations(useTermCredibility, graphsNames);
	cout<<"Performing final test: "<<endl;
    gp->performeFinalTest();
	
    
    delete classifier;
    delete gp;
	return 0;
}

void configureTermCredibility(Statistics& stats, bool useTermCredibility, bool normalEstimator, bool normalizeTermsPerGreatestClassValue){
    if(useTermCredibility){
		stats.setUsingTermCredibility();
        stats.setNormalEstimator(normalEstimator);    
        stats.setNormalizeTermsPerGreatestClassValue(normalizeTermsPerGreatestClassValue);

        stats.retrieveContentMetrics();
    }
}

void configureGraphCredibility(Statistics& stats, InOut& io, vector<string>& graphsNames){

    if(graphsNames.size() != 0){
        cout<<"Ai vao os graphs."<<endl;
        for(unsigned int j = 0; j < graphsNames.size(); j++){
            cout<<graphsNames[j] <<endl;
            stats.readGraph(graphsNames[j]);
        }

        //TODO: descomentar 
        stats.retrieveGraphMetrics(io.getTest());
        //  stats.retrieveGraphMetricsDebug(io.getTest());
    }
}	

void parametersHelp()
{
	cout<<endl;
	cout<<"Parameters options:"<<endl;
	cout<<"          -treino FileName: Train file."<<endl;
	cout<<"          -validacao FileName: Validation file."<<endl;
	cout<<"          -teste FileName: Test file."<<endl;
	cout<<"          -param FileName: GP Parameters file. (default: gp.ini)"<<endl;
	cout<<"          -seed int: Seed."<<endl;
	cout<<"          -g x FileName: Name of graph number x."<<endl;
	cout<<"          -load FileName: Load Population."<<endl;
	cout<<"          -save FileName: Change the prefix used to save population."<<endl;
	cout<<"          -dontsave: Dont save population."<<endl;
	cout<<"          -noterms: Dont use the terms credibility."<<endl;
    cout<<"          -g int string[int]: Number of graphs followed by theirs respective names."<<endl;
    cout<<"          -numerical int: The First X collums after the class mark from th in files are treated as numeric values."<<endl;
    cout<<"          -brunoro Filename: Used to save brunoro's file."<<endl;
    cout<<"          -predictions Filename: Used to save predictions' file."<<endl;
    cout<<"          -normal: Set to use normal estimator (when using numerical fields)."<<endl;
    cout<<"          -perclass: Normalize using the greatest one per class."<<endl;
    cout<<"          -change d: Change Train set from d to d generations."<<endl;
    cout<<"          -eval fileIn fileOut: evaluate a set of credibility functions from fileIn putting results into fileOut."<<endl;
    cout<<"          -evalonly: Only evaluate the baseline."<<endl;
    cout<<"          -dontoptimize: Dont use graphs optimization (use it when memory is a problem)."<<endl;

}

bool has(int limit, const char *word, ...)
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

string getStringArgument(int actual, int argc, char** argv, int offset = 1)
{
	string str;
	if( actual + offset < argc && argv[ actual + offset ][0] != '-'){
		str = argv[ actual + offset ];
	}else{
		fprintf (stderr, "Option %s requires an argument or is invalid.\n", argv[actual]);
		exit(1);
	}
	return str;
}


int getIntArgument(int actual, int argc, char** argv)
{
	string str = getStringArgument(actual, argc, argv);
	for(unsigned int i = 0; i < str.size(); i++){
		if(!isdigit(str[i])){
			fprintf (stderr, "Option -%s requires an integer argument.\n", argv[actual]);
			exit(1);
		}
	}
	return atoi(str.c_str());
}

int getGraphsArgument(int actual, int argc, char** argv, vector<string> &graphsNames)
{
	int numberOfGraphs = getIntArgument(actual, argc, argv);

	for(int j = 1; j <= numberOfGraphs; j++){
		graphsNames.push_back(getStringArgument(actual,argc,argv,j+1));
	}
	return numberOfGraphs;
}


int isdigit (int c)
{
	if (c >= '0' && c <='9')
		return 1;
	else
		return 0;
}

void getParameters(int argc, char** argv, string& trainFile, string& validationFile, bool& usingValidation, string& testFile, string& parameterConfigFileName, int &seed, bool& loadPop, string& loadFileName, bool& dontsave, string& saveFileName, bool& newSaveFileName, bool& useTermsCredibility, vector<string> &graphsNames, int& numCollums, string& predictionsFilename,  bool& printPredictionsFile, string& brunoroFilename, bool& printBrunoroFile, bool& normalEstimator, bool& normalizeTermsPerGreatestClassValue, string& evaluationFileName, bool& evaluationFromFile, string &evaluationDestiny, int& genToChange, bool& evalOnly, bool& optimizeGraphMetrics, bool& useKNN)
{
	TRACE_V("MAIN","getParameters.");
	
	bool error = false;
    int numberOfGraphs = 0;

	for(int i = 1 ; i < argc; i ++){	
		//cout<< "Parametro " << i << " = " << argv[i]<<endl;

		if( has(4 , argv[i], "-h", "--help", "-help", "--h") ){
			cout<<"Help!"<<endl;
			parametersHelp();
			exit(0);
		}
		else if( has(4, argv[i], "-treino", "--treino", "-train", "--train") ){
			trainFile = getStringArgument(i,argc,argv);
			i++;
			cout<< "Training File: "<< trainFile <<endl;
		}
		else if( has(6, argv[i], "-validacao", "--validacao", "-v", "--v", "-validation", "--validation") ){
			usingValidation = true;
			validationFile = getStringArgument(i,argc,argv);
			i++;
			cout<< "Validation File: "<<validationFile<<endl;
		}else if( has(4, argv[i], "-teste", "--teste", "-test" , "--test") ){
            testFile = getStringArgument(i,argc,argv);
			i++;
			cout<<"Test File = " << testFile<<endl;
		}
		else if( has(4, argv[i], "-seed", "--seed", "-s", "--s") ){
			seed = getIntArgument(i,argc,argv);
			i++;
			cout<<"Setting new seed = " << seed<<endl;
		}
		else if( has(6, argv[i], "-load", "-loadfile", "--load", "--loadfile", "-l", "--l") ){
			loadPop = true;
			loadFileName = getStringArgument(i,argc,argv);
			i++;
			cout<<"Load Population actived"<<endl;
		}
		else if( has(6, argv[i],"-d", "--d", "-dontsave", "-notsave", "--dontsave", "--notsave") ){
			dontsave = true;
			cout<<"Dont save populations"<<endl;
		}
		else if( has(4, argv[i], "-savefile", "-save", "--savefile", "--save") ){
			newSaveFileName = true;
			saveFileName = getStringArgument(i,argc,argv);
			i++;
			cout<<"Setting save file prefix to " << saveFileName <<endl;
		}
		else if( has(4, argv[i], "-noterm", "--noterm", "-noterms", "--noterms") ){
			useTermsCredibility = false;
			cout<<"Disabling term credibility." << endl;
		}
		else if( has(4, argv[i], "-g", "--g","--graphs","-graphs") ){
			numberOfGraphs = getGraphsArgument(i,argc,argv,graphsNames);
			i += numberOfGraphs + 1;
			cout<<"Getting "<< numberOfGraphs << " graphs."<<endl;
			
			for(int k = 0; k < numberOfGraphs; k++){
				cout<<"Graph "<< k << " : "<< graphsNames[k] <<endl;
			}
		}
		else if( has(6, argv[i], "-num", "--num","-numeric","--numeric","-numerical","--numerical") ){
			numCollums = getIntArgument(i,argc,argv);
			i++;
			cout<< "Number of collums set as numeric: "<< numCollums <<endl;
		}
	   	else if( has(4, argv[i], "-brunoro", "--brunoro", "-b", "--b") ){
			brunoroFilename = getStringArgument(i,argc,argv);
			printBrunoroFile = true;
            i++;
			cout<< "Brunoro File: "<< brunoroFilename <<endl;
		}
    	else if( has(4, argv[i], "-predictions", "--predictions", "-pred", "--pred") ){
			predictionsFilename = getStringArgument(i,argc,argv);
			printPredictionsFile = true;
            i++;
			cout<< "Prediction File: "<< predictionsFilename <<endl;
		}
        else if( has(2, argv[i], "-normal", "--normal") ){
			normalEstimator = true;
			cout<< "Using Normal estimator."<<endl;
		}
		else if( has(2, argv[i], "-perclass", "--perclass") ){
			normalizeTermsPerGreatestClassValue = true;
			cout<< "Using Normalization per greatest class value."<< endl;
		}
		else if( has(4, argv[i], "-eval", "--eval", "-evaluate", "--evaluate") ){
            evaluationFromFile = true;
            evaluationFileName = getStringArgument(i,argc,argv);
            i++;
            evaluationDestiny = getStringArgument(i,argc,argv);
            i++;
            cout<< "Evaluation functions from file: " << evaluationFileName<<endl;
            cout<< "Evaluation functions writing file: " << evaluationDestiny<<endl;
        }
		else if( has(6, argv[i], "-from", "--from", "-parameters", "--parameters", "-param", "--param") ){
            parameterConfigFileName = getStringArgument(i,argc,argv);
            i++;
            cout<< "Parameters from file " << parameterConfigFileName<<endl;
        }
		else if( has(4, argv[i], "-changeGen", "--changeGen", "-change", "--change") ){
            genToChange = getIntArgument(i,argc,argv);
            i++;
            cout<<"Changing train set from "<< genToChange << " to " <<genToChange <<" generations. " <<endl;
        }
		else if( has(4, argv[i], "-evalonly", "--evalonly", "-evalOnly", "--evalOnly") ){
            evalOnly = true;
            cout<<"Only evaluating the baseline."<<endl;
        }
		else if( has(4, argv[i], "-dontoptimize", "--dontoptmize", "-naootimiza", "--naootimiza", "--notopt", "-notopt") ){
            optimizeGraphMetrics = false;
            cout<<"Not optimizing graph metrics"<<endl;
        }
    	else if( has(2, argv[i], "-knn", "--knn") ){
            useKNN = false;
            cout<<"Using KNN"<<endl;
        }
        else{
			fprintf (stderr, "Unknown option `-%s'.\n", argv[i]);
            parametersHelp();
			exit(1);
		}
	}
	if(trainFile.empty()){
		cerr<<endl<<"Please, use '-treino' option to add a train file."<<endl;
		error = true;
	}
	if(testFile.empty() && validationFile.empty()){
		cerr<<"Please, use '-teste' or '-validacao' option to enter a test or validation file."<<endl;
		error = true;
	}
	if(!numberOfGraphs && !useTermsCredibility){
		cerr<<"Please, use graphs or use terms to calculate a credibility function."<<endl;
		error = true;
	}
    if(evalOnly && evaluationFromFile){
        cerr<<"Please, use only evalOnly or evaluationFromFile Option";
        error = true;
    }
	if(error){
		parametersHelp();
		exit(1);
	}
} 





