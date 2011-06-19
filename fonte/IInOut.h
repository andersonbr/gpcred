

#include<iostream>

using namespace std;

class IInOut
{
	public:
		virtual ~IInOut() {}
		virtual void readTrain(const char* filename) = 0;
//		virtual void readValidation(const char* filename) = 0;
		virtual void readTest(const char* filename) = 0;
};


