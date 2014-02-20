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
  first_reading      = 0;
  current_x          = 0;       // Assigning default values
  current_y          = 0;
  previous_x         = 0;
  total_distance     = 0;
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
  MOOSMSG_LIST::iterator p;
   
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;

    if (msg.GetKey() == "NAV_X") {
      previous_x = current_x;
      current_x  = msg.GetDouble();
    }
    else if (msg.GetKey() == "NAV_Y") {
      previous_y = current_y;
      current_y = msg.GetDouble();
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
  total_distance += sqrt(pow((current_x - previous_x),2) + 
			 pow((current_y - previous_y),2));
  cout << "Total distance is " << total_distance << endl;
  m_Comms.Notify("ODOMETRY_DIST", total_distance);


  m_iterations++;
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool Odometry2::OnStartUp()
{
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

  RegisterVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void Odometry2::RegisterVariables()
{
  m_Comms.Register("NAV_X",0);
  m_Comms.Register("NAV_Y",0);
}

