/************************************************************/
/*    NAME: Janille Maragh                                              */
/*    ORGN: MIT                                             */
/*    FILE: GenPath.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "GenPath.h"

using namespace std;

//---------------------------------------------------------
// Constructor

GenPath::GenPath()
{
    assign_by_region = false;
    m_bufferlength  = 100;
    
    m_points.resize(m_bufferlength);
    for (int i = 0; i++; i<m_bufferlength) {
        m_points.at(i).id = -1;
    }
    m_points_ordered.resize(m_bufferlength);
    
    m_iterations = 0;
    m_timewarp   = 1;
}

//---------------------------------------------------------
// Destructor

GenPath::~GenPath()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool GenPath::OnNewMail(MOOSMSG_LIST &NewMail)
{
    MOOSMSG_LIST::iterator p;
    
    for(p=NewMail.begin(); p!=NewMail.end(); p++) {
        CMOOSMsg &msg = *p;
        
        if (msg.GetKey() == "VISIT_POINT") {
            if (msg.GetString() == "firstpoint") {
                m_points.clear();
                update_str = "";
            }
            else if (msg.GetString() == "lastpoint") {
                update_str = "";
            }
            else {
                for (int i = 0; i < m_bufferlength; i++) {
                    if (m_points.at(i).id == -1) {      // if struct is empty
                        
                        // parse incoming string, comma as a delimeter
                        svector         = parseString(msg.GetString(),",");
                        
                        // remove "x=" etc. from each item in svector, store in struct
                        temp_string         = stripBlankEnds(biteString(svector[0],'='));
                        m_points.at(i).x    = atof(svector[0].c_str());
                        
                        temp_string         = stripBlankEnds(biteString(svector[1],'='));
                        m_points.at(i).y    = atof(svector[1].c_str());
                        
                        temp_string         = stripBlankEnds(biteString(svector[2],'='));
                        m_points.at(i).id   = atof(svector[2].c_str());
                        
                        break;
                    }
                }
            }
        }
        
        else if (msg.GetKey() == "NAV_X")
            m_osx = atof(msg.GetString().c_str());
        else if (msg.GetKey() == "NAV_Y")
            m_osy = atof(msg.GetString().c_str());
    }
    return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool GenPath::OnConnectToServer()
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

bool GenPath::Iterate()
{
    double shortest_dist    = -1;
    int closest_index       = -1;
    double dist;
    
    for (int i = 0; i < m_bufferlength; i++){
        if (m_points.at(i).id != -1) {
            dist = pow((pow((m_points.at(i).x - m_osx), 2) + \
                        pow((m_points.at(i).y - m_osy), 2)), 0.5);
            
            if (shortest_dist == -1) {
                shortest_dist = dist;
                closest_index = i;
                ResetPoint(m_points.at(i));
            }
            
            else if (dist < shortest_dist) {
                shortest_dist = dist;
                closest_index = i;
                ResetPoint(m_points.at(i));
            }
        }
    }
    
    m_seglist.add_vertex(m_points.at(closest_index).x, m_points.at(closest_index).y);
    update_str += m_seglist.get_spec();
    m_Comms.Notify("UPDATES_VAR",update_str);
    
    m_iterations++;
    return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool GenPath::OnStartUp()
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

void GenPath::RegisterVariables()
{
    m_Comms.Register("FOOBAR", 0);
}

//---------------------------------------------------------
// Procedure: ResetPoint

void GenPath::ResetPoint(struct Point& point)
{
    point.x     = 0;
    point.y     = 0;
    point.id    = -1;
}




