#ifndef __UTIL__H__
#define __UTIL__H__

#include<iostream>
#include<map>
#include<vector>

/*
	Trace Debug: send message to stderr
*/
/*
inline void TRACE_D(string TAG, string msg){
	cerr<<TAG<<" --- " << msg<<endl;
}
*/
#define TRACE_D(TAG,MSG) {std::cerr<<TAG << " --- " << MSG << std::endl;}

/*
	Trace verbose: send message to stdout.
*/
/*
inline void TRACE_V(string TAG, string msg){	
	cout<<TAG<<" --- " << msg<<endl;
}
*/
#define TRACE_V(TAG,MSG) {std::cout<<TAG << " --- " << MSG << std::endl;}


inline std::string getCompIndex(std::string t, std::string c){
	return t+"-"+c;
}

inline double getValue(std::map< std::string, double> &h, std::string k) {
	std::map<std::string, double>::iterator it = h.find(k);
	
	if (it != h.end()) 
		return it->second;
	
	return 0.0;
}

inline double getValue(std::map< std::string, int> &h, std::string k) {
	std::map<std::string, int>::iterator it = h.find(k);
	
	if (it != h.end()) 
		return it->second;
	
	return 0;
}

inline double getValue(std::vector< std::map< std::string, double> > &h, unsigned int g, std::string k) {
	if(h.size() < g) return 0.0;
	else return getValue(h[g], k);
}

inline int getValue(std::vector< std::map< std::string, int> > &h, unsigned int g, std::string k) {
	if(h.size() < g) return 0.0;
	else return int(getValue(h[g], k));
}

#endif

