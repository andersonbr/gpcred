#ifndef __STATISTICS__H__
#define __STATISTICS__H__

#include "Examples.h"
#include "Util.h"
#include "Matematica.h"
#include "Graph.h"
#include "Enums.h"
#include "IGraph.h"

#include<map>
#include<cmath>
#include<cstdlib>
#include <set>

using namespace std;

class Statistics {

    private:

        bool usingTermCredibility;

        int totalDocs;

        map< string, string > trainIdClass;
        set<string> vocabulary;
        set<string> classes;

        //It is GraphId -> Class -> Graph
        map< int , map<string, IGraph*> > iGraphs;
        int graphNumberCounter;

        double sumTF;

        map<string, double> sumDFperClass;
        map<string, double> sumTFperClass;
        map<string, double> DFperTerm; 
        map<string, double> DFperClass; 

        map<string, double> TFperClass; 
        map<string, double> TFperTerm; 

        map<string, double> IDF;

        map<string, double> TFIDF;
        map<string, double> MaxTFIDF;

        map<string, double> MaxTFICF;
        map<string, double> TFICF;

        map<string, double> AM;
        map<string, double> MaxAM;

        map<string, double> DOM;
        map<string, double> MaxDom;

        map<string, double> IG;
        map<string, double> MaxIG; 

        map<string, double> Gini;

        map<string, double> OR;
        map<string, double> MaxOR; 

        map<string, double> CE;
        map<string, set<string> > CFperTerm;

        map<string, double> CTD;
        map<string, double> MaxCTD; 

        map<string, double> GSS;
        map<string,double> MaxGSS;

        map<string, double> CHI;
        map<string, double> MaxCHI;

        map<string, double> CC;
        map<string, double> MaxCC; 

        // Graph Metrics --- Metric [ GID ] [ ID - CLASS ] = Value
        vector< map<string, int > > idClassNeighborhoodSize1;
        vector< map<string, int > > idClassNeighborhoodSize2;
        vector< map<string, int > > idClassNeighborhoodSize3;
        vector< map<string, double > > idClassHubScore;
        vector< map<string, double > > idClassAuthority;
        vector< map<string, double > > idClassEigenVectorCentrality;
        vector< map<string, double > > idClassCloseness;
        vector< map<string, double > > idClassStrength;
        vector< map<string, double > > idClassConstraint;
        vector< map<string, double > > idClassPageRank;
        vector< map<string, double > > idClassBetweenness;
        vector< map<string, int > > idClassBibCoupling;
        vector< map<string, int > > idClassCoCitation;
        vector< map<string, double > > idClassJaccardSimilarity;
        vector< map<string, double > > idClassDiceSimilarity;
        vector< map<string, double > > idClassInverseLogSimilarity;
        vector< map<string, double > > idClassAvgNearstNeighborDegree;

        bool normalEstimator;
        bool usingNomalizePerGreatestClassValue;
        bool optimizeGraphMetrics;

        double minmaxNormalization(double val, double minValue, double maxValue);
        double maxNormalization(double val, double maxValue);

        double getGraphValue( int metric, int graph, string id, string classid);

        //ok, it shouldnt be here...sorry software engs.
        bool usingKNN;
        int KNNK;
        bool usingKNNOptimize;

        //categorical values
        map< int, map < string, map< string, int> > > tupleValue;

    public:

        Statistics();
        virtual ~Statistics();

        //clean up everything!
        void clear();

        void setUsingKNN(bool usingKNN, int K, bool usingknnoptimize);
        bool getUsingKNN();
        bool getUsingKNNOptimize();

        int getCategoricalValue(int index, string classId, string token);
        int getCategoricalSize(int index, string classId);
        int getK();
        void setNormalEstimator(bool nEstimator){
            normalEstimator = nEstimator;
        }
        
        void setNormalizeTermsPerGreatestClassValue(bool norm){
            usingNomalizePerGreatestClassValue = norm;
        }

        bool getNormalEstimator(){
            return normalEstimator;
        }

        void calculateIDF();
        void retrieveContentMetrics();
        void setUsingTermCredibility(){
            usingTermCredibility = true;
        }

        bool getUsingTermCredibility(){
            return usingTermCredibility;
        }
        
        int getNumberOfGraphs(){
            return graphNumberCounter;
        }

        string getTrainClass(string exampleId){
            if(trainIdClass.find(exampleId) != trainIdClass.end())
                return trainIdClass[exampleId];
            else return "__NOT__FOUND__";
        }

        void retrieveGraphMetrics(Examples&);
        void retrieveGraphMetricsDebug(Examples&);
        void readExamples(Examples exs);
        void readGraph(string graphFileName);
        
        void setOptimizeGraphMetrics(bool optimize);
        
//TODO:Transformar funcoes abaixo de acordo com necessidade de otimizacao....

        double getNeighborhoodSize1(string id, string c, int graph)        {return ( optimizeGraphMetrics ?  getValue(idClassNeighborhoodSize1, graph, getCompIndex(id,c)) : getGraphValue( NEIGHBORHOOD1, graph, id, c) ); }

        double getNeighborhoodSize2(string id, string c, int graph)        {return ( optimizeGraphMetrics ?  getValue(idClassNeighborhoodSize2, graph, getCompIndex(id,c)): getGraphValue( NEIGHBORHOOD2, graph, id, c) ) ; }

        double getNeighborhoodSize3(string id, string c, int graph)        {return ( optimizeGraphMetrics ?  getValue(idClassNeighborhoodSize3, graph, getCompIndex(id,c)) : getGraphValue( NEIGHBORHOOD3, graph, id, c) ); }
        double getHubScore(string id, string c, int graph)                 {return ( optimizeGraphMetrics ?  getValue(idClassHubScore, graph, getCompIndex(id,c)): getGraphValue( HUBSCORE, graph, id, c) ); }
        double getAuthority(string id, string c, int graph)	               {return ( optimizeGraphMetrics ?  getValue(idClassAuthority, graph, getCompIndex(id,c)): getGraphValue( AUTHORITY, graph, id, c) );}
        double getEigenVectorCentrality(string id, string c, int graph)	   {return ( optimizeGraphMetrics ?  getValue(idClassEigenVectorCentrality, graph, getCompIndex(id,c)): getGraphValue( EIGENVECTOR, graph, id, c) );}
        double getCloseness(string id, string c, int graph)		           {return ( optimizeGraphMetrics ?  getValue(idClassCloseness, graph, getCompIndex(id,c)): getGraphValue( CLOSENESS, graph, id, c) );}
        double getStrength(string id, string c, int graph)		           {return ( optimizeGraphMetrics ?  getValue(idClassStrength, graph, getCompIndex(id,c)): getGraphValue( STRENGTH, graph, id, c) );}
        double getConstraint(string id, string c, int graph)		       {return ( optimizeGraphMetrics ?  getValue(idClassConstraint, graph, getCompIndex(id,c)): getGraphValue( CONSTRAINT, graph, id, c) );}
        double getPageRank(string id, string c, int graph)		           {return ( optimizeGraphMetrics ?  getValue(idClassPageRank, graph, getCompIndex(id,c)): getGraphValue( PAGERANK, graph, id, c) );}
        double getBetweenness(string id, string c, int graph)		       {return ( optimizeGraphMetrics ?  getValue(idClassBetweenness, graph, getCompIndex(id,c)): getGraphValue( BETWEENNESS, graph, id, c) );}
        int getBibCoupling(string id, string c, int graph)		           {return ( optimizeGraphMetrics ?  getValue(idClassBibCoupling, graph, getCompIndex(id,c)): getGraphValue( BIBCOUPLING, graph, id, c) );}
        int getCoCitation(string id, string c, int graph)		           {return ( optimizeGraphMetrics ?  getValue(idClassCoCitation, graph, getCompIndex(id,c)): getGraphValue( COCITATION, graph, id, c) );}
        double getJaccardSimilarity(string id, string c, int graph)	       {return ( optimizeGraphMetrics ?  getValue(idClassJaccardSimilarity, graph, getCompIndex(id,c)): getGraphValue( JACCARDSIMILARITY, graph, id, c) );}
        double getDiceSimilarity(string id, string c, int graph)	       {return ( optimizeGraphMetrics ?  getValue(idClassDiceSimilarity, graph, getCompIndex(id,c)): getGraphValue( DICESIMILARITY, graph, id, c) );}
        double getInverseLogSimilarity(string id, string c, int graph)	   {return ( optimizeGraphMetrics ?  getValue(idClassInverseLogSimilarity, graph, getCompIndex(id,c)): getGraphValue( INVERSELOGSIMILARITY, graph, id, c) );}
        double getAvgNearstNeighborDegree(string id, string c, int graph)  {return ( optimizeGraphMetrics ?  getValue(idClassAvgNearstNeighborDegree, graph, getCompIndex(id,c)): getGraphValue( AVGNEIGHBORHOODDEGREE, graph, id, c) );}

        set<string>& getClasses()					                       {return classes;}
        int getNumberOfClasses()                                           {return classes.size();}
        int getTotalDocs()						                           {return totalDocs;}
        int getVocabularySize()						                       {return vocabulary.size();}
        map<string,double>& getTFPerClass()				                   {return TFperClass;}
        map<string,double>& getSumDFPerClass()				               {return sumDFperClass;}

        set<string>& getVocabulary()					                   {return vocabulary;}

        double getSumDFperClass(string c)				                   {return getValue(sumDFperClass, c);}
        double getSumTF()						                           {return sumTF;}
        double getDFperTerm(string t)					                   {return getValue(DFperTerm, t);}
        double getDFperClass(string t, string c)			               {return getValue(DFperClass, getCompIndex(t,c));}
        double getTFperClass(string t, string c)			               {return getValue(TFperClass, getCompIndex(t,c));}
        double getSumTFperClass(string c)				                   {return getValue(sumTFperClass, c);}

        double getTFperTerm(string t)					                   {return getValue(TFperTerm, t);}

        double getIDF(string t)						                       {return getValue(IDF, t);}

        double getTFIDF(string t, string c)				                   {return getValue(TFIDF, getCompIndex(t,c));}
        double getMaxTFIDF(string t)					                   {return getValue(MaxTFIDF, t);}

        double getMaxTFICF(string t)					                   {return getValue(MaxTFICF, t);}
        double getTFICF(string t, string c)				                   {return getValue(TFICF, getCompIndex(t,c));}

        double getAM(string t, string c)				                   {return getValue(AM, getCompIndex(t,c));}
        double getMaxAM(string t)					                       {return getValue(MaxAM, t);}

        double getDominance(string t, string c)				               {return getValue(DOM, getCompIndex(t,c));}
        double getMaxDominance(string t)				                   {return getValue(MaxDom, t);}

        double getIG(string t, string c)				                   {return getValue(IG, getCompIndex(t,c));}
        double getMaxIG(string t)					                       {return getValue(MaxIG, t);}

        double getGini(string t)					                       {return getValue(Gini, t);}

        double getOR(string t, string c)				                   {return getValue(OR, getCompIndex(t,c));}
        double getMaxOR(string t)					                       {return getValue(MaxOR, t);}

        double getCE(string t)						                       {return getValue(CE, t);}

        double getCTD(string t, string c) 				                   {return getValue(CTD, getCompIndex(t,c));}
        double getMaxCTD(string t) 					                       {return getValue(MaxCTD, t);}

        double getGSS(string t, string c)  				                   {return getValue(GSS, getCompIndex(t,c));}
        double getMaxGSS(string t) 					                       {return getValue(MaxGSS, t);}

        double getCHI(string t, string c)  				                   {return getValue(CHI, getCompIndex(t,c));}
        double getMaxCHI(string t)                                         {return getValue(MaxCHI, t);}

        double getCC(string t, string c)                                   {return getValue(CC, getCompIndex(t,c));}
        double getMaxCC(string t)                                          {return getValue(MaxCC, t);}
};

#endif

