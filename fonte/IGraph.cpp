#include "IGraph.h"

#include <vector>

IGraph::IGraph(Graph* g): hubscore_normalizer(1.0), authority_normalizer(1.0), eigenVectorCentrality_normalizer(1.0), closeness_normalizer(1.0), strength_normalizer(1.0), 
			 constraint_normalizer(1.0), pageRank_normalizer(1.0), betweenness_normalizer(1.0) {
#ifdef IGRAPH_LIB
    if(!g) return;

	int counter = 0;
	
	vector<string> edgesGraph;
	vector<double> valuesGraph;

	g->getAllEdgesIGraphFormat(edgesGraph, valuesGraph);
	
	igraph_vector_t edges;
	igraph_vector_init(&edges, edgesGraph.size());
	igraph_vector_init(&iedges, valuesGraph.size());

	//get one by one of edges, transform it into a string and save this value.
	for(unsigned int i = 0 ; i < edgesGraph.size() ; i++){
		int node;
		if ( converter.count(edgesGraph[i]) ){
			node = converter[edgesGraph[i]];
		}
		else{
			node = counter++;
			converter[edgesGraph[i]] = node;
		}
		VECTOR(edges)[i] = node; 
	}

	for(unsigned int i = 0; i < valuesGraph.size(); i++){
		VECTOR(iedges)[i] = (int) valuesGraph[i];
	}

	igraph_create(&igraph, &edges, 0, IGRAPH_DIRECTED);

	igraph_vector_destroy(&edges);
//	igraph_create(&igraph, &edges, 0, IGRAPH_UNDIRECTED);


	igraph_vector_init(&hubscore, 0);
	igraph_vector_init(&authority, 0);
	igraph_vector_init(&eigenvector_centrality, 0);

    /*  Errors 
     *  Ok, sometimes the best thing to do is ignore!
     *  Waiting for version 0.6.... TODO: comment and test this program on libigraph0.6
     * */
    igraph_set_error_handler(igraph_error_handler_ignore);

#endif
}

IGraph::~IGraph(){

#ifdef IGRAPH_LIB
	igraph_destroy(&igraph);
	igraph_vector_destroy(&hubscore);
	igraph_vector_destroy(&authority);
	igraph_vector_destroy(&eigenvector_centrality);

	igraph_vector_destroy(&iedges);
#endif
}

int IGraph::getUniqueId(string id){
	if(converter.count(id))
		return converter[id];
	return -1;
}

int IGraph::getNeighborhoodSize1(string id){

    int result = 0;
 	int uniqueId = getUniqueId(id);

	//TODO: Should i return -1? 0 is never a good idea!
	if( uniqueId == -1) return 0;
	
#ifdef IGRAPH_LIB
//#define VECTOR_PTR(v) ((igraph_vector_t**)(v).stor_begin)

	//Aqui faz diferenca o IGRAPH_IN e o IGRAPH_OUT! Interessante!
	igraph_vector_t resultVector;
	igraph_vector_init(&resultVector, 1);

	igraph_neighborhood_size(&igraph, &resultVector, igraph_vss_1( uniqueId ), 1, IGRAPH_ALL);
	
	//igraph_vector_ptr_t resultPtr;
	//igraph_vector_ptr_init(&resultPtr, 0);
	
	//igraph_neighborhood(&igraph, &resultPtr, igraph_vss_1( uniqueId ), 1, IGRAPH_IN);

	//cout<< "id = " << id << " -- " <<  VECTOR_PTR(resultPtr)[0] << endl;
	//cout<< "id = " << id << " -- " <<  VECTOR(resultVector)[0] << endl;
	result = int(VECTOR(resultVector)[0]);


	igraph_vector_destroy(&resultVector);
	//igraph_vector_destroy(&resultPtr);
#endif	
	return result;

}

int IGraph::getNeighborhoodSize2(string id){

 	int uniqueId = getUniqueId(id);
    int result = 0;
	//TODO: Should i return -1? 0 is never a good idea!
	if( uniqueId == -1) return 0;
	
#ifdef IGRAPH_LIB
	igraph_vector_t resultVector;
	igraph_vector_init(&resultVector, 1);

	igraph_neighborhood_size(&igraph, &resultVector, igraph_vss_1( uniqueId ), 2, IGRAPH_ALL);
	result = int(VECTOR(resultVector)[0]);
	igraph_vector_destroy(&resultVector);
#endif
	return result;
}

int IGraph::getNeighborhoodSize3(string id){

 	int uniqueId = getUniqueId(id);

	//TODO: Should i return -1? 0 is never a good idea!
	if( uniqueId == -1) return 0;
    int result = 0;

#ifdef IGRAPH_LIB
	igraph_vector_t resultVector;
	igraph_vector_init(&resultVector, 1);

	igraph_neighborhood_size(&igraph, &resultVector, igraph_vss_1( uniqueId ), 1, IGRAPH_ALL);
	result = int(VECTOR(resultVector)[0]);
	igraph_vector_destroy(&resultVector);
#endif	
	return result;
}


void IGraph::calculateHubScore(){
	
//	cout<<"computing hubscore"<<endl;

#ifdef IGRAPH_LIB
    igraph_arpack_options_t options;
	igraph_arpack_options_init(&options);
	
	igraph_hub_score(&igraph, &hubscore, NULL, 1, &options);  // http://igraph.sourceforge.net/doc/html/igraph_hub_score.html
#endif
//	cout<<"hub_score calculated"<<endl;
}

double IGraph::getHubScore(string id){

 	int uniqueId = getUniqueId(id);

	//TODO: Should i return -1? 0 is never a good idea!
	if( uniqueId == -1) return 0;
#ifdef IGRAPH_LIB
	return (VECTOR(hubscore)[uniqueId] * hubscore_normalizer);
#else
    return 0.0;
#endif
}

void IGraph::calculateAuthority(){

//	cout<<"computing authority"<<endl;
	
#ifdef IGRAPH_LIB
	igraph_arpack_options_t options;
	igraph_arpack_options_init(&options);
	igraph_authority_score(&igraph, &authority, NULL, 1, &options);  // http://igraph.sourceforge.net/doc/html/igraph_authority_score.html
#endif	
//	cout<<"authority calculated"<<endl;
}

double IGraph::getAuthority(string id){

	int uniqueId = getUniqueId(id);

	//TODO: Should i return -1? 0 is never a good idea!
	if( uniqueId == -1) return 0;
	
#ifdef IGRAPH_LIB
	return (VECTOR(authority)[uniqueId] * authority_normalizer);
#else
    return 0.0;
#endif
}
		
double IGraph::getEigenVectorCentrality(string id){

	int uniqueId = getUniqueId(id);

	//TODO: Should i return -1? 0 is never a good idea!
	if( uniqueId == -1) return 0;
	
#ifdef IGRAPH_LIB
	return (VECTOR(eigenvector_centrality)[uniqueId] * eigenVectorCentrality_normalizer);
#else
    return 0.0;
#endif
}

void IGraph::calculateEigenVectorCentrality(){

//	cout<<"computing eigenvector cantrality"<<endl;
	
#ifdef IGRAPH_LIB
	igraph_arpack_options_t options;
	//TODO enviar corretamente o peso das arestas
	
	igraph_arpack_options_init(&options);
	igraph_eigenvector_centrality(&igraph, &eigenvector_centrality, NULL, 1, &iedges, &options);  // http://igraph.sourceforge.net/doc/html/igraph_eigenvector_centrality.html
#endif
//	cout<<"eigenvector centrality calculated"<<endl;
}


double IGraph::getCloseness(string id){

 	int uniqueId = getUniqueId(id);

	//TODO: Should i return -1? 0 is never a good idea!
	if( uniqueId == -1) return 0;
	double result = 0.0;

#ifdef IGRAPH_LIB
	igraph_vector_t resultVector;
	igraph_vector_init(&resultVector, 1);

	igraph_closeness(&igraph, &resultVector, igraph_vss_1( uniqueId ), IGRAPH_ALL);
	result = VECTOR(resultVector)[0] * closeness_normalizer;
	igraph_vector_destroy(&resultVector);
#endif
	return result;
}

double IGraph::getStrength(string id){

 	int uniqueId = getUniqueId(id);

	//TODO: Should i return -1? 0 is never a good idea!
	if( uniqueId == -1) return 0;
	double result = 0.0;

#ifdef IGRAPH_LIB
	igraph_vector_t resultVector;
	igraph_vector_init(&resultVector, 1);

	igraph_strength(&igraph, &resultVector, igraph_vss_1( uniqueId ), IGRAPH_ALL, 1, &iedges);
	result = VECTOR(resultVector)[0] * strength_normalizer; 
	igraph_vector_destroy(&resultVector);
#endif
	return result;
}

double IGraph::getConstraint(string id){

 	int uniqueId = getUniqueId(id);

	//TODO: Should i return -1? 0 is never a good idea!
	if( uniqueId == -1) return 0;
	double result = 0.0;

#ifdef IGRAPH_LIB
	igraph_vector_t resultVector;
	igraph_vector_init(&resultVector, 1);

	igraph_constraint(&igraph, &resultVector, igraph_vss_1( uniqueId ), &iedges);
	result = VECTOR(resultVector)[0] * constraint_normalizer;
	igraph_vector_destroy(&resultVector);
#endif
	return result;
}

double IGraph::getPageRank(string id){

	int uniqueId = getUniqueId(id);

	//TODO: Should i return -1? 0 is never a good idea!
	if( uniqueId == -1) return 0;
    double result = 0.0;

#ifdef IGRAPH_LIB
	igraph_vector_t resultVector;
	igraph_vector_init(&resultVector, 1);

	igraph_arpack_options_t options;
	igraph_arpack_options_init(&options);

	igraph_pagerank(&igraph, &resultVector, NULL, igraph_vss_1( uniqueId ), IGRAPH_DIRECTED, 1, &iedges, &options); // http://igraph.sourceforge.net/doc/html/igraph_pagerank.html
	result = VECTOR(resultVector)[0] * pageRank_normalizer;
	igraph_vector_destroy(&resultVector);
	
#endif
	return result;
}


double IGraph::getBetweenness(string id){

 	int uniqueId = getUniqueId(id);

	//TODO: Should i return -1? 0 is never a good idea!
	if( uniqueId == -1) return 0;
	double result = 0.0;

#ifdef IGRAPH_LIB
	igraph_vector_t resultVector;
	igraph_vector_init(&resultVector, 1);

	igraph_betweenness(&igraph, &resultVector, igraph_vss_1( uniqueId ), IGRAPH_DIRECTED); // http://igraph.sourceforge.net/doc/html/igraph_betweenness.html
	result = VECTOR(resultVector)[0] * betweenness_normalizer;
	igraph_vector_destroy(&resultVector);
#endif
	return result;
}


int IGraph::getBibCoupling(string id){

	int uniqueId = getUniqueId(id);

	//TODO: Should i return -1? 0 is never a good idea!
	if( uniqueId == -1) return 0;
	int result = 0.0;

#ifdef IGRAPH_LIB
	igraph_matrix_t resultMatrix;
	igraph_matrix_init(&resultMatrix, 0, 0);

	igraph_bibcoupling(&igraph, &resultMatrix, igraph_vss_1( uniqueId )); // http://igraph.sourceforge.net/doc/html/igraph_bibcoupling.html
		
	for(int i = 0 ; i < igraph_matrix_nrow(&resultMatrix); i++){
		for(int j = 0 ; j < igraph_matrix_ncol(&resultMatrix); j++){
			result += int(MATRIX(resultMatrix,i,j));
		}
	}
	igraph_matrix_destroy(&resultMatrix);	
#endif
	return result;
}

int IGraph::getCoCitation(string id){

	int uniqueId = getUniqueId(id);

	//TODO: Should i return -1? 0 is never a good idea!
	if( uniqueId == -1) return 0;
	int result = 0.0;

#ifdef IGRAPH_LIB
	igraph_matrix_t resultMatrix;
	igraph_matrix_init(&resultMatrix, 0, 0);

	igraph_cocitation(&igraph, &resultMatrix, igraph_vss_1( uniqueId )); // http://igraph.sourceforge.net/doc/html/igraph_cocitation.html
		
	for(int i = 0 ; i < igraph_matrix_nrow(&resultMatrix); i++){
		for(int j = 0 ; j < igraph_matrix_ncol(&resultMatrix); j++){
			result += int(MATRIX(resultMatrix,i,j));
		}
	}
	
	igraph_matrix_destroy(&resultMatrix);	
#endif
	return result;
}


double IGraph::getJaccardSimilarity(string id){

	int uniqueId = getUniqueId(id);

	//TODO: Should i return -1? 0 is never a good idea!
	if( uniqueId == -1) return 0;
	double result = 0.0;

#ifdef IGRAPH_LIB
	igraph_matrix_t resultMatrix;
	igraph_matrix_init(&resultMatrix, 0, 0);

	igraph_similarity_jaccard(&igraph, &resultMatrix, igraph_vss_1( uniqueId ), IGRAPH_ALL, true); // http://igraph.sourceforge.net/doc/html/igraph_similarity_jaccard.html
		
	for(int i = 0 ; i < igraph_matrix_nrow(&resultMatrix); i++){
		for(int j = 0 ; j < igraph_matrix_ncol(&resultMatrix); j++){
			result += MATRIX(resultMatrix,i,j);
		}
	}
	igraph_matrix_destroy(&resultMatrix);	
#endif
	return result;
}

double IGraph::getDiceSimilarity(string id){

	int uniqueId = getUniqueId(id);

	//TODO: Should i return -1? 0 is never a good idea!
	if( uniqueId == -1) return 0;
	double result = 0.0;

#ifdef IGRAPH_LIB
	igraph_matrix_t resultMatrix;
	igraph_matrix_init(&resultMatrix, 0, 0);

	igraph_similarity_dice(&igraph, &resultMatrix, igraph_vss_1( uniqueId ), IGRAPH_ALL, true); // http://igraph.sourceforge.net/doc/html/igraph_similarity_jaccard.html
		
	for(int i = 0 ; i < igraph_matrix_nrow(&resultMatrix); i++){
		for(int j = 0 ; j < igraph_matrix_ncol(&resultMatrix); j++){
			result += MATRIX(resultMatrix,i,j);
		}
	}
	igraph_matrix_destroy(&resultMatrix);	
#endif
	return result;
}

double IGraph::getInverseLogSimilarity(string id){

	int uniqueId = getUniqueId(id);

	//TODO: Should i return -1? 0 is never a good idea!
	if( uniqueId == -1) return 0;
    double result = 0.0;

#ifdef IGRAPH_LIB
	igraph_matrix_t resultMatrix;
	igraph_matrix_init(&resultMatrix, 0, 0);

	igraph_similarity_inverse_log_weighted(&igraph, &resultMatrix, igraph_vss_1( uniqueId ), IGRAPH_ALL); // http://igraph.sourceforge.net/doc/html/igraph_similarity_inverse_log_weighted.html
		
	for(int i = 0 ; i < igraph_matrix_nrow(&resultMatrix); i++){
		for(int j = 0 ; j < igraph_matrix_ncol(&resultMatrix); j++){
			result += MATRIX(resultMatrix,i,j);
		}
	}
	
	igraph_matrix_destroy(&resultMatrix);	
#endif
	return result;
}

double IGraph::getAvgNearstNeighborDegree(string id){
/*
 *	I am not using this function because of this error:
 *	gp-cred: error.c:125: IGRAPH_FINALLY_REAL: Assertion `no<100' failed.
 *	
 *	No clues =/
 *
 */
/*
	int uniqueId = getUniqueId(id);

	//TODO: Should i return -1? 0 is never a good idea!
	if( uniqueId == -1) return 0;

	igraph_vector_t resultVector;
	igraph_vector_init(&resultVector, 1);

	igraph_avg_nearest_neighbor_degree(&igraph, igraph_vss_1( uniqueId ), NULL, &resultVector, &iedges); // http://igraph.sourceforge.net/doc/html/igraph_avg_nearest_neighbor_degree.html
	
	double result = VECTOR(resultVector)[0];

	if(isnan(result) 
	    result = 0.0;

	igraph_vector_destroy(&resultVector);	
	return result;
*/
    id = ""; //to avoid warnings
    return 0.0;
}

