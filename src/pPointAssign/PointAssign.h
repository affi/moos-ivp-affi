/************************************************************/
/*    NAME: Janille Maragh                                              */
/*    ORGN: MIT                                             */
/*    FILE: PointAssign.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef PointAssign_HEADER
#define PointAssign_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include <stdlib.h>
#include <math.h>
#include <iterator>

class PointAssign : public CMOOSApp
{
public:
    PointAssign();
    ~PointAssign();
    
protected:
    bool OnNewMail(MOOSMSG_LIST &NewMail);
    bool Iterate();
    bool OnConnectToServer();
    bool OnStartUp();
    void RegisterVariables();

    struct Point {
        bool    empty;
        double  x;
        double  y;
        int     id;
        std::string  stringVal;
    };
    
    std::vector<struct Point> m_Points;
    
    bool                        assign_by_region;
    int                         m_bufferlength;
    std::string                 variableOut;
    std::string                 temp_string;
    std::vector<std::string>    svector;
    
    
private: // Configuration variables
//    string  toString(const double t);
    
private: // State variables
    unsigned int m_iterations;
    double       m_timewarp;
};

#endif 
