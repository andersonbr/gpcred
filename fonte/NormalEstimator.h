
#ifndef __NORMAL_ESTIMATOR__H__
#define __NORMAL_ESTIMATOR__H__

class NormalEstimator{

    private:

        double round(double data);
        
        /** The sum of the weights */
        double m_SumOfWeights;

        /** The sum of the values seen */
        double m_SumOfValues;

        /** The sum of the values squared */
        double m_SumOfValuesSq;

        /** The current mean */
        double m_Mean;

        /** The current standard deviation */
        double m_StandardDev;
        
        /** The precision of numeric values ( = minimum std dev permitted) */
        double m_Precision;

       
        double normalProbability(double a);
        double p1evl( double x, double coef[], int N );
        double polevl( double x, double coef[], int N );
        double errorFunction(double x);
        double errorFunctionComplemented(double a); 

        static double MAXLOG;
        static double SQRTH;
    
    public:
        NormalEstimator(){}
        NormalEstimator(double precision);
        void addValue(double data, double weight);
        double getProbability(double data);
        double getMean();
        double getStdDev();
        double getPrecision();
        double getSumOfWeights();
};

#endif

