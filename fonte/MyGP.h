#ifndef __MY_GP_H__
#define __MY_GP_H__

#include "gp.h"
#include "gpconfig.h"
#include "Statistics.h"
#include "InOut.h"
#include "Enums.h"

#include <map>

// class identifiers
const int MyGeneID=GPUserID;
const int MyGPID=GPUserID+1;
const int MyPopulationID=GPUserID+2;

// forward declaration
class MyGP;
class MyGene;

// Inherit the three GP classes GPGene, GP and GPPopulation
class MyGene : public GPGene
{

private:
	Statistics* stats;

public:

  // Duplication (mandatory)
  MyGene (const MyGene& gpo, Statistics* stats) : GPGene (gpo), stats(stats){
  }
  virtual GPObject& duplicate () { return *(new MyGene(*this,stats)); }

  // Creation of own class objects (mandatory)
  MyGene (GPNode& gpo, Statistics* stats) : GPGene (gpo), stats(stats) {}
  virtual GPGene* createChild (GPNode& gpo) {
    return new MyGene (gpo,stats); }

  // Tree evaluation
  double evaluate ();
  //TODO: verificar pq nao consigo passar default parameter pelo .cpp
  double evaluate(MyGP &, string, string, int graphid = 0);

  // Load and save (not mandatory)
  MyGene (Statistics* stats) : stats(stats) {}
  virtual int isA () { return MyGeneID; }
  virtual GPObject* createObject() { return new MyGene(stats); }
  //virtual char* load (istream& is);
  //virtual void save (ostream& os);

  // Print (not mandatory)
  // virtual void printOn (ostream& os);

  // Access children (not mandatory)
  MyGene* NthMyChild (int n) {
    return (MyGene*) GPContainer::Nth (n); }
};

class MyGP : public GP
{
private:
	Statistics* stats;
	InOut* io;

public:
  static int gennum;
  static int indCounter;

  // Duplication (mandatory)
  MyGP (MyGP& gpo, Statistics* stats, InOut* io) : GP (gpo), stats (stats) , io(io) {
  }
  virtual GPObject& duplicate () { return *(new MyGP(*this,stats,io)); }

  // Creation of own clss objects (mandatory)
  MyGP (int genes, Statistics* stats, InOut* io) : GP (genes),stats(stats), io(io) {}
  ~MyGP(){
  }
  
  virtual GPGene* createGene (GPNode& gpo) {
    return new MyGene (gpo,stats); }

  // Tree evaluation (mandatory)
  virtual void evaluate(bool printFinalOutfile, std::string msg);

  double finalEvaluate() { return 0.0; }
  double finalEvaluate(std::map<int, std::map<string,double> >&)//versao especial para o comite
	{return 0.0;}
  // Print (not mandatory)
  // virtual void printOn (ostream& os);

  // Load and save (not mandatory)
  MyGP (Statistics* stats, InOut* io) : stats (stats) , io(io) {}
  virtual int isA () { return MyGPID; }
  virtual GPObject* createObject() { return new MyGP(stats,io); }
//  char* load (istream& is){
//  }
  //virtual void save (ostream& os);

  // Access trees (not mandatory)
  MyGene* NthMyGene (int n) {
    return (MyGene*) GPContainer::Nth (n); }
};

class MyPopulation : public GPPopulation
{
private:
	Statistics* stats;
	InOut* io;
	int indCounter;

public:
  // Constructor (mandatory)
  MyPopulation (GPVariables& GPVar_, GPAdfNodeSet& adfNs_, Statistics* stats, InOut* io) :
    GPPopulation (GPVar_, adfNs_), stats(stats), io(io) {
        
    }

  // Duplication (mandatory)
  MyPopulation (MyPopulation& gpo, Statistics* stats, InOut* io) : GPPopulation(gpo), stats(stats), io(io) {}
  virtual GPObject& duplicate () { return *(new MyPopulation(*this)); }

  // Creation of own class objects (mandatory)
  virtual GP* createGP (int numOfTrees) { return new MyGP (numOfTrees, stats, io); }

  // Load and save (not mandatory)
  MyPopulation (Statistics* stats, InOut* io) : stats(stats), io(io) {}
  virtual int isA () { return MyPopulationID; }
  virtual GPObject* createObject() { return new MyPopulation(stats,io); }
  //char* load (istream& is){
  //GPPopulation::load(is);
  //}
  //virtual void save (ostream& os){};

  // Print (not mandatory)
  // virtual void printOn (ostream& os);

  // Access genetic programs (not mandatory)
  MyGP* NthMyGP (int n) {
    return (MyGP*) GPContainer::Nth (n); }
};

#endif


