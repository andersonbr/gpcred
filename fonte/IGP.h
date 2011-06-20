#ifndef __IGP_H__
#define __IGP_H__

class IGP{

	protected:
		int genCounter;
		int maxGen;
		int seed;
		std::string saveFileName, loadFileName;
	
	public:
		IGP(int seed): seed(seed) {};
		virtual ~IGP() = 0;

		virtual void createInitialPopulation();
		virtual	void evolve();

		int getActualGen();
		int getMaxGen();
		
		virtual double getBestFitness() = 0;
		virtual void print() = 0;

		virtual void loadPop() = 0;
		virtual void savePop() = 0;
		void setLoadPopFileName(std::string);
		void setSavePopFileName(std::string);
        
        virtual void performeFinalTest() = 0;
        virtual void printFinalOutFile() = 0;
}; 

#endif
