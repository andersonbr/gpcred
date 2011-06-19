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
//	configArray[0] = {"PopulationSize", DATAINT, &cfg.PopulationSize};
/*	configArray[1] = {(char*)"NumberOfGenerations", DATAINT, &cfg.NumberOfGenerations};
	configArray[2] = {(char*)"CreationType", DATAINT, &cfg.CreationType};
	configArray[3] = {(char*)"CrossoverProbability", DATADOUBLE, &cfg.CrossoverProbability};
	configArray[4] = {(char*)"CreationProbability", DATADOUBLE, &cfg.CreationProbability};
	configArray[5] = {(char*)"MaximumDepthForCreation", DATAINT, &cfg.MaximumDepthForCreation};
	configArray[6] = {(char*)"MaximumDepthForCrossover", DATAINT, &cfg.MaximumDepthForCrossover};
	configArray[7] = {(char*)"SelectionType", DATAINT, &cfg.SelectionType};
	configArray[8] = {(char*)"TournamentSize", DATAINT, &cfg.TournamentSize};
	configArray[9] = {(char*)"DemeticGrouping", DATAINT, &cfg.DemeticGrouping};
	configArray[10] = {(char*)"DemeSize", DATAINT, &cfg.DemeSize};
	configArray[11] = {(char*)"DemeticMigProbability", DATADOUBLE, &cfg.DemeticMigProbability};
	configArray[12] = {(char*)"SwapMutationProbability", DATADOUBLE, &cfg.SwapMutationProbability};
	configArray[13] = {(char*)"ShrinkMutationProbability", DATADOUBLE, &cfg.ShrinkMutationProbability};
	configArray[14] = {(char*)"AddBestToNewPopulation", DATAINT, &cfg.AddBestToNewPopulation};
	configArray[15] = {(char*)"SteadyState", DATAINT, &cfg.SteadyState};
	configArray[16] = {(char*)"SubSetSize", DATAINT, &cfg.SubSetSize};
	configArray[17] = {(char*)"ActiveGPMode", DATAINT, &cfg.ActiveGPMode};
	configArray[18] = {(char*)"GenToChangeTrain", DATAINT, &cfg.GenToChangeTrain};
	configArray[19] = {(char*)"", DATAINT, NULL};
*/
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

    pop->NthMyGP(pop->bestOfPopulation)->evaluate();
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

	int termCredibilityOffset = stats->getUsingTermCredibility() ? 1 : 0;

	adfNs.reserveSpace (termCredibilityOffset + stats->getNumberOfGraphs());

	cout<<" termCredibilityOffset = " << termCredibilityOffset <<endl;
	cout<<" stats->getNumberOfGraphs() = "<< stats->getNumberOfGraphs()<<endl;


	GPNodeSet **ns;
	ns = new GPNodeSet * [ termCredibilityOffset  + stats->getNumberOfGraphs() ];

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

	for(int i =  termCredibilityOffset; i < (stats->getNumberOfGraphs() +  termCredibilityOffset); i++){
		ns[i] = new GPNodeSet (22);
		adfNs.put (i, *ns[i]);

		ns[i]->putNode (*new GPNode (SUM, (char*) "+", 2));
		ns[i]->putNode (*new GPNode (SUB, (char*) "-", 2));
		ns[i]->putNode (*new GPNode (MULT, (char*) "*", 2));
		ns[i]->putNode (*new GPNode (DIV, (char*) "%", 2));
		ns[i]->putNode (*new GPNode (POW, (char*) "Pow", 2));
		ns[i]->putNode (*new GPNode (LOG, (char*) "Log", 2));  //Six operators

		ns[i]->putNode(*new GPNode (NEIGHBORHOOD1, (char*) "Neighborhood1(d,c)"));
		ns[i]->putNode(*new GPNode (NEIGHBORHOOD2, (char*) "Neighborhood2(d,c)"));
		ns[i]->putNode(*new GPNode (NEIGHBORHOOD3, (char*) "Neighborhood3(d,c)"));
		ns[i]->putNode(*new GPNode (HUBSCORE, (char*) "HubScore(d,c)"));
		ns[i]->putNode(*new GPNode (AUTHORITY, (char*) "Authority(d,c)"));
        ns[i]->putNode(*new GPNode (EIGENVECTOR, (char*) "EigenVector(d,c)"));
		ns[i]->putNode(*new GPNode (CLOSENESS, (char*) "Closeness(d,c)")); 
		ns[i]->putNode(*new GPNode (STRENGTH, (char*) "Strength(d,c)"));
		ns[i]->putNode(*new GPNode (CONSTRAINT, (char*) "Constraint(d,c)"));
		ns[i]->putNode(*new GPNode (PAGERANK, (char*) "PageRank(d,c)"));
		ns[i]->putNode(*new GPNode (BETWEENNESS, (char*) "Betweenness(d,c)"));
		ns[i]->putNode(*new GPNode (BIBCOUPLING, (char*) "BibCoupling (d,c)"));
		ns[i]->putNode(*new GPNode (COCITATION, (char*) "CoCitation(d,c)"));
		ns[i]->putNode(*new GPNode (JACCARDSIMILARITY, (char*) "JaccardSimilarity(d,c)"));
		ns[i]->putNode(*new GPNode (DICESIMILARITY, (char*) "DiceSimilarity(d,c)"));
		ns[i]->putNode(*new GPNode (INVERSELOGSIMILARITY, (char*) "InverseLigSimilarity(d,c)")); //16 terms
//NAO USAR!  ns[i]->putNode(*new GPNode (AVGNEIGHBORHOODDEGREE, (char*) "AvgNeighborHoodDegree(d,c)"));
	
    }

	delete []ns;
}


