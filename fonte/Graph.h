#ifndef __GRAPH__H__
#define __GRAPH__H__

#include<iostream>
#include<map>
#include<vector>

// Graph Implementation
// Vertexes are Strings and edges are double values
using namespace std;

class Graph{

	private:
		map<string, map<string, double> > structure;
		double EMPTY;

	public:
		Graph() : EMPTY(0.0){}
		virtual ~Graph(){}
		void addEdge(string v1, string v2, double v);
		bool hasEdge(string v1, string v2);
		double getEdge(string v1, string v2);
		int countEdges();
		void getAllEdgesIGraphFormat(vector<string>& edges, vector<double>& weights);
};


#endif
