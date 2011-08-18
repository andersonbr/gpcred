#include <iostream>

#include "gpcpp.h"
#include "MyGP.h"
#include "Util.h"

using namespace std;

gpcpp::gpcpp(Statistics *stats, InOut* io, int seed) : IGP(seed), stats(stats), io(io) {

    GPInit(1,this->seed);
    GPRegisterClass(new MyGP(stats,io));
    GPRegisterClass(new MyGene(stats));
    GPRegisterClass(new MyPopulation(stats,io));

    struct GPConfigVarInformation cfgArray[] = {
        {(char*) "PopulationSize", DATAINT, &cfg.PopulationSize},
        {(char*)"NumberOfGenerations", DATAINT, &cfg.NumberOfGenerations},
        {(char*)"CreationType", DATAINT, &cfg.CreationType},
        {(char*)"CrossoverProbability", DATADOUBLE, &cfg.CrossoverProbability},
        {(char*)"CreationProbability", DATADOUBLE, &cfg.CreationProbability},
        {(char*)"MaximumDepthForCreation", DATAINT, &cfg.MaximumDepthForCreation},
        {(char*)"MaximumDepthForCrossover", DATAINT, &cfg.MaximumDepthForCrossover},
        {(char*)"SelectionType", DATAINT, &cfg.SelectionType},
        {(char*)"TournamentSize", DATAINT, &cfg.TournamentSize},
        {(char*)"DemeticGrouping", DATAINT, &cfg.DemeticGrouping},
        {(char*)"DemeSize", DATAINT, &cfg.DemeSize},
        {(char*)"DemeticMigProbability", DATADOUBLE, &cfg.DemeticMigProbability},
        {(char*)"SwapMutationProbability", DATADOUBLE, &cfg.SwapMutationProbability},
        {(char*)"ShrinkMutationProbability", DATADOUBLE, &cfg.ShrinkMutationProbability},
        {(char*)"AddBestToNewPopulation", DATAINT, &cfg.AddBestToNewPopulation},
        {(char*)"SteadyState", DATAINT, &cfg.SteadyState},
        {(char*)"SubSetSize", DATAINT, &cfg.SubSetSize},
        {(char*)"ActiveGPMode", DATAINT, &cfg.ActiveGPMode},
        {(char*)"GenToChangeTrain", DATAINT, &cfg.GenToChangeTrain},
        {(char*)"", DATAINT, NULL}

    };
	
    // Read configuration file.
	GPConfiguration config (cout, (char*) (io->getGPParameterConfigFileName()).c_str(), cfgArray);
	
	// Print the configuration
	cout << cfg << endl;
	
	maxGen = cfg.NumberOfGenerations;
}

gpcpp::~gpcpp(){

	if(pop){
		delete pop;
	}
}

void gpcpp::createInitialPopulation(){

	TRACE_V("gpcpp","createInitialPopulation.");
	
	IGP::createInitialPopulation();
	
	createNodeSet();
	
	std::cout<<adfNs<<endl;
    if(io->isUsingBrunoroFile())
       adfNs.printBrunoro(io->getBrunoroFile());
    
    
	pop = new MyPopulation(cfg,adfNs,stats,io);
	pop->create();
	
	pop->createGenerationReport(1,genCounter, io->getBestFile(), io->getHistFile());
    if(io->isUsingBrunoroFile())
        pop->printBrunoro(io->getBrunoroFile(), genCounter);

}

double gpcpp::getBestFitness(){
	return 0.0;
}

void gpcpp::print(){
	
	if(pop){
		pop->createGenerationReport(0,genCounter,io->getBestFile(),io->getHistFile());
        if(io->isUsingBrunoroFile())
            pop->printBrunoro(io->getBrunoroFile(), genCounter);
    }
}

void gpcpp::evolve(){
	
	TRACE_V("gpcpp", "evolve.");
	
	IGP::evolve();
	
	MyGP::gennum = genCounter;
	MyGP::indCounter = 0;
	
	MyPopulation *newPop = NULL;

	if(!cfg.SteadyState)
		newPop = new MyPopulation(cfg,adfNs,stats,io);

	pop->generate(io->getBrunoroFile(), *newPop);

	if(!cfg.SteadyState){
		delete pop;
		pop = newPop;
	}
	pop->createGenerationReport(0,genCounter, io->getBestFile(), io->getHistFile());
    if(io->isUsingBrunoroFile())
        pop->printBrunoro(io->getBrunoroFile(), genCounter);
}

void gpcpp::performeFinalTest(){
    pop->NthMyGP(pop->bestOfPopulation)->evaluate(true,"TestBest");
}

void gpcpp::printFinalOutFile(){
    pop->NthMyGP(pop->bestOfPopulation)->evaluate(true,"TrainBest");
}

void gpcpp::savePop(){

	TRACE_V("gpcpp", "savePop");
	
	char popfile[64];
	sprintf(popfile,"%s.%d", saveFileName.c_str(), genCounter);
	ofstream savePop(popfile);
	
	adfNs.save(savePop);
	pop->save(savePop);
	savePop.close();
}

void gpcpp::loadPop(){

    TRACE_V("gpcpp", "loadPop.");

    IGP::createInitialPopulation();
    
    ifstream loadPopFile(loadFileName.c_str());
    if(!loadPopFile){ cerr<<"Population file not found. Check the file name please."<<endl; exit(0);}

    pop = new MyPopulation(cfg,adfNs,stats,io);
    adfNs.load(loadPopFile);
    pop->load(loadPopFile);
    pop->setNodeSets(adfNs);

    loadPopFile.close();
    
    pop->createGenerationReport(0,genCounter, io->getBestFile(), io->getHistFile());
}


void gpcpp::createNodeSet(){

	// Reserve space for the node sets (root tree and ADF tree for ratios 
	// (decrease structural complexity)

	int contentCredibilityOffset = stats->getUsingTermCredibility() || stats->getUsingCategoricalCredibility() ? 1 : 0;

	adfNs.reserveSpace (contentCredibilityOffset + stats->getNumberOfGraphs());

	cout<<" contentCredibilityOffset = " << contentCredibilityOffset <<endl;
	cout<<" stats->getNumberOfGraphs() = "<< stats->getNumberOfGraphs()<<endl;


	GPNodeSet **ns;
	ns = new GPNodeSet * [ contentCredibilityOffset  + stats->getNumberOfGraphs() ];

	if(stats->getUsingTermCredibility()){

		ns[0] = new GPNodeSet (35);
		adfNs.put(0, *ns[0]);

		ns[0]->putNode (*new GPNode (SUM, (char*) "+", 2));
		ns[0]->putNode (*new GPNode (SUB, (char*) "-", 2));
		ns[0]->putNode (*new GPNode (MULT, (char*) "*", 2));
		ns[0]->putNode (*new GPNode (DIV, (char*) "%", 2));
		ns[0]->putNode (*new GPNode (LOG, (char*) "Log", 2));
		ns[0]->putNode (*new GPNode (POW, (char*) "Pow", 2));

		ns[0]->putNode (*new GPNode (AM, (char*) "AM"));
		ns[0]->putNode (*new GPNode (PROB_COND, (char*)  "P"));
		ns[0]->putNode (*new GPNode (PROB_COND_NEG,(char*) "P'"));
		ns[0]->putNode (*new GPNode (GINI,(char*)  "GINI"));
		ns[0]->putNode (*new GPNode (DOMINANCE, (char*) "DOM"));
		ns[0]->putNode (*new GPNode (IG,(char*) "IG"));
		ns[0]->putNode (*new GPNode (CHI,(char*) "CHI"));
		ns[0]->putNode (*new GPNode (GSS,(char*) "GSS"));
		ns[0]->putNode (*new GPNode (CC,(char*) "CC"));
		ns[0]->putNode (*new GPNode (CE,(char*) "CE"));
		ns[0]->putNode (*new GPNode (OR,(char*) "OR"));
		ns[0]->putNode (*new GPNode (CTD,(char*) "CTD"));
		ns[0]->putNode (*new GPNode (IDF,(char*) "IDF"));
		ns[0]->putNode (*new GPNode (TFICF,(char*) "TFICF")); //NECESSARIO avaliar usando 1 + log(TFICF) e TFICF/MAX
		ns[0]->putNode (*new GPNode (TFIDF, (char *) "TFIDF"));//NECESSARIO avaliar usando 1 + log(TFIDF) e TFIDF/MAX
		ns[0]->putNode (*new GPNode (TF_PER_CLASS, (char*) "TF"));   //usando 1 + log(TF_PER_CLASS)
		ns[0]->putNode (*new GPNode (TF_PER_TERM, (char*) "sumTF"));     //usando 1 + log(TF_PER_TERM)
		ns[0]->putNode (*new GPNode (DF_PER_CLASS,(char*) "DF"));  //usando 1 + log(DF_PER_CLASS)
		ns[0]->putNode (*new GPNode (DF_PER_TERM,(char*) "sumDF"));
		ns[0]->putNode (*new GPNode (MAX_DOMINANCE, (char*) "MaxDom"));
		ns[0]->putNode (*new GPNode (MAX_IG,(char*) "MaxIG"));
		ns[0]->putNode (*new GPNode (MAX_AM, (char*) "MaxAM"));
		ns[0]->putNode (*new GPNode (MAX_CHI,(char*) "MaxCHI"));
		ns[0]->putNode (*new GPNode (MAX_GSS,(char*) "MaxGSS"));
		ns[0]->putNode (*new GPNode (MAX_CC,(char*) "MaxCC"));
		ns[0]->putNode (*new GPNode (MAX_OR,(char*) "MaxOR"));
		ns[0]->putNode (*new GPNode (MAX_CTD,(char*) "MaxCTD"));
		ns[0]->putNode (*new GPNode (MAX_TFIDF,(char*) "MaxTFIDF")); 
		ns[0]->putNode (*new GPNode (MAX_TFICF,(char*) "MaxTFICF")); 

	}
	else if(stats->getUsingCategoricalCredibility()){

		ns[0] = new GPNodeSet (16);
		adfNs.put(0, *ns[0]);

		ns[0]->putNode (*new GPNode (SUM, (char*) "+", 2));
		ns[0]->putNode (*new GPNode (SUB, (char*) "-", 2));
		ns[0]->putNode (*new GPNode (MULT, (char*) "*", 2));
		ns[0]->putNode (*new GPNode (DIV, (char*) "%", 2));
		ns[0]->putNode (*new GPNode (LOG, (char*) "Log", 2));
		ns[0]->putNode (*new GPNode (POW, (char*) "Pow", 2));

		ns[0]->putNode (*new GPNode (NUM1, (char*) "NUM1"));
    	ns[0]->putNode (*new GPNode (NUM2, (char*) "NUM2"));
    	ns[0]->putNode (*new GPNode (NUM3, (char*) "NUM3"));
		ns[0]->putNode (*new GPNode (AM, (char*) "AM"));
		ns[0]->putNode (*new GPNode (GINI,(char*)  "GINI"));
		ns[0]->putNode (*new GPNode (IG,(char*) "IG"));
		ns[0]->putNode (*new GPNode (CHI,(char*) "CHI"));
		ns[0]->putNode (*new GPNode (GSS,(char*) "GSS"));
		ns[0]->putNode (*new GPNode (CC,(char*) "CC"));
		ns[0]->putNode (*new GPNode (OR,(char*) "OR"));
//		ns[0]->putNode (*new GPNode (TFIDF, (char *) "TFIDF"));

	}


	for(int i =  contentCredibilityOffset; i < (stats->getNumberOfGraphs() +  contentCredibilityOffset); i++){
		ns[i] = new GPNodeSet (22);
		adfNs.put (i, *ns[i]);

		ns[i]->putNode (*new GPNode (SUM, (char*) "+", 2));
		ns[i]->putNode (*new GPNode (SUB, (char*) "-", 2));
		ns[i]->putNode (*new GPNode (MULT, (char*) "*", 2));
		ns[i]->putNode (*new GPNode (DIV, (char*) "%", 2));
		ns[i]->putNode (*new GPNode (POW, (char*) "Pow", 2));
		ns[i]->putNode (*new GPNode (LOG, (char*) "Log", 2));  //Six operators

		ns[i]->putNode(*new GPNode (NEIGHBORHOOD1, (char*) "Neighborhood1"));
		ns[i]->putNode(*new GPNode (NEIGHBORHOOD2, (char*) "Neighborhood2"));
		ns[i]->putNode(*new GPNode (NEIGHBORHOOD3, (char*) "Neighborhood3"));
		ns[i]->putNode(*new GPNode (HUBSCORE, (char*) "HubScore"));
		ns[i]->putNode(*new GPNode (AUTHORITY, (char*) "Authority"));
        ns[i]->putNode(*new GPNode (EIGENVECTOR, (char*) "EigenVector"));
		ns[i]->putNode(*new GPNode (CLOSENESS, (char*) "Closeness")); 
		ns[i]->putNode(*new GPNode (STRENGTH, (char*) "Strength"));
		ns[i]->putNode(*new GPNode (CONSTRAINT, (char*) "Constraint"));
		ns[i]->putNode(*new GPNode (PAGERANK, (char*) "PageRank"));
		ns[i]->putNode(*new GPNode (BETWEENNESS, (char*) "Betweenness"));
		ns[i]->putNode(*new GPNode (BIBCOUPLING, (char*) "BibCoupling"));
		ns[i]->putNode(*new GPNode (COCITATION, (char*) "CoCitation"));
		ns[i]->putNode(*new GPNode (JACCARDSIMILARITY, (char*) "JaccardSimilarity"));
		ns[i]->putNode(*new GPNode (DICESIMILARITY, (char*) "DiceSimilarity"));
		ns[i]->putNode(*new GPNode (INVERSELOGSIMILARITY, (char*) "InverseLogSimilarity")); //16 terms
//NAO USAR!  ns[i]->putNode(*new GPNode (AVGNEIGHBORHOODDEGREE, (char*) "AvgNeighborHoodDegree"));
	
    }

	delete []ns;
}


