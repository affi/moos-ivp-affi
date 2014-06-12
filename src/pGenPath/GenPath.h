/************************************************************/
/*    NAME: Janille Maragh                                              */
/*    ORGN: MIT                                             */
/*    FILE: GenPath.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef GenPath_HEADER
#define GenPath_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include "XYSegList.h"
#include <vector>
#include <math.h>
#include <stdlib.h>

class GenPath : public CMOOSApp
{
public:
    GenPath();
    ~GenPath();
    
protected:
    bool OnNewMail(MOOSMSG_LIST &NewMail);
    bool Iterate();
    bool OnConnectToServer();
    bool OnStartUp();
    void RegisterVariables();
    
    
    
private: // Configuration variables
    struct Point {
        bool    empty;
        double  x;
        double  y;
        int     id;
        std::string  stringVal;
    };
    
    std::vector<struct Point> m_points;
    std::vector<struct Point> m_points_ordered;
    
    bool                        assign_by_region;
    int                         m_bufferlength;
    XYSegList                   m_seglist;
    
    // own-ship information
    double                      m_osx;
    double                      m_osy;
    
    std::string                 variableOut;
    std::string                 temp_string;
    std::vector<std::string>    svector;
    std::string                 update_str;
    
    void ResetPoint(struct Point& point);

    
    
private: // State variables
    unsigned int m_iterations;
    double       m_timewarp;
    
    
};

#endif 
