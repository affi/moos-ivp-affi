/************************************************************/
/*    NAME: Janille                                              */
/*    ORGN: MIT                                             */
/*    FILE: Odometry2.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef Odometry2_HEADER
#define Odometry2_HEADER

#include "MOOS/libMOOS/MOOSLib.h"

class Odometry2 : public CMOOSApp
{
 public:
   Odometry2();
   ~Odometry2();

 protected:
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();
   void RegisterVariables();

   bool   first_reading;
   double current_x;
   double current_y;
   double previous_x;
   double previous_y;
   double total_distance;

 private: // Configuration variables

 private: // State variables
   unsigned int m_iterations;
   double       m_timewarp;
};

#endif 
