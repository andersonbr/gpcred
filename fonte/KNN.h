#ifndef __KNN__H__
#define __KNN__H__

#include "Util.h"
#include "ICredibilityClassifier.h"
#include "Statistics.h"
#include "NormalEstimator.h"
#include "Matematica.h"

using namespace std;

//a good reference:
//http://www.usenix.org/events/sec02/full_papers/liao/liao_html/node4.html

class docWeighted{
    public:
        string docId;
        double weight;
        docWeighted(string id, double w): docId(id), weight(w) {}
};

struct docWeightedCmp {
    bool operator() (const docWeighted& lhs, const docWeighted& rhs) const
    {return greaterThan(lhs.weight, rhs.weight) || ( equals(lhs.weight,rhs.weight) && lhs.docId > rhs.docId);}
//    {return lhs.docId > rhs.docId;}
};

class KNN : public ICredibilityClassifier
{
	private:
		//Denominador da equacao, usando esse mapa para economizar processamento
		map<string, set<docWeighted, docWeightedCmp> > termDocWset ;
        map<string, double> docTrainSizes;

		map<string, double> contentCredibility;
		bool usingContentCredibility;
		
		vector< map<string, double> > graphsCredibility;
		vector<string> graphsNames;
		bool usingGraphCredibility;

        //both are used for numerical values
        bool usingNormalEstimator;
        map< int, map< string, double > > means;
        map< int, map< string, double > > deviations;
        
        //used for numerical values
        map<string, vector<double> > exTrain;
    
        //confusion matrix
        map<string, map< string, int> > confusionMatrix;
        
        //optimizing...
        static map<string, map< string, double> > saveValues;
        static bool valuesSaved;
        bool usingKNNOptimize;

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

		//Statistical object
		Statistics* stats;

        double Normalizer;
		
        //Statistics
		void calculateF1(map<string, unsigned long long> hits, map<string, unsigned long long> misses, map<string, unsigned long long> docsPerClass, map<string, unsigned long long> mappedDocs);
		double microF1;
		double macroF1;
        
        void computeConfusionMatrix(string actual, string predicted);
        void showConfusionMatrix();

        int K;
        string getPredictedClass(set<docWeighted, docWeightedCmp>& trainExamples);

	public:
		KNN(Statistics* st, int K, bool optimize);
		virtual ~KNN();
		
		void train(Examples& exs);		
		void test(Examples& exs);

		//TODO: pensar se nao eh melhor passar por ponteiro (espaco, rapidez x erros)
		void setContentCredibilityMap(map<string, double>& contentCredibility);
		void setGraphCredibilityMaps(vector< map<string, double> >& graphCredibility);
		
		void dontUseContentCredibility();
		void setCredibilityConfigurations(bool usingTermCredibility, std::vector<std::string> graphsNames);
		void useContentCredibility(bool);

        void setPrintPrediction(bool);
        void showPredictions();

		double getMicroF1();
		double getMacroF1();

        void printFinalOutFile(ostream& out, string msg);
};

#endif

