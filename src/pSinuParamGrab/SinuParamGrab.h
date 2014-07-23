/*************************************************************/
/*    NAME: Janille Maragh                                   */
/*    ORGN: MIT                                              */
/*    FILE: SinuParamGrab.h                                  */
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

#ifndef SinuParamGrab_HEADER
#define SinuParamGrab_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include <math.h>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

using namespace std;

class SinuParamGrab : public CMOOSApp
{
 public:
   SinuParamGrab();
   ~SinuParamGrab();

 protected:
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();
   void RegisterVariables();

 private: // Configuration variables
    
    // from parsing UCTD_MSMNT_UPDATE
    vector<double>  m_x_pts;
    vector<double>  m_y_pts;
    vector<double>  m_temps;
    
    // from parsing UCTD_PARAMETER_ESTIMATE
    vector<double>  m_wavelengths;
    vector<double>  m_angles;
    vector<double>  m_amplitudes;
    
    double VectorAverage(vector<double> myVector);
    string toString (const double t);
    
    
 private: // State variables
    double      m_max_temp;
    double      m_min_temp;
    double      m_avg_temp;
    double      m_x_at_avg;
    double      m_y_at_avg;
    
    double      m_avg_wavelength;
    double      m_avg_amplitude;
    double      m_avg_angle;
    
    string      m_out_angle;
    string      m_out_amplitude;
    string      m_out_wavelength;
    
    double      m_min_dev_from_avg;
    int         m_min_dev_from_avg_idx;
    
    string      m_x_string;
    string      m_y_string;
    string      m_angle_string;
    string      m_amplitude_string;
    string      m_wavelength_string;
    string      m_reverse_string;
    
   unsigned int m_iterations;
   double       m_timewarp;
};

#endif 
