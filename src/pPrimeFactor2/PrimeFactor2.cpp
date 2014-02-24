/************************************************************/
/*    NAME: Janille Maragh                                  */
/*    ORGN: MIT 2.680                                       */
/*    FILE: PrimeFactor2.cpp                                */
/*    DATE: February 19, 2014                               */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "PrimeFactor2.h"

using namespace std;

//---------------------------------------------------------
// Constructor

PrimeFactor2::PrimeFactor2()
{
    buffer_length = 100;
    num_values.resize(buffer_length);
    
    calculations = 0;
    total_received = 0;
    
    for (int i = 0; i < buffer_length; i++) {
        num_values.at(i).orig_num          = 0;
        num_values.at(i).current_num       = 0;
        num_values.at(i).index_received    = 0;
        num_values.at(i).index_calculated  = 0;
        num_values.at(i).start_time        = 0;
        num_values.at(i).end_time          = 0;
        num_values.at(i).solve_time        = 0;
        num_values.at(i).prime_string      = "";
        num_values.at(i).username          = "affi";
    }
    
    outgoing_var = "";
    output_string = "";
    
    
    // state variables
    m_iterations = 0;
    m_timewarp   = 1;
}

//---------------------------------------------------------
// Destructor

PrimeFactor2::~PrimeFactor2()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool PrimeFactor2::OnNewMail(MOOSMSG_LIST &NewMail)
{
    MOOSMSG_LIST::iterator p;
    
    for(p=NewMail.begin(); p!=NewMail.end(); p++) {
        CMOOSMsg &msg = *p;
        
        if (msg.GetKey() == "NUM_VALUE") {
            total_received++;
            for (int i = 0; i < buffer_length; i++) {
                if (num_values.at(i).orig_num) {
                    cout << fixed;
                    cout.precision(1);
                    cout << "MOOS Time is " << MOOSTime() << std::endl;
                    num_values.at(i).start_time     = MOOSTime();
                    num_values.at(i).orig_num       = msg.GetDouble();
                    num_values.at(i).current_num    = msg.GetDouble();
                    num_values.at(i).index_received = total_received;
                    break;
                }
            }
        }
    }
    
    return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool PrimeFactor2::OnConnectToServer()
{
    RegisterVariables();
    return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool PrimeFactor2::Iterate()
{
    for (int i = 1; i < buffer_length; i++) {
        if(num_values.at(i).current_num == 1) {
            num_values.at(i).solve_time = num_values.at(i).end_time -
            num_values.at(i).start_time;
            output_string =
            "orig=" + toString(num_values.at(i).orig_num) +
            ",received=" + toString(num_values.at(i).index_calculated) +
            ",calculated=" + toString(num_values.at(i).solve_time) +
            ",primes=" + num_values.at(i).prime_string +
            ",username=" + num_values.at(i).username;
            
            // publish entire string as "PRIME_RESULT"
            m_Comms.Notify("PRIME_RESULT",output_string);
            ClearNumberWithFeatures(num_values.at(i));
        }
        else if ((num_values.at(i).current_num != 0) &&
                 (num_values.at(i).current_num != 1)) {
            Factorize(num_values.at(i),calculations);
        }
    }
    
    m_iterations++;
    return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool PrimeFactor2::OnStartUp()
{
    list<string> sParams;
    m_MissionReader.EnableVerbatimQuoting(false);
    if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
        list<string>::iterator p;
        for(p=sParams.begin(); p!=sParams.end(); p++) {
            string original_line = *p;
            string param = stripBlankEnds(toupper(biteString(*p, '=')));
            string value = stripBlankEnds(*p);
            
            if(param == "PRIME_RESULT")
                outgoing_var = value;
        }
    }
    
    m_timewarp = GetMOOSTimeWarp();
    
    RegisterVariables();
    return(true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void PrimeFactor2::RegisterVariables()
{
    m_Comms.Register("NUM_VALUE", 0);
}

//---------------------------------------------------------
// Procedure: IsPrime

bool PrimeFactor2::IsPrime(int num) {
    // 0 and 1 are not prime
    if ((num == 0) || (num == 1))
        return false;
    // has no factors besides itself and 1?
    for (int i = 2; i < (num/2); i++) {
        if ((num/i) == 0)
            return false;
        else
            return true;
    }
}

//---------------------------------------------------------
// Procedure: Factorize

void PrimeFactor2::Factorize(struct NumberWithFeatures& item, int& calc_num) {
    int i = 2;
    
    // increment i if not a factor of current_num
    while (item.current_num%i != 0) {
        i++;
    }
    // is the factor prime?
    if (IsPrime(i)) {
        // adds colon delimeter only if prime_string is not empty
        if(!item.prime_string.empty()) {
            item.prime_string += ":";
        }
        item.prime_string += toString(i);
    }
    // update current_num by dividing it by i
    item.current_num = (item.current_num)/i;
    
    if (item.current_num == 1) {
        calculations++;
        item.index_calculated = calculations;
    }
}

//---------------------------------------------------------
// Procedure: toString

std::string PrimeFactor2::toString(const double t) {
    // open output string stream, add argument, convert to string
    std::ostringstream ss;
    ss << t;
    return ss.str();
}

//---------------------------------------------------------
// Procedure: ClearNumberWithFeatures

void PrimeFactor2::ClearNumberWithFeatures(struct NumberWithFeatures& item) {
    
    item.orig_num           = 0;
    item.current_num        = 0;
    item.index_received     = 0;
    item.index_calculated   = 0;
    item.start_time         = 0;
    item.end_time           = 0;
    item.solve_time         = 0;
    item.prime_string       = "";
    item.username           = "affi";
}

