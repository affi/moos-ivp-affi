/*************************************************************/
/*    NAME: Janille Maragh                                   */
/*    ORGN: MIT                                              */
/*    FILE: SinuParamGrab.cpp                                */
/*    DATE: Wednesday, July 23, 2014                         */
/*    SMRY: Subscribes to UCTD_MSMNT_REPORT, which provides  */
/*          the x-,y-coordinates and temperatures from a     */
/*          previous survey, then uses them to choose a      */
/*          starting point for a second sinusoidal survey,   */
/*          by searching the coordinates for the leftmost    */
/*          point that's close to the average of the max and */
/*          min temperatures.                                */
/*          Also subscribes to UCTD_PARAMETER_ESTIMATE and   */
/*          parses it for ampltitude, offset angle and       */
/*          wavelength estimates from the previous survey,   */
/*          and uses those estimates to update the parameters*/
/*          for the following sinusoidal survey.             */
/*************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "SinuParamGrab.h"

using namespace std;

//---------------------------------------------------------
// Constructor

SinuParamGrab::SinuParamGrab()
{
    m_iterations = 0;
    m_timewarp   = 1;
}

//---------------------------------------------------------
// Destructor

SinuParamGrab::~SinuParamGrab()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool SinuParamGrab::OnNewMail(MOOSMSG_LIST &NewMail)
{
    MOOSMSG_LIST::iterator p;
    
    vector<string> svector;
    string temp_string;
    
    for(p=NewMail.begin(); p!=NewMail.end(); p++) {
        cout << "------ Start of Mail for loop\n";
        
        CMOOSMsg &msg = *p;
        
        if (msg.GetKey() == "UCTD_MSMNT_REPORT") {
            // parse incoming string, comma as a delimeter
            svector         = parseString(msg.GetString(),",");
            
            // remove "x=" from first item in svector
            temp_string     = stripBlankEnds(biteString(svector[2],'='));
            m_x_pts.push_back(atof((svector[2]).c_str()));
            
            // remove "y=" from first item in svector
            temp_string     = stripBlankEnds(biteString(svector[3],'='));
            m_y_pts.push_back(atof((svector[3]).c_str()));
            
            // remove "temp=" from first item in svector
            temp_string     = stripBlankEnds(biteString(svector[4],'='));
            m_temps.push_back(atof((svector[4]).c_str()));
            
        }
        
        else if (msg.GetKey() == "UCTD_PARAMETER_ESTIMATE") {
            // parse incoming string, comma as a delimeter
            svector         = parseString(msg.GetString(),",");
            
            // remove "angle=" from first item in svector
            temp_string     = stripBlankEnds(biteString(svector[2],'='));
            m_out_angle = svector[2];
            // these vectors used if we wanted to take in multiple
            // angle estimates and average them, but we're only
            // taking in the one estimate result after the two circle runs
            m_angles.push_back(atof((svector[2]).c_str()));
            
            // remove "amplitude=" from first item in svector
            temp_string     = stripBlankEnds(biteString(svector[3],'='));
            m_out_amplitude = toString(2.0*(atof(svector[3].c_str())));
            m_amplitudes.push_back(atof((svector[3]).c_str()));
            
            // remove "wavelength=" from first item in svector
            temp_string     = stripBlankEnds(biteString(svector[5],'='));
            m_out_wavelength = toString(0.5*(atof(svector[5].c_str())));
            m_wavelengths.push_back(atof((svector[5]).c_str()));
            
        }
    }
	cout << "end of mail for loop" << endl;
    return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool SinuParamGrab::OnConnectToServer()
{
    RegisterVariables();
    return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool SinuParamGrab::Iterate()
{
    if (m_temps.empty()) {
        return(true);
    }
    
    m_min_temp = m_temps.at(0);
    m_max_temp = m_temps.at(0);
    
    // find minimum and maximum temperatures from circular survey
    // to estimate temperature at front
    for (int i = 1; i < int(m_temps.size()); i++) {
        if (m_temps.at(i) < m_min_temp)
            m_min_temp = m_temps.at(i);
        else if (m_temps.at(i) > m_max_temp)
            m_max_temp = m_temps.at(i);
    }
    
    m_avg_temp = (m_max_temp + m_min_temp)/2;
    
    // by using the leftmost point passed on the front, we can use the normal
    // left to right motion to traverse the front within the operating region
    double leftmost_x = m_x_pts.at(0);
    double leftmost_y = m_y_pts.at(0);
    
    for (int i = 1; i < int(m_temps.size()); i++) {
        if ((fabs(m_temps.at(i) - m_avg_temp) < 1) && (m_x_pts.at(i) < leftmost_x)){
            leftmost_x = m_x_pts.at(i);
            leftmost_y = m_y_pts.at(i);
            
        }
    }
    
    // generating strings to concatenate for updates string
    m_x_string          = "START_X=" + toString(leftmost_x);
    m_y_string          = "START_Y=" + toString(leftmost_y);
    m_angle_string      = "OFFSET_ANGLE=" + m_out_angle;
    m_amplitude_string  = "AMPLITUDE=" + m_out_amplitude;
    m_wavelength_string = "WAVELENGTH=" + m_out_wavelength;
    
    // generating string to be used to update sinusoid parameters
    string output_string = m_angle_string + " # " + m_amplitude_string + " # " + m_wavelength_string + " # " + m_x_string + " # " + m_y_string;
    
    // publish updates string if there are estimates to be published
    // GENERATE_NOW indicates that a sinusoidal path may now be generated
    if ((!(toString(leftmost_x)).empty()) && (!m_out_angle.empty())) {
        m_Comms.Notify("UPDATES_VAR",output_string);
        m_Comms.Notify("GENERATE_NOW","true");
    }
    else if (m_out_angle.empty()) {
        m_Comms.Notify("GENERATE_NOW","false");
    }
    
    m_iterations++;
    return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool SinuParamGrab::OnStartUp()
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

void SinuParamGrab::RegisterVariables()
{
    m_Comms.Register("UCTD_MSMNT_REPORT", 0);
    m_Comms.Register("UCTD_PARAMETER_ESTIMATE", 0);
}

//---------------------------------------------------------
// Procedure: to String

string SinuParamGrab::toString(const double t) {
    std::ostringstream ss;
    ss << t;
    return ss.str();
}


