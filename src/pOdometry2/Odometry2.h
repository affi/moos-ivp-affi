/************************************************************/
/*    NAME: Janille M. Maragh                               */
/*    ORGN: MIT                                             */
/*    FILE: Odometry2.h                                     */
/*    DATE: February 24, 2014                               */
/*    SUMMARY: This app calculated the total distance a     */
/*    vehicle has traveled by calculating how far it has    */
/*    traveled between readings of x- and y-positions. It   */
/*    subscribes to  NAV_X and NAV_Y. By constantly adding  */
/*    the distance traveled since the last reading, the     */
/*    total distance traveled is calculated. Is Appcasting. */
/************************************************************/

#ifndef Odometry2_HEADER
#define Odometry2_HEADER
#include <math.h>

//#include "MOOS/libMOOS/MOOSLib.h"
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"

using namespace std;

class Odometry2 : public AppCastingMOOSApp
{
public:
    Odometry2();
    ~Odometry2();
    
protected:
    bool OnNewMail(MOOSMSG_LIST &NewMail);
    bool Iterate();
    bool OnConnectToServer();
    bool OnStartUp();
    void registerVariables(); // own method so that we can invoke AppCastingMOOSApp
                              // superclass when registering for variables
    
    bool   m_first_reading;  // must be accounted for in case first readings != 0
    double m_current_x;
    double m_current_y;
    double m_previous_x;
    double m_previous_y;
    double m_total_distance;

    bool  buildReport();     // for reporting the total distance traveled
    
private: // Configuration variables
    
private: // State variables
    unsigned int m_iterations;
    double       m_timewarp;
};

#endif 
