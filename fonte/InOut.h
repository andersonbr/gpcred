#ifndef __INOUT__H__
#define __INOUT__H__


#include <iostream>
#include <vector>
#include <cstdlib>
#include <iomanip>
#include <fstream>

#include "IInOut.h"
#include "Examples.h"

using namespace std;


class InOut : public IInOut
{
	private:
		Examples trainSet;
//		Examples validationSet;
		Examples testSet;
//      bool _usingValidationSet;

        string GPParameterConfigFileName;
		string basename;

		ofstream bestFile;
		ofstream histFile;
    
        ofstream brunoroFile;
        bool _usingBrunoroFile;

        ofstream predictionsFile;
        bool _usingPredictionsFile;

        ofstream evaluateFile;
        bool _usingEvaluateFile;

		void read(const char* filename, int option);
		enum{ train_option, validation_option, test_option };
    
        int numericalCollums;

	public:
		InOut(string basename);
	    ~InOut();

        void clear();

		void readTrain(const char* filename);
//		void readValidation(const char* filename);
		void readTest(const char* filename);
        
//      bool usingValidationSet();
//      void setUsingValidationSet(bool val);

        void setNumericalCollums(int nc);
        int getNumericalCollums(int nc);

		ofstream& getHistFile();
		ofstream& getBestFile();

        bool isUsingBrunoroFile();
        void setBrunoroFile(string brunoroFilename);
        ofstream& getBrunoroFile();

        bool isUsingPredictionsFile();
        void setPredictionsFile(string predictionsFilename);
        ofstream& getPredictionsFile();

        bool isUsingEvaluateFile();
        void setEvaluateFile(string evaluateFilename);
        ofstream& getEvaluateFile();


		Examples& getTrain();
//		Examples& getValidation();
		Examples& getTest();

        string getGPParameterConfigFileName();
        void setGPParameterConfigFileName(string fileName);
};

#endif

