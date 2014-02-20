/************************************************************/
/*    NAME: Janille Maragh                                              */
/*    ORGN: MIT                                             */
/*    FILE: PrimeFactorTester2.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef PrimeFactorTester2_HEADER
#define PrimeFactorTester2_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include <stdint.h>
#include <stdlib.h>
#include "MBUtils.h"
#include <string>
#include <iostream>

using namespace std;

class PrimeFactorTester2 : public CMOOSApp
{
 public:
   PrimeFactorTester2();
   ~PrimeFactorTester2();

 protected:
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();
   void RegisterVariables();

 private: // Configuration variables
   string received_string;
   string prime_string_in;
   string outgoing_var;
   string prime_string_test;
   string outgoing_string;

   vector<string> svector;

   uint64_t original_value;

   void PrimeFactorization(uint64_t num);
   bool IsPrime(int i);
   string toString (const int t);

 private: // State variables
   unsigned int m_iterations;
   double       m_timewarp;
};

#endif 
