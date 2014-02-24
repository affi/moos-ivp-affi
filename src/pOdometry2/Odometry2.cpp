/************************************************************/
/*    NAME: Janille                                              */
/*    ORGN: MIT                                             */
/*    FILE: Odometry2.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "Odometry2.h"
#include <cmath>
#include <iostream>

using namespace std;

//---------------------------------------------------------
// Constructor

Odometry2::Odometry2()
{
    // state variables
    m_iterations       = 0;
    m_timewarp         = 1;
    
    // initialize variables used to calculate dist traveled
    m_first_reading    = true;
    m_current_x        = 0;       // Assigning default values
    m_current_y        = 0;
    m_previous_x         = 0;
    m_previous_y         = 0;
    m_total_distance     = 0;
}

//---------------------------------------------------------
// Destructor

Odometry2::~Odometry2()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool Odometry2::OnNewMail(MOOSMSG_LIST &NewMail)
{
    AppCastingMOOSApp::OnNewMail(NewMail);
    
    MOOSMSG_LIST::iterator p;
    
    for(p=NewMail.begin(); p!=NewMail.end(); p++) {
        CMOOSMsg &msg = *p;
        if (m_first_reading) {
            m_current_x = msg.GetDouble();
            m_current_y = msg.GetDouble();
            m_first_reading = false;
        }
        if (msg.GetKey() == "NAV_X") {
            m_previous_x = m_current_x;
            m_current_x  = msg.GetDouble();
        }
        else if (msg.GetKey() == "NAV_Y") {
            m_previous_y = m_current_y;
            m_current_y = msg.GetDouble();
        }
        return(true);
    }
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool Odometry2::OnConnectToServer()
{
    RegisterVariables();
    return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool Odometry2::Iterate()
{
    m_total_distance += sqrt(pow((m_current_x - m_previous_x),2) +
                             pow((m_current_y - m_previous_y),2));
    cout << "Total distance is " << m_total_distance << endl;
    m_Comms.Notify("ODOMETRY_DIST", m_total_distance);
    
    
    m_iterations++;
    AppCastingMOOSApp::PostReport();
    return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool Odometry2::OnStartUp()
{
    AppCastingMOOSApp::OnStartUp();
    list<string> sParams;
    m_MissionReader.EnableVerbatimQuoting(false);
    
    if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
        list<string>::iterator p;
        
        for(p=sParams.begin(); p!=sParams.end(); p++) {
            string original_line = *p;
            string param = stripBlankEnds(toupper(biteString(*p, '=')));
            string value = stripBlankEnds(*p);
            
            //      if(param == "FOO") {
            //handled
            // }
            // else if(param == "BAR") {
            //handled
            //}
        }
    }
    
    m_timewarp = GetMOOSTimeWarp();
    
    registerVariables();
    return(true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

//void Odometry2::RegisterVariables()
//{
//    m_Comms.Register("NAV_X",0);
//    m_Comms.Register("NAV_Y",0);
//}


//---------------------------------------------------------
// Procedure: registerVariables
void Odometry2::registerVariables()
{
    AppCastingMOOSApp::RegisterVariables();
    
    m_Comms.Register("NAV_X",0);
    m_Comms.Register("NAV_Y",0);
    
}

//---------------------------------------------------------
bool Odometry2::buildReport()
{
    m_msgs << "Total distance traveled: " << m_total_distance <<endl;
    return(true);
}

