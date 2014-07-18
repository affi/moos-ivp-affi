/************************************************************/
/*    NAME: Janille Maragh                                              */
/*    ORGN: MIT                                             */
/*    FILE: BHV_MySinusoid.h                                      */
/*    DATE:                                                 */
/************************************************************/

#ifndef MySinusoid_HEADER
#define MySinusoid_HEADER

#include <string.h>
#include "IvPBehavior.h"
#include <math.h>
#include "OF_Coupler.h"
#include "ZAIC_PEAK.h"
#include "AngleUtils.h"
#include <algorithm>


class BHV_MySinusoid : public IvPBehavior {
public:
  BHV_MySinusoid(IvPDomain);
  ~BHV_MySinusoid() {};
  
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
    void            UpdateStateVariables();
    IvPFunction*    buildFunctionWithZAIC();
    
protected: // Configuration parameters
    double          m_offset_angle;
    double          m_start_x;
    double          m_start_y;
    double          m_amplitude;
    double          m_wavelength;
    double          m_disp_start_to_end;
    double          m_num_wpts;
    double          m_arrival_radius;
    double          m_des_speed;
    bool            m_trace_backwards;
    
    double          m_targ_index;
    vector<double>  m_x_pts;
    vector<double>  m_y_pts;

protected: // State variables
    double      m_osx;
    double      m_osy;
    double      m_targ_x;
    double      m_targ_y;
    
    double      m_dist_to_targ;
    double      m_ang_to_targ;
    
    bool        m_generate_path;
};

#define IVP_EXPORT_FUNCTION

extern "C" {
  IVP_EXPORT_FUNCTION IvPBehavior * createBehavior(std::string name, IvPDomain domain) 
  {return new BHV_MySinusoid(domain);}
}
#endif
