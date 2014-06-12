/************************************************************/
/*    NAME: Janille Maragh                                  */
/*    ORGN: MIT                                             */
/*    FILE: BHV_Pulse.h                                     */
/*    DATE: May 12, 2014                                    */
/************************************************************/

#ifndef Pulse_HEADER
#define Pulse_HEADER

#include <string>
#include "IvPBehavior.h"
#include "XYRangePulse.h"

class BHV_Pulse : public IvPBehavior {
public:
    BHV_Pulse(IvPDomain);
    ~BHV_Pulse() {};
    
    bool         setParam(std::string, std::string);
    void         onSetParamComplete();
    void         onCompleteState();
    void         onIdleState();
    void         onHelmStart();
    void         postConfigStatus();
    void         onRunToIdleState();
    void         onIdleToRunState();
    
    IvPFunction* onRunState();
    
protected: // Local Utility functions
    void         UpdateStateVariables();
    
protected: // Configuration parameters
    
protected: // State variables
    double       m_x;
    double       m_y;
    double       m_wpt_time;
    int          m_wpt_index;
    double       m_range;
    double       m_pulse_duration;
    bool         m_pulse;
};

#define IVP_EXPORT_FUNCTION

extern "C" {
    IVP_EXPORT_FUNCTION IvPBehavior * createBehavior(std::string name, IvPDomain domain)
    {return new BHV_Pulse(domain);}
}
#endif
