/************************************************************/
/*    NAME: Janille Maragh                                  */
/*    ORGN: MIT                                             */
/*    FILE: BHV_ZigLeg.cpp                                  */
/*    DATE: May 14, 2014                                    */
/************************************************************/

#include <iterator>
#include <cstdlib>
#include "MBUtils.h"
#include "BuildUtils.h"
#include "BHV_ZigLeg.h"
#include <math.h>

using namespace std;

//---------------------------------------------------------------
// Constructor

BHV_ZigLeg::BHV_ZigLeg(IvPDomain domain) :
IvPBehavior(domain)
{
    m_wpt_index     = -1;
    m_wpt_time      = 0;
    m_x             = 0;
    m_y             = 0;
    m_zig_angle     = 30;
    m_zig_duration  = 10;
    m_zigleg        = false;
    m_ipf_type      = "zaic";
    m_received = false;
    
    // Provide a default behavior name
    IvPBehavior::setParam("name", "defaultname");
    
    // Declare the behavior decision space
    m_domain = subDomain(m_domain, "course,speed");
    
    // Add any variables this behavior needs to subscribe for
    addInfoVars("NAV_X, NAV_Y, NAV_HEADING");
    addInfoVars("WPT_INDEX");
    
    
}

//---------------------------------------------------------------
// Procedure: setParam()

bool BHV_ZigLeg::setParam(string param, string val)
{
    // Convert the parameter to lower case for more general matching
    param = tolower(param);
    
    // Get the numerical value of the param argument for convenience once
    double double_val = atof(val.c_str());
    
    if((param == "zig_duration") && isNumber(val)) {
        m_zig_duration = double_val;
        return(true);
    }
    else if (param == "zig_angle" && isNumber(val)) {
        m_zig_angle = double_val;
        return(true);
    }
    else if(param == "ipf_type") {
        val = tolower(val);
        if(val == "zaic")  {
            m_ipf_type = val;
            return(true);
        }
    }
    
    // If not handled above, then just return false;
    return(false);
}

//---------------------------------------------------------------
// Procedure: onSetParamComplete()
//   Purpose: Invoked once after all parameters have been handled.
//            Good place to ensure all required params have are set.
//            Or any inter-param relationships like a<b.

void BHV_ZigLeg::onSetParamComplete()
{
}

//---------------------------------------------------------------
// Procedure: onHelmStart()
//   Purpose: Invoked once upon helm start, even if this behavior
//            is a template and not spawned at startup

void BHV_ZigLeg::onHelmStart()
{
}

//---------------------------------------------------------------
// Procedure: onIdleState()
//   Purpose: Invoked on each helm iteration if conditions not met.

void BHV_ZigLeg::onIdleState()
{
}

//---------------------------------------------------------------
// Procedure: onCompleteState()

void BHV_ZigLeg::onCompleteState()
{
}

//---------------------------------------------------------------
// Procedure: postConfigStatus()
//   Purpose: Invoked each time a param is dynamically changed

void BHV_ZigLeg::postConfigStatus()
{
}

//---------------------------------------------------------------
// Procedure: onIdleToRunState()
//   Purpose: Invoked once upon each transition from idle to run state

void BHV_ZigLeg::onIdleToRunState()
{
}

//---------------------------------------------------------------
// Procedure: onRunToIdleState()
//   Purpose: Invoked once upon each transition from run to idle state

void BHV_ZigLeg::onRunToIdleState()
{
}

//---------------------------------------------------------------
// Procedure: onRunState()
//   Purpose: Invoked each iteration when run conditions have been met.

IvPFunction* BHV_ZigLeg::onRunState()
{
    // Part 1: Build the IvP function
    IvPFunction *ipf = 0;
    
    UpdateStateVariables();
    
    double curr_time = getBufferCurrTime();
    if (m_received) {
        if ((curr_time - m_wpt_time) >= 10.0) {
            m_received = false;
            m_zigleg = true;
        }
    }
    
    //LET'S SEND A ZIG COMMAND
    if (m_zigleg) {
        if(m_ipf_type == "zaic")
            ipf = buildFunctionWithZAIC();
        else
            ipf = 0;
        
        if(ipf == 0)
            postWMessage("Problem Creating the IvP Function");
        
        if ((curr_time - m_wpt_time) >= (10.0 + m_zig_duration))
            m_zigleg = false;
    }
    
    
    
    // Part N: Prior to returning the IvP function, apply the priority wt
    // Actual weight applied may be some value different than the configured
    // m_priority_wt, depending on the behavior author's insite.
    if(ipf)
        ipf->setPWT(m_priority_wt);
    
    return(ipf);
}

void BHV_ZigLeg::UpdateStateVariables() {
    bool ok1, ok2, ok3, ok4;
    
    m_x             = getBufferDoubleVal("NAV_X", ok1);
    m_y             = getBufferDoubleVal("NAV_Y", ok2);
    
    if (!m_zigleg)
        m_heading       = getBufferDoubleVal("NAV_HEADING", ok3);
    else
        ok3 = true;
    
    int new_index   = (int)getBufferDoubleVal("WPT_INDEX", ok4);
    
    if((new_index != m_wpt_index) && (new_index != -1)) {
        //m_zigleg = false;
        m_received = true;
        m_wpt_time = getBufferCurrTime();
    }
    
    m_wpt_index = new_index;
    
    // Get vehicle position from InfoBuffer and post a
    // warning if problem is encountered
    if(!ok1 || !ok2) {
        postWMessage("No ownship X/Y info in info_buffer.");
    }
    if(!ok3) {
        postWMessage("No ownship heading info in info_buffer.");
    }
    if(!ok4) {
        postWMessage("No ownship waypoint index info in info_buffer.");
    }
    
}

//-----------------------------------------------------------
// Procedure: buildFunctionWithZAIC

IvPFunction *BHV_ZigLeg::buildFunctionWithZAIC()
{
    double new_angle = m_heading + m_zig_angle;
    ZAIC_PEAK crs_zaic(m_domain, "course");
    crs_zaic.setSummit(new_angle);
    crs_zaic.setPeakWidth(0);
    crs_zaic.setBaseWidth(180.0);
    crs_zaic.setSummitDelta(0);
    crs_zaic.setValueWrap(true);
    if(crs_zaic.stateOK() == false) {
        string warnings = "Course ZAIC problems " + crs_zaic.getWarnings();
        postWMessage(warnings);
        return(0);
    }
    
    IvPFunction *crs_ipf = crs_zaic.extractIvPFunction();
    
    return(crs_ipf);
}

