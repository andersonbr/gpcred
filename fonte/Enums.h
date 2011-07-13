
#ifndef __ENUMS_H__
#define __ENUMS_H__

enum termMetrics{ 
		DF_PER_TERM = 0,
		DF_PER_CLASS,
		TF_PER_TERM,
		TF_PER_CLASS,
		AM,
		MAX_AM,
		DOMINANCE,
		PROB_COND,
		PROB_COND_NEG,
		GINI,
		IG,
		OR,
        IDF,
		TFIDF,
		TFICF,
		CE,
		CTD,
		GSS,
		CHI,
		CC,
		MAX_IG,
		MAX_DOM,
		MAX_OR,
		MAX_CTD,
		MAX_GSS,
		MAX_CHI,
		MAX_CC,
		MAX_DOMINANCE,
		MAX_TFIDF,
		MAX_TFICF,
		SUM_TF_PER_CLASS,
		
        //Graph Metrics
		NEIGHBORHOOD1, 
		NEIGHBORHOOD2, 
		NEIGHBORHOOD3, 
		HUBSCORE,
		AUTHORITY,
		EIGENVECTOR,
		CLOSENESS,
        STRENGTH,
        CONSTRAINT,
		PAGERANK,
        BETWEENNESS,
        BIBCOUPLING,
        COCITATION,
        JACCARDSIMILARITY,
        DICESIMILARITY,
        INVERSELOGSIMILARITY,
        AVGNEIGHBORHOODDEGREE,

        NUM1,
        NUM2,
        NUM3

};

enum operators{
		SUM = 10000,
		DIV, 
		MULT,
		SUB ,
		LOG,
		POW
};

#endif

