

#ifndef __ICREDIBILITY__CLASSIFIER__H__
#define __ICREDIBILITY__CLASSIFIER__H__

#include "Examples.h"
#include "Statistics.h"
#include "IClassifier.h"

class ICredibilityClassifier : public IClassifier
{
		
	public:
		virtual ~ICredibilityClassifier() {}
	    
        virtual void useContentCredibility(bool) = 0;
		
        virtual void setContentCredibilityMap(map<string, double>& contentCredibility) = 0;
		virtual void setGraphCredibilityMaps(vector< map<string, double> >& graphCredibility) = 0;
        virtual void cleanStaticFields() =0;
};

#endif

