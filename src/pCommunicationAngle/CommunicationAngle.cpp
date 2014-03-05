/************************************************************/
/*    NAME: Janille                                              */
/*    ORGN: MIT                                             */
/*    FILE: CommunicationAngle.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "CommunicationAngle.h"
#include <cmath>

using namespace std;

//---------------------------------------------------------
// Constructor

CommunicationAngle::CommunicationAngle()
{
    xo          = 0;
    yo          = 0;
    zo          = 0;
    xc          = 0;
    yc          = 0;
    zc          = 0;
    g           = 9.81;
    co          = 1480;
    deltatheta  = 0.00001;
    pi          = 3.14159265;
    
    m_iterations = 0;
    m_timewarp   = 1;
}

//---------------------------------------------------------
// Destructor

CommunicationAngle::~CommunicationAngle()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool CommunicationAngle::OnNewMail(MOOSMSG_LIST &NewMail)
{
    MOOSMSG_LIST::iterator p;
    
    for(p=NewMail.begin(); p!=NewMail.end(); p++) {
        CMOOSMsg &msg = *p;
        
        if (msg.GetKey() == "NAV_X") {
            xo = msg.GetDouble();
        }
        else if (msg.GetKey() == "NAV_Y") {
            yo = msg.GetDouble();
        }
        else if (msg.GetKey() == "NAV_DEPTH") {
            zo = msg.GetDouble();
        }
        else if (msg.GetKey() == "'collaborator'_NAV_X") {
            xc = msg.GetDouble();
        }
        else if (msg.GetKey() == "'collaborator'_NAV_Y") {
            yc = msg.GetDouble();
        }
        else if (msg.GetKey() == "'collaborator'_NAV_DEPTH") {
            zc = msg.GetDouble();
        }
        
#if 0 // Keep these around just for template
        string key   = msg.GetKey();
        string comm  = msg.GetCommunity();
        double dval  = msg.GetDouble();
        string sval  = msg.GetString();
        string msrc  = msg.GetSource();
        double mtime = msg.GetTime();
        bool   mdbl  = msg.IsDouble();
        bool   mstr  = msg.IsString();
#endif
    }
	
    return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool CommunicationAngle::OnConnectToServer()
{
    // register for variables here
    // possibly look at the mission file?
    // m_MissionReader.GetConfigurationParam("Name", <string>);
    // m_Comms.Register("VARNAME", 0);
	
    RegisterVariables();
    return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool CommunicationAngle::Iterate()
{
    m_iterations++;
    return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool CommunicationAngle::OnStartUp()
{
    list<string> sParams;
    m_MissionReader.EnableVerbatimQuoting(false);
    if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
        list<string>::iterator p;
        for(p=sParams.begin(); p!=sParams.end(); p++) {
            string original_line = *p;
            string param = stripBlankEnds(toupper(biteString(*p, '=')));
            string value = stripBlankEnds(*p);
            
            if(param == "FOO") {
                //handled
            }
            else if(param == "BAR") {
                //handled
            }
        }
    }
    
    m_timewarp = GetMOOSTimeWarp();
    
    RegisterVariables();
    return(true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void CommunicationAngle::RegisterVariables()
{
    m_Comms.Register("VEHICLE_NAME", 0);
    m_Comms.Register("COLLABORATOR_NAME", 0);
    m_Comms.Register("NAV_X", 0);
    m_Comms.Register("NAV_Y", 0);
    m_Comms.Register("NAV_DEPTH", 0);
    m_Comms.Register("NAV_HEADING", 0);
    m_Comms.Register("NAV_SPEED", 0);
    m_Comms.Register("'collaborator'_NAV_X", 0);
    m_Comms.Register("'collaborator'_NAV_Y", 0);
    m_Comms.Register("'collaborator'_NAV_DEPTH", 0);
    m_Comms.Register("'collaborator'_NAV_HEADING", 0);
    m_Comms.Register("'collaborator'_NAV_SPEED", 0);
}

//---------------------------------------------------------
// Procedure: CalcElevAngle
double CommunicationAngle::CalcElevAngle(double xo, double yo, double zo, double xc, double yc, double zc)
{
    // distance between OS and collaborator
    double dist = pow((pow((xo - xc),2),pow((yo - yc),2)),0.5);
    // vertical distance (z) between OS and collaborator
    double delta_z = zo - zc;
    // angle between horizon and dist
    double beta = asin(delta_z/dist);
    // r-distance between OS and centre of dist
    double x1 = 0.5*dist*cos(beta);
    // z-distance between centre of circle and OS
    double H = zo + (co/g);
    // z-distance between OS and centre of dist
    double h = 0.5*dist*sin(beta);
    // z-distance between centre of circle and centre of dist
    double height = H + h;
    // r-distance between centre of dist and centre of circle
    double x2 = height*tan(beta);
    // radius of circles
    double R = pow((pow((x1+x2),2)),0.5);
    // angle between dist and line btwn centre of circle and OS
    double delta = acos(dist/(2*R));
    // angle of elevation
    double theta0 = 90 + beta - delta;
    
    return theta0;
    
}

//---------------------------------------------------------
// Procedure: CalcTransmissionLoss
double CommunicationAngle::CalcTransmissionLoss(double theta, double theta0, double zo, double R)
{
    // arc length
    double s = R*(theta0 - theta);
    // speed of sound at z = arc length
    double cs = 1480 + g*s;
    // speed of sound at OS
    double czo = 1480 + g*zo;
    // relative cross-sectional area of ray tube
    double J = ((rCalc(R,theta,theta0))/(sin(theta)))*(((rCalc(R,theta+deltatheta,theta0)-rCalc(R,theta,theta0)))/(deltatheta));
    // pressure field amplitude
    double p1 = 1/(4*pi);
    double ps = (1/(4*pi))*pow(abs((cs*cos(theta0))/(czo*J)),0.5);
    // transmission loss
    double TL = -20*log10(ps/p1);
    
    return TL;
    
}


//---------------------------------------------------------
// Procedure: rCalc
double CommunicationAngle::rCalc (double R, double theta, double theta0)
{
    double r = R*sin(theta0) - R*cos(theta);
    return r;
}
