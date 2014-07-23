/*************************************************************/
/*    NAME: Janille Maragh                                   */
/*    ORGN: MIT                                              */
/*    FILE: BHV_MySinusoid.cpp                               */
/*    DATE: Wednesday, July 23, 2014                         */
/*    SMRY: This behaviour generates a set of waypoints that */
/*          allow a vehicle to follow a sinusoidal path with */
/*          specified, wavelength, amplitude, offset angle   */
/*          from horizontal (x-axis), starting point, and    */
/*          direction (left to right or vice versa).         */
/*************************************************************/

#include <iterator>
#include <cstdlib>
#include "MBUtils.h"
#include "BuildUtils.h"
#include "BHV_MySinusoid.h"

using namespace std;

//---------------------------------------------------------------
// Constructor

BHV_MySinusoid::BHV_MySinusoid(IvPDomain domain) :
IvPBehavior(domain)
{
    m_osx = 0;
    m_osy = 0;
    m_generate_path = false;
    m_targ_index = 0;
    m_offset_angle = 0;
    m_start_x = -100;
    m_start_y = -90;
    m_amplitude = 20;
    m_wavelength = 60;
    m_disp_start_to_end = 120;
    m_num_wpts = 30;
    m_arrival_radius = 5;
    m_des_speed = 3;
    m_trace_backwards = false;
    m_startup = true;
    
    // Provide a default behavior name
    IvPBehavior::setParam("name", "defaultname");
    
    // Declare the behavior decision space
    m_domain = subDomain(m_domain, "course,speed");
    
    // Add any variables this behavior needs to subscribe for
    addInfoVars("NAV_X, NAV_Y, GENERATE_NOW");
    
}

//---------------------------------------------------------------
// Procedure: setParam()

bool BHV_MySinusoid::setParam(string param, string val)
{
    // Convert the parameter to lower case for more general matching
    param = tolower(param);
    
    if(param == "reverse") {
        if ((strcmp(val.c_str(),"true")) == 0) {
            m_trace_backwards = true;
        }
        else if ((strcmp(val.c_str(),"false")) == 0) {
            m_trace_backwards = false;
        }
        return(true);
    }
    
    // Get the numerical value of the param argument for convenience once
    double double_val = atof(val.c_str());
    
    if((param == "offset_angle") && isNumber(val)) {
        m_offset_angle = double_val;
        return(true);
    }
    else if((param == "start_x") && isNumber(val)) {
        m_start_x = double_val;
        return(true);
    }
    else if((param == "start_y") && isNumber(val)) {
        m_start_y = double_val;
        return(true);
    }
    else if((param == "amplitude") && isNumber(val)) {
        m_amplitude = double_val;
        return(true);
    }
    else if((param == "speed") && (double_val > 0) && (isNumber(val))) {
        m_des_speed = double_val;
        return(true);
    }
    else if((param == "wavelength") && isNumber(val)) {
        m_wavelength = double_val;
        return(true);
    }
    else if((param == "dist_start_end") && isNumber(val)) {
        m_disp_start_to_end = double_val;
        return(true);
    }
    else if((param == "num_wpts") && isNumber(val)) {
        m_num_wpts = double_val;
        return(true);
    }
    else if((param == "arrival_radius") && isNumber(val)) {
        m_arrival_radius = double_val;
        return(true);
    }
    
    // If not handled above, then just return false;
    return(false);
}

//---------------------------------------------------------------
// Procedure: onSetParamComplete()
//   Purpose: Invoked once after all parameters have been handled.
//            Good place to ensure all required params have are set.
//            Or any inter-param relationships like a<b.

void BHV_MySinusoid::onSetParamComplete()
{
}

//---------------------------------------------------------------
// Procedure: onHelmStart()
//   Purpose: Invoked once upon helm start, even if this behavior
//            is a template and not spawned at startup

void BHV_MySinusoid::onHelmStart()
{
}

//---------------------------------------------------------------
// Procedure: onIdleState()
//   Purpose: Invoked on each helm iteration if conditions not met.

void BHV_MySinusoid::onIdleState()
{
}

//---------------------------------------------------------------
// Procedure: onCompleteState()

void BHV_MySinusoid::onCompleteState()
{
}

//---------------------------------------------------------------
// Procedure: postConfigStatus()
//   Purpose: Invoked each time a param is dynamically changed

void BHV_MySinusoid::postConfigStatus()
{
}

//---------------------------------------------------------------
// Procedure: onIdleToRunState()
//   Purpose: Invoked once upon each transition from idle to run state

void BHV_MySinusoid::onIdleToRunState()
{
}

//---------------------------------------------------------------
// Procedure: onRunToIdleState()
//   Purpose: Invoked once upon each transition from run to idle state

void BHV_MySinusoid::onRunToIdleState()
{
}
//-----------------------------------------------------------
// Procedure: postViewPoint

void BHV_MySinusoid::postViewPoint(bool viewable)
{
    m_nextpt.set_label(m_us_name + "'s next waypoint");
    
    string point_spec;
    if(viewable)
        point_spec = m_nextpt.get_spec("active=true");
    else
        point_spec = m_nextpt.get_spec("active=false");
    postMessage("VIEW_POINT", point_spec);
}
//---------------------------------------------------------------
// Procedure: onRunState()
//   Purpose: Invoked each iteration when run conditions have been met.

IvPFunction* BHV_MySinusoid::onRunState()
{
    // Part 1: Build the IvP function
    IvPFunction *ipf = 0;
    
    UpdateStateVariables();
    
    // calculate waypoints on circumference
    if (m_generate_path)
    {
        for (int i = 0; i < m_num_wpts; i++) {
            // calculating the global location of the points on the angled sinusoid
            double k = (2*M_PI)/m_wavelength;
            double x = i*(m_disp_start_to_end/m_num_wpts);
            double theta = (m_offset_angle*M_PI)/180;
            double A = m_amplitude;
            
            // xp and yp are the points relative to chosen starting point
            double xp = x*cos(theta) - A*(sin(k*x))*(tan(theta))*(cos(theta));
            double yp = ((A*sin(k*x))/(cos(theta))) + xp*tan(theta);
            
            double x_global = m_start_x + xp;
            double y_global = m_start_y + yp;
            
            // setting limits on waypoints so they all lie within the op region
            if (x_global > 175) {
                x_global = 175;
            }
            if (y_global > 15) {
                y_global = 15;
            }
            if ((y_global > ((0.4*x_global)-25)) && (x_global >= -50) && (x_global <= 100)) {
                y_global = (0.4*x_global) - 25;
            }
            if ((y_global > ((0.7*x_global)-15)) && (x_global >= -100) && (x_global < -50)) {
                y_global = (0.7*x_global) - 15;
            }
            if (y_global < -190) {
                y_global = -190;
            }
            if ((y_global < ((-2.5*x_global)-320)) && (x_global >= -100) && (x_global <= -50)) {
                y_global = (-2.5*x_global) - 320;
            }
            
            // adding checked and ordered points of sinusoid to list of waypoints
            m_x_pts.push_back(x_global);
            m_y_pts.push_back(y_global);
        }
        
        // allow us to follow the reverse (right to left) sinusoid
        if (m_trace_backwards) {
            reverse(m_x_pts.begin(), m_x_pts.end());
            reverse(m_y_pts.begin(), m_y_pts.end());
        }
        
        m_targ_index = 0;
        m_targ_x = m_x_pts[m_targ_index];
        m_targ_y = m_y_pts[m_targ_index];
        m_generate_path = false;
    }
    
    if (m_targ_index > (m_num_wpts - 1)) {
        setComplete();
        postViewPoint(false);
        return(0);
    }
    else {
        m_dist_to_targ  = pow((pow((m_targ_x - m_osx),2)+pow((m_targ_y - m_osy),2)),0.5);
        
        // once within arrival radius of target wpt, we aim for the next
        if (m_dist_to_targ < m_arrival_radius) {
            m_targ_index++;
        }
        
        m_targ_x = m_x_pts[m_targ_index];
        m_targ_y = m_y_pts[m_targ_index];
        
        // for viewing the waypoints during mission
        m_nextpt.set_vx(m_x_pts[m_targ_index]);
        m_nextpt.set_vy(m_y_pts[m_targ_index]);
        
        ipf = buildFunctionWithZAIC();
    }
    
    postViewPoint(true);
    
    
    // Part N: Prior to returning the IvP function, apply the priority wt
    // Actual weight applied may be some value different than the configured
    // m_priority_wt, depending on the behavior author's insite.
    if(ipf)
        ipf->setPWT(m_priority_wt);
    
    return(ipf);
}

//-----------------------------------------------------------
// Procedure: UpdateStateVariables

void BHV_MySinusoid::UpdateStateVariables() {
    bool ok1, ok2;
    
    m_osx             = getBufferDoubleVal("NAV_X", ok1);
    m_osy             = getBufferDoubleVal("NAV_Y", ok2);
    m_gen_now         = getBufferStringVal("GENERATE_NOW");
    
    
    // Get vehicle position from InfoBuffer and post a
    // warning if problem is encountered
    if(!ok1 || !ok2) {
        postWMessage("No ownship X/Y info in info_buffer.");
    }
    
    if(m_gen_now == "")
        postWMessage("GENERATE_NOW info not found in the info buffer");
    
    // we don't generate a sinusoidal path until we have estimates for its
    // parameters from UCTD_PARAMETER_ESTIMATE, published by pSinuParamGrab
    if ((strcmp(m_gen_now.c_str(),"true") == 0) && (m_startup)) {
        m_generate_path = true;
        m_startup = false;
    }
    
}


//-----------------------------------------------------------
// Procedure: buildFunctionWithZAIC

IvPFunction *BHV_MySinusoid::buildFunctionWithZAIC()
{
    ZAIC_PEAK spd_zaic(m_domain, "speed");
    spd_zaic.setSummit(m_des_speed);
    spd_zaic.setPeakWidth(0.5);
    spd_zaic.setBaseWidth(1.0);
    spd_zaic.setSummitDelta(0.8);
    if(spd_zaic.stateOK() == false) {
        string warnings = "Speed ZAIC problems " + spd_zaic.getWarnings();
        postWMessage(warnings);
        return(0);
    }
    
    double rel_ang_to_wpt = relAng(m_osx, m_osy, m_targ_x, m_targ_y);
    ZAIC_PEAK crs_zaic(m_domain, "course");
    crs_zaic.setSummit(rel_ang_to_wpt);
    
    crs_zaic.setPeakWidth(0);
    crs_zaic.setBaseWidth(180.0);
    crs_zaic.setSummitDelta(0);
    crs_zaic.setValueWrap(true);
    if(crs_zaic.stateOK() == false) {
        string warnings = "Course ZAIC problems " + crs_zaic.getWarnings();
        postWMessage(warnings);
        return(0);
    }
    
    IvPFunction *spd_ipf = spd_zaic.extractIvPFunction();
    IvPFunction *crs_ipf = crs_zaic.extractIvPFunction();
    
    OF_Coupler coupler;
    IvPFunction *ivp_function = coupler.couple(crs_ipf, spd_ipf, 50, 50);
    
    return(ivp_function);
}



