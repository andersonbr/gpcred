#ifndef __IGRAPH__H__
#define __IGRAPH__H__

#include "Graph.h"
#include "Examples.h"
#include "GraphInterface.h"

#define IGRAPH_LIB

#ifdef IGRAPH_LIB
#include <igraph.h>
#endif


class IGraph: public GraphInterface{

	private:

#ifdef IGRAPH_LIB
		igraph_t igraph;
		igraph_vector_t iedges;

		igraph_vector_t hubscore;
		igraph_vector_t authority;
		igraph_vector_t eigenvector_centrality;
#endif
        map<string,int> converter;
		
        double hubscore_normalizer;
		double authority_normalizer;
		double eigenVectorCentrality_normalizer;
		double closeness_normalizer;
		double strength_normalizer;
		double constraint_normalizer;
		double pageRank_normalizer;
		double betweenness_normalizer; 

	public:
		IGraph(Graph* g);
		virtual ~IGraph();
		int getUniqueId(string id);
		
		int getNeighborhoodSize1(string id);
		int getNeighborhoodSize2(string id);
		int getNeighborhoodSize3(string id);
		int getBibCoupling(string id);
		int getCoCitation(string id);
		
		double getCloseness(string id);
		double getStrength(string id);
		double getConstraint(string id);
		double getPageRank(string id);
		double getBetweenness(string id);
		double getJaccardSimilarity(string id);
		double getDiceSimilarity(string id);
		double getInverseLogSimilarity(string id);
		double getAvgNearstNeighborDegree(string id);
	
		double getHubScore(string id);
		void calculateHubScore();
		
		double getAuthority(string id);
		void calculateAuthority();
		
		double getEigenVectorCentrality(string id);
		void calculateEigenVectorCentrality();
};

#endif

