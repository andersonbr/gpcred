#ifndef __GPCPP__H__
#define __GPCPP__H__

#include "IGP.h"
#include "gp.h"
#include "gpconfig.h"
#include "MyGP.h"
#include "InOut.h"
#include "Statistics.h"

class gpcpp : public IGP{
                     
	private:
		GPAdfNodeSet adfNs;
		MyPopulation *pop;
		GPVariables cfg;
		Statistics* stats;
		InOut* io;

		//int numberOfGraphs;
		void createNodeSet();
	
	public:
		gpcpp(Statistics* stats, InOut* io, int seed);
		virtual ~gpcpp();
		
		double getBestFitness();
		void evolve();
		void createInitialPopulation();
        void performeFinalTest();

		void print();
		void loadPop();
		void savePop();

};

#endif

