#ifndef __ICLASSIFIER__H__
#define __ICLASSIFIER__H__

#include "Examples.h"
#include "Statistics.h"

class IClassifier
{
		
	public:
		virtual ~IClassifier() {}
		virtual void train(Examples& exs) = 0;
		virtual void test(Examples& exs) = 0;

		virtual double getMicroF1()= 0;
		virtual double getMacroF1() = 0;
		virtual void setCredibilityConfigurations(bool usingTermCredibility, std::vector<std::string> graphsNames) = 0;
        virtual void showConfusionMatrix() = 0;
        virtual void showPredictions() = 0;
        
        virtual void printFinalOutFile(ostream& out, string msg) =0;
};


#endif

