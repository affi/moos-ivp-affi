/************************************************************/
/*    NAME: Janille Maragh                                  */
/*    ORGN: MIT                                             */
/*    FILE: BHV_ZigLeg.h                                    */
/*    DATE: May 14, 2014                                    */
/************************************************************/

#ifndef ZigLeg_HEADER
#define ZigLeg_HEADER

#include <string>
#include "IvPBehavior.h"
#include "XYRangePulse.h"
#include "ZAIC_PEAK.h"


class BHV_ZigLeg : public IvPBehavior {
public:
    BHV_ZigLeg(IvPDomain);
    ~BHV_ZigLeg() {};
    
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
    IvPFunction* buildFunctionWithZAIC();
    
protected: // Configuration parameters
    std::string  m_ipf_type;

    
protected: // State variables
    double       m_x;           // OS
    double       m_y;           // OS
    double       m_heading;     // OS
    double       m_wpt_time;
    int          m_wpt_index;
    bool         m_received;
    
    // for the zig
    bool         m_zigleg;
    double       m_zig_duration;
    double       m_zig_angle;
};

#define IVP_EXPORT_FUNCTION

extern "C" {
    IVP_EXPORT_FUNCTION IvPBehavior * createBehavior(std::string name, IvPDomain domain)
    {return new BHV_ZigLeg(domain);}
}
#endif
