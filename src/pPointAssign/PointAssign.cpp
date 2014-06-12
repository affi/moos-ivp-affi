/************************************************************/
/*    NAME: Janille Maragh                                              */
/*    ORGN: MIT                                             */
/*    FILE: PointAssign.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "PointAssign.h"

using namespace std;

//---------------------------------------------------------
// Constructor

PointAssign::PointAssign()
{
    m_bufferlength = 100;
    
    m_Points.resize(m_bufferlength);
    
    for (int i = 0; i < m_bufferlength; i++) {
        m_Points.at(i).empty        = true;
        m_Points.at(i).x            = 0;
        m_Points.at(i).y            = 0;
        m_Points.at(i).id           = 0;
        m_Points.at(i).stringVal    = "";
    }
    
    variableOut         = "VISIT_POINT_";
    assign_by_region    = false;
    
    m_iterations = 0;
    m_timewarp   = 1;
}

//---------------------------------------------------------
// Destructor

PointAssign::~PointAssign()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool PointAssign::OnNewMail(MOOSMSG_LIST &NewMail)
{
    MOOSMSG_LIST::iterator p;
    
    for(p=NewMail.begin(); p!=NewMail.end(); p++) {
        CMOOSMsg &msg = *p;
        
        if ((msg.GetKey() == "VISIT_POINT") && (msg.GetString()=="firstpoint")) {
            m_Comms.Notify(variableOut,"firstpoint");
        }
        else if ((msg.GetKey() == "VISIT_POINT") && (msg.GetString()=="lastpoint")) {
            m_Comms.Notify(variableOut,"lastpoint");
        }
        else {
            for (int i = 0; i < m_bufferlength; i++){
                if (m_Points.at(i).empty) {
                    m_Points.at(i).stringVal = msg.GetString();
                    svector         = parseString(m_Points.at(i).stringVal,",");
                    
                    // get x-value
                    temp_string         = stripBlankEnds(biteString(svector[0],'='));
                    m_Points.at(i).x    = atof(svector[0].c_str());
                    cout << "x is " << m_Points.at(i).x << endl;
                    
                    // get y-value
                    temp_string         = stripBlankEnds(biteString(svector[1],'='));
                    m_Points.at(i).y    = atof(svector[1].c_str());
                    cout << "y is " << m_Points.at(i).y << endl;
                    
                    // get ID number
                    temp_string         = stripBlankEnds(biteString(svector[2],'='));
                    m_Points.at(i).id   = atoi(svector[2].c_str());
                    cout << "ID is "<< m_Points.at(i).id << endl;
                }
            }
        }
    }
    
    return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool PointAssign::OnConnectToServer()
{
    RegisterVariables();
    return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool PointAssign::Iterate()
{
    for (int i = 1; i < m_bufferlength; i++) {
        if (m_Points.at(i).empty == false) {
            if (assign_by_region == true) {
                if (m_Points.at(i).x < 88) {
                    variableOut += "HENRY";
                }
                else {
                    variableOut += "GILDA";
                }
            }
            else {
                if (m_Points.at(i).id%2 == 0) {
                    variableOut += "HENRY";
                }
                else {
                    variableOut += "GILDA";
                }
            }
            m_Comms.Notify(variableOut,m_Points.at(i).stringVal);
            
            // Reset struct
            m_Points.at(i).empty        = true;
            m_Points.at(i).x            = 0;
            m_Points.at(i).y            = 0;
            m_Points.at(i).id           = 0;
            m_Points.at(i).stringVal    = "";
            
        }
    }
    
    m_iterations++;
    return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool PointAssign::OnStartUp()
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

void PointAssign::RegisterVariables()
{
    m_Comms.Register("VISIT_POINT", 0);
}

//---------------------------------------------------------
//// Procedure: toString
//
//std::string PointAssign::toString(const double t) {
//    // open output string stream, add argument, convert to string
//    std::ostringstream ss;
//    ss << t;
//    return ss.str();
//}

