#ifndef __NAIVEBAYES__H__
#define __NAIVEBAYES__H__

#include "Util.h"
#include "ICredibilityClassifier.h"
#include "Statistics.h"
#include "NormalEstimator.h"

using namespace std;

class NaiveBayes : public ICredibilityClassifier
{
	
	private:
		//Denominador da equacao, usando esse mapa para economizar processamento
		map<string, double> sumTF;
		map<string, double> contentCredibility;
		bool usingContentCredibility;
		
		vector< map<string, double> > graphsCredibility;
		vector<string> graphsNames;
		bool usingGraphCredibility;

        //both are used for numerical values
        bool usingNormalEstimator;
        map< int, map< string, double > > means;
        map< int, map< string, double > > deviations;
   

        map< int, map< string, NormalEstimator> > nEstimator;

        //confusion matrix
        map<string, map< string, int> > confusionMatrix;
        
        //prediction table: prediction [exampleId] -> guess
        class guess{
            public:
                string correctClass;
                string predictedClass;
                guess(string corr, string pred): correctClass(corr), predictedClass(pred) {}
                guess(){}
                virtual ~guess(){}
        };

        map<string, guess> prediction;
        void savePrediction(string exampleId, string correctClass, string predictedClass);
        bool printPrediction;

		double getContentCredibility(string term, string docClass);
		double getGraphCredibility(int,string,string);
        double getCategoricalCredibility(int i, string token, string classId);

		//Statistical object
		Statistics* stats;

		//Intern use inside NB apriori
		double computeApriori(string classId);

		//Conditional Part
		double computeConditionalDenominator(string classId);
		double computeConditionalNumerator(string termId, string classId);

		//Normalizator
		double Normalizer;

		//Statistics
		void calculateF1(map<string, unsigned long long> hits, map<string, unsigned long long> misses, map<string, unsigned long long> docsPerClass, map<string, unsigned long long> mappedDocs);
		double microF1;
		double macroF1;
        
        //To numerical values    
        double gaussianDistribution(double value, double mean, double deviation);

        void computeConfusionMatrix(string actual, string predicted);
        void showConfusionMatrix();

        double weibullDistribution(double value, double gamma, int k);
        double exponentialDistribution(double value, double gamma);
	public:
		NaiveBayes(Statistics* st);
		virtual ~NaiveBayes();
		
		void train(Examples& exs);		
		void test(Examples& exs);

		//TODO: pensar se nao eh melhor passar por ponteiro (espaco, rapidez x erros)
		void setContentCredibilityMap(map<string, double>& contentCredibility);
		void setGraphCredibilityMaps(vector< map<string, double> >& graphCredibility);
		
		void dontUseContentCredibility();
		void setCredibilityConfigurations(bool usingTermCredibility, std::vector<std::string> graphsNames);
		void useContentCredibility(bool);
	
        void useNormalEstimator(bool);

        void setPrintPrediction(bool);
        void showPredictions();

		double getMicroF1();
		double getMacroF1();

        void printFinalOutFile(ostream& out, string msg);
};

#endif

