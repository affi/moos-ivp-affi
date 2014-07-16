/************************************************************/
/*    NAME: Janille Maragh                                              */
/*    ORGN: MIT                                             */
/*    FILE: BHV_MyCircle.h                                      */
/*    DATE:                                                 */
/************************************************************/

#ifndef MyCircle_HEADER
#define MyCircle_HEADER

#include <string>
#include "IvPBehavior.h"
#include <math.h>
#include "OF_Coupler.h"
#include "ZAIC_PEAK.h"
#include "AngleUtils.h"

class BHV_MyCircle : public IvPBehavior {
public:
  BHV_MyCircle(IvPDomain);
  ~BHV_MyCircle() {};
  
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
    double          m_centre_x;
    double          m_centre_y;
    double          m_radius;
    double          m_num_segments;
    double          m_arrival_radius;
    double          m_targ_index;
    double          m_des_speed;
    
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
  {return new BHV_MyCircle(domain);}
}
#endif
