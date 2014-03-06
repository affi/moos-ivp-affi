/************************************************************/
/*    NAME: Janille Maragh                                  */
/*    ORGN: MIT                                             */
/*    FILE: CommunicationAngle.h                            */
/*    DATE: Wednesday, March 5, 2013                        */
/*    SMRY: This app reads in the current 3D position of    */
/*          own ship and the current 3D position of the     */
/*          collaborator ship. It then calculates the       */
/*          angle of elevation of the communication ray,    */
/*          which begins at own ship and ends that the      */
/*          collaboration ship. It also calculates the      */
/*          transmission loss during communication.         */
/************************************************************/

#ifndef CommunicationAngle_HEADER
#define CommunicationAngle_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include <cmath>

class CommunicationAngle : public CMOOSApp
{
    public:
    CommunicationAngle();
    ~CommunicationAngle();
    
    protected:
    bool OnNewMail(MOOSMSG_LIST &NewMail);
    bool Iterate();
    bool OnConnectToServer();
    bool OnStartUp();
    void RegisterVariables();
    
    // own ship coordinates
    double xo;
    double yo;
    double zo;
    
    // collaborator ship coordinates
    double xc;
    double yc;
    double zc;
    
    // target OS coordinates if no direct path exists
    double xt;
    double yt;
    double zt;
    
    // characteristic values, as described in instructions
    double theta0;
    double theta;
    double transmission_loss;
    double R;
    
    // constants
    double g;
    double co; //1480
    double deltatheta;
    double pi;
    
    // my functions
    void CalcElevAngle(double xo, double yo, double zo, double xc, double yc, double zc, double& R, double& theta0);
    double CalcTransmissionLoss(double theta, double theta0, double zo, double R);
    double rCalc(double R, double theta, double theta0);
    std::string toString(const double t);
    
    private: // Configuration variables
    double surface_sound_speed;
    double sound_speed_gradient;
    double water_depth;
    double time_interval;
    
    private: // State variables
    unsigned int m_iterations;
    double       m_timewarp;
};

#endif 
