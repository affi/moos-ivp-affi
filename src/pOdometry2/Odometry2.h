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
    
    bool   m_first_reading;
    double m_current_x;
    double m_current_y;
    double m_previous_x;
    double m_previous_y;
    double m_total_distance;
    
private: // Configuration variables
    
private: // State variables
    unsigned int m_iterations;
    double       m_timewarp;
};

#endif 
