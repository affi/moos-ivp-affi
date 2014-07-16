/************************************************************/
/*    NAME: Janille Maragh                                              */
/*    ORGN: MIT                                             */
/*    FILE: BHV_MyCircle.cpp                                    */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include <cstdlib>
#include "MBUtils.h"
#include "BuildUtils.h"
#include "BHV_MyCircle.h"

using namespace std;

//---------------------------------------------------------------
// Constructor

BHV_MyCircle::BHV_MyCircle(IvPDomain domain) :
IvPBehavior(domain)
{
    m_osx = 0;
    m_osy = 0;
    m_arrival_radius = 1;
    m_generate_path = true;
    m_ang_to_targ = 0;
    m_targ_index = 0;
    
    //pi  = atan(1)*4;
    
    // Provide a default behavior name
    IvPBehavior::setParam("name", "defaultname");
    
    // Declare the behavior decision space
    m_domain = subDomain(m_domain, "course,speed");
    
    // Add any variables this behavior needs to subscribe for
    addInfoVars("NAV_X, NAV_Y");
}

//---------------------------------------------------------------
// Procedure: setParam()

bool BHV_MyCircle::setParam(string param, string val)
{
    // Convert the parameter to lower case for more general matching
    param = tolower(param);
    
    // Get the numerical value of the param argument for convenience once
    double double_val = atof(val.c_str());
    
    if((param == "centre_x") && isNumber(val)) {
        m_centre_x = double_val;
        return(true);
    }
    else if((param == "centre_y") && isNumber(val)) {
        m_centre_y = double_val;
        return(true);
    }
    else if((param == "radius") && isNumber(val)) {
        m_radius = double_val;
        return(true);
    }
    else if((param == "num_segments") && isNumber(val)) {
        m_num_segments = double_val;
        return(true);
    }
    else if((param == "speed") && (double_val > 0) && (isNumber(val))) {
        m_des_speed = double_val;
        return(true);
    }
    //    else if((param == "arrival_radius") && isNumber(val)) {
    //        m_arrival_radius = double_val;
    //        return(true);
    //    }
    
    // If not handled above, then just return false;
    return(false);
}

//---------------------------------------------------------------
// Procedure: onSetParamComplete()
//   Purpose: Invoked once after all parameters have been handled.
//            Good place to ensure all required params have are set.
//            Or any inter-param relationships like a<b.

void BHV_MyCircle::onSetParamComplete()
{
}

//---------------------------------------------------------------
// Procedure: onHelmStart()
//   Purpose: Invoked once upon helm start, even if this behavior
//            is a template and not spawned at startup

void BHV_MyCircle::onHelmStart()
{
}

//---------------------------------------------------------------
// Procedure: onIdleState()
//   Purpose: Invoked on each helm iteration if conditions not met.

void BHV_MyCircle::onIdleState()
{
}

//---------------------------------------------------------------
// Procedure: onCompleteState()

void BHV_MyCircle::onCompleteState()
{
}

//---------------------------------------------------------------
// Procedure: postConfigStatus()
//   Purpose: Invoked each time a param is dynamically changed

void BHV_MyCircle::postConfigStatus()
{
}

//---------------------------------------------------------------
// Procedure: onIdleToRunState()
//   Purpose: Invoked once upon each transition from idle to run state

void BHV_MyCircle::onIdleToRunState()
{
}

//---------------------------------------------------------------
// Procedure: onRunToIdleState()
//   Purpose: Invoked once upon each transition from run to idle state

void BHV_MyCircle::onRunToIdleState()
{
}

//---------------------------------------------------------------
// Procedure: onRunState()
//   Purpose: Invoked each iteration when run conditions have been met.

IvPFunction* BHV_MyCircle::onRunState()
{
    // Part 1: Build the IvP function
    IvPFunction *ipf = 0;
    
    UpdateStateVariables();
    
    // calculate waypoints on circumference
    if (m_generate_path)
    {
        for (int i = 0; i < m_num_segments; i++) {
            double x = m_centre_x + m_radius*cos((2*i*M_PI)/m_num_segments);
            double y = m_centre_y + m_radius*sin((2*i*M_PI)/m_num_segments);
            m_x_pts.push_back(x);
            m_y_pts.push_back(y);
        }
        
        m_targ_index = 0;
        m_targ_x = m_x_pts[m_targ_index];
        m_targ_y = m_y_pts[m_targ_index];
        m_generate_path = false;
    }
    
    if (m_targ_index > (m_num_segments - 1)) {
        setComplete();
        return(0);
    }
    else {
        m_dist_to_targ  = pow((pow((m_targ_x - m_osx),2)+pow((m_targ_y - m_osy),2)),0.5);
        
        if (m_dist_to_targ < m_arrival_radius) {
            m_targ_index++;
        }
        
        m_targ_x = m_x_pts[m_targ_index];
        m_targ_y = m_y_pts[m_targ_index];
        
        m_ang_to_targ   = atan2((m_targ_y - m_osy),(m_targ_x - m_osx));
        
        ipf = buildFunctionWithZAIC();
    }
    // Part N: Prior to returning the IvP function, apply the priority wt
    // Actual weight applied may be some value different than the configured
    // m_priority_wt, depending on the behavior author's insite.
    if(ipf)
        ipf->setPWT(m_priority_wt);
    
    return(ipf);
}


//-----------------------------------------------------------
// Procedure: UpdateStateVariables

void BHV_MyCircle::UpdateStateVariables() {
    bool ok1, ok2;
    
    m_osx             = getBufferDoubleVal("NAV_X", ok1);
    m_osy             = getBufferDoubleVal("NAV_Y", ok2);
    
    // Get vehicle position from InfoBuffer and post a
    // warning if problem is encountered
    if(!ok1 || !ok2) {
        postWMessage("No ownship X/Y info in info_buffer.");
    }
    
}

//-----------------------------------------------------------
// Procedure: buildFunctionWithZAIC

IvPFunction *BHV_MyCircle::buildFunctionWithZAIC()
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
    
//    ZAIC_PEAK crs_zaic(m_domain, "course");
//    crs_zaic.setSummit(m_ang_to_targ);
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


