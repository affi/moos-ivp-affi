/************************************************************/
/*    NAME: Janille Maragh                                  */
/*    ORGN: MIT                                             */
/*    FILE: BHV_Pulse.cpp                                   */
/*    DATE: May 12, 2014                                    */
/************************************************************/

#include <iterator>
#include <cstdlib>
#include "MBUtils.h"
#include "BuildUtils.h"
#include "BHV_Pulse.h"

using namespace std;

//---------------------------------------------------------------
// Constructor

BHV_Pulse::BHV_Pulse(IvPDomain domain) :
IvPBehavior(domain)
{
    // Provide a default behavior name
    IvPBehavior::setParam("name", "defaultname");
    
    // Declare the behavior decision space
    m_domain = subDomain(m_domain, "course,speed");
    
    // Add any variables this behavior needs to subscribe for
    addInfoVars("NAV_X, NAV_Y");
    addInfoVars("WPT_INDEX");
    
    //m_wpt_index = -1;
}

//---------------------------------------------------------------
// Procedure: setParam()

bool BHV_Pulse::setParam(string param, string val)
{
    // Convert the parameter to lower case for more general matching
    param = tolower(param);
    
    // Get the numerical value of the param argument for convenience once
    double double_val = atof(val.c_str());
    
    if((param == "pulse_range") && isNumber(val)) {
        m_range = double_val;
        return(true);
    }
    else if (param == "pulse_duration" && isNumber(val)) {
        m_pulse_duration = double_val;
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

void BHV_Pulse::onSetParamComplete()
{
}

//---------------------------------------------------------------
// Procedure: onHelmStart()
//   Purpose: Invoked once upon helm start, even if this behavior
//            is a template and not spawned at startup

void BHV_Pulse::onHelmStart()
{
}

//---------------------------------------------------------------
// Procedure: onIdleState()
//   Purpose: Invoked on each helm iteration if conditions not met.

void BHV_Pulse::onIdleState()
{
}

//---------------------------------------------------------------
// Procedure: onCompleteState()

void BHV_Pulse::onCompleteState()
{
}

//---------------------------------------------------------------
// Procedure: postConfigStatus()
//   Purpose: Invoked each time a param is dynamically changed

void BHV_Pulse::postConfigStatus()
{
}

//---------------------------------------------------------------
// Procedure: onIdleToRunState()
//   Purpose: Invoked once upon each transition from idle to run state

void BHV_Pulse::onIdleToRunState()
{
}

//---------------------------------------------------------------
// Procedure: onRunToIdleState()
//   Purpose: Invoked once upon each transition from run to idle state

void BHV_Pulse::onRunToIdleState()
{
}

//---------------------------------------------------------------
// Procedure: onRunState()
//   Purpose: Invoked each iteration when run conditions have been met.

IvPFunction* BHV_Pulse::onRunState()
{
    // Part 1: Build the IvP function
    IvPFunction *ipf = 0;
    
    UpdateStateVariables();
    
    double curr_time = getBufferCurrTime();
    
    if (m_pulse) {
        // this part might not be necessary?
        //        do
        //            ;
        //        while ((curr_time - m_wpt_time) < 5.0);
        
        if ((curr_time - m_wpt_time) >= 5.0) { //just to be super super careful
            XYRangePulse pulse;
            pulse.set_x(m_x);
            pulse.set_y(m_y);
            pulse.set_label("bhv_pulse");
            pulse.set_color("edge","yellow");
            pulse.set_color("fill","yellow");
            pulse.set_time(curr_time);
            pulse.set_rad(m_range);
            pulse.set_duration(m_pulse_duration);
            
            string  spec = pulse.get_spec();
            postMessage("VIEW_RANGE_PULSE", spec);
            
            m_pulse = false;
        }
    }
    
    
    // Part N: Prior to returning the IvP function, apply the priority wt
    // Actual weight applied may be some value different than the configured
    // m_priority_wt, depending on the behavior author's insite.
    if(ipf)
        ipf->setPWT(m_priority_wt);
    return(ipf);
}


void BHV_Pulse::UpdateStateVariables() {
    bool ok1, ok2, ok3;
    
    m_x             = getBufferDoubleVal("NAV_X", ok1);
    m_y             = getBufferDoubleVal("NAV_Y", ok2);
    int new_index   = (int)getBufferDoubleVal("WPT_INDEX", ok3);
    
    if((new_index != m_wpt_index) && (new_index != -1)) {
        m_pulse = true;
        m_wpt_time = getBufferCurrTime();
        m_wpt_index = new_index;
    }
    
    // Get vehicle position from InfoBuffer and post a
    // warning if problem is encountered
    if(!ok1 || !ok2) {
        postWMessage("No ownship X/Y info in info_buffer.");
    }
    if(!ok3) {
        postWMessage("No ownship waypoint index info in info_buffer.");
    }
    
}


