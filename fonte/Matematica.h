
#ifndef __MATEMATICA__H__
#define __MATEMATICA__H__

#include <cmath>


#define EPS 1e-10
//MY_INFINITY is smaller then 2**31
#define MY_INFINITY 1e9

// -1 : x < y ; 0: x == y ; 1 x > y
inline int cmp(double x, double y = 0, double tol = EPS) {
	return (x <= y + tol) ? (x + tol < y) ? -1 : 0 : 1;
}

inline bool lessThanZero(double val) { return cmp(val) == -1 ? true : false;}
inline bool lessOrEqualThanZero(double val) { return cmp(val) <= 0 ? true : false;}
inline bool isZero(double val) { return cmp(val) == 0 ? true : false;}
inline bool greaterThan(double a, double b = 0.0) {return cmp(a,b) == 1 ? true : false;}
inline bool greaterOrEqualThan(double a, double b = 0.0) {return cmp(a,b) >= 1 ? true : false;}
inline bool lesserThan(double a, double b = 0.0) {return cmp(a,b) == -1 ? true : false;}
inline bool equals(double a, double b) {return cmp(a,b) == 0 ? true : false;}

inline double my_log(double val){ double res = log(val); return (lessThanZero(res) || isnan(res) || isinf(res)) ? 0.0 : res;}
inline double my_log_avoid_zero(double val){ double res = log(val); return (isnan(res) || isinf(res)) ? 0.0 : res;}
inline double my_div(double x, double y) {return ( isZero(y) ) ? 0.0 : x/y;}
inline double my_pow(double val, double val2){ double res = pow(val,val2) ; return isinf(res) || isnan(res) ? MY_INFINITY : res; }
inline double my_mult(double val, double val2){ double res = val * val2; return isinf(res) || isnan(res) ? MY_INFINITY : res;  }
inline double my_sum(double val, double val2){ double res = val + val2; return isinf(res) || isnan(res) ? MY_INFINITY : res;  }
inline double my_sub(double val, double val2){ double res = val - val2; return lessThanZero(res) ? 0.0 : res;  }

#endif
