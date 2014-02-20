/************************************************************/
/*    NAME: Janille Maragh                                              */
/*    ORGN: MIT                                             */
/*    FILE: PrimeFactor2.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef PrimeFactor2_HEADER
#define PrimeFactor2_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include <stdint.h>
#include <vector>
#include <stdlib.h>

using namespace std;

class PrimeFactor2 : public CMOOSApp
{
 public:
   PrimeFactor2();
   ~PrimeFactor2();

 protected:
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();
   void RegisterVariables();

   struct entered_value {
     uint64_t orig_num;
     uint64_t current_num;
     int      index_received;
     int      index_calculated;
     double   start_time;
     double   end_time;
     double   solve_time;
     string   prime_string;
     string   username;
   };

   std::vector<struct entered_value> num_values;

   int        calculations;
   int        total_received;
   string     outgoing_var;
   string     output_string;
   int        buffer_length;
 private: // Configuration variables??
   bool       IsPrime(int num);
   void       PrimeFactorization(struct entered_value& item, int& num_rec);
   string     toString(const double t);
   void       EnteredValueClear(struct entered_value& item);

 private: // State variables
   unsigned int m_iterations;
   double       m_timewarp;
};

#endif 
