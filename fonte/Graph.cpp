#include "Graph.h"

void Graph::getAllEdgesIGraphFormat(vector<string>& edges, vector<double>& weights){

	for(map<string, map<string,double> >::iterator it = structure.begin(); it!= structure.end(); it++){
		for(map<string,double>::iterator msd = structure[it->first].begin(); msd != structure[it->first].end(); msd++){
			edges.push_back(it->first);
			edges.push_back(msd->first);
			weights.push_back(msd->second);
		}
	}
}

void Graph::addEdge(string v1, string v2, double value=1.0){
	structure[v1][v2] = value;
}

bool Graph::hasEdge(string v1, string v2){
	if( structure.count(v1) > 0 && structure[v1].count(v2) > 0){
		return true;
	}
	return false;
}

double Graph::getEdge(string v1, string v2){
	if(hasEdge(v1,v2))
		return structure[v1][v2];
	else
		return EMPTY;
}

int Graph::countEdges(){
	return structure.size();
}

