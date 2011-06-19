
#include "Statistics.h"
#include "InOut.h"

#include<stack>

class TreeEvaluator{
    
    private:
        Statistics* stats;
        InOut* io;
        
        bool isOperand(string val);
        bool isOperator(string val);
        bool has(int limite, const char *word, ...);

        double getOperandValue(string operand, string exampleId, string classId);
        double getResult(string operatorID, std::stack<double>& values);
    public:

        TreeEvaluator(Statistics* stats, InOut* io);
        ~TreeEvaluator();

        void evaluateFromFile(string fileName);
};

