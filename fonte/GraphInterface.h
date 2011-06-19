#ifndef __GRAPH_INTERFACE__H__
#define __GRAPH_INTERFACE__H__

class GraphInterface{

	private:

	public:
		GraphInterface(){}
		virtual ~GraphInterface(){}

		virtual int getNeighborhoodSize1(string id) = 0;
		virtual int getNeighborhoodSize2(string id) = 0;
		virtual int getNeighborhoodSize3(string id) = 0;
		virtual int getBibCoupling(string id) = 0;
		virtual int getCoCitation(string id) = 0;
		
		virtual double getCloseness(string id) = 0;
		virtual double getStrength(string id) = 0;
		virtual double getConstraint(string id) = 0;
		virtual double getPageRank(string id) = 0;
		virtual double getBetweenness(string id) = 0;
		virtual double getJaccardSimilarity(string id) = 0;
		virtual double getDiceSimilarity(string id) = 0;
		virtual double getInverseLogSimilarity(string id) = 0;
		virtual double getAvgNearstNeighborDegree(string id) = 0;
		
		virtual void calculateHubScore() = 0;
		virtual double getHubScore(string id) = 0;

		virtual double getAuthority(string id) = 0;
		virtual void calculateAuthority() = 0;
		
		virtual double getEigenVectorCentrality(string id) = 0;
		virtual void calculateEigenVectorCentrality() = 0;
};



#endif


