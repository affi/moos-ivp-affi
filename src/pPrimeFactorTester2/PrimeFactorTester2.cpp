/************************************************************/
/*    NAME: Janille Maragh                                              */
/*    ORGN: MIT                                             */
/*    FILE: PrimeFactorTester2.cpp                                        */
/*    DATE: February 24, 2014   
 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "PrimeFactorTester2.h"

using namespace std;

//---------------------------------------------------------
// Constructor

PrimeFactorTester2::PrimeFactorTester2()
{
    received_string   = "";
    prime_string_in   = "";
    outgoing_var      = "";
    prime_string_test = "";
    outgoing_string   = "";
    original_value    = 0;
    
    m_iterations      = 0;
    m_timewarp        = 1;
}

//---------------------------------------------------------
// Destructor

PrimeFactorTester2::~PrimeFactorTester2()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool PrimeFactorTester2::OnNewMail(MOOSMSG_LIST &NewMail)
{
    MOOSMSG_LIST::iterator p;
    string temp_string;
    for(p=NewMail.begin(); p!=NewMail.end(); p++) {
        CMOOSMsg &msg = *p;
        
        if (msg.GetKey() == "PRIME_RESULT") {
            received_string = msg.GetString();
            received_string_copy = received_string;
            
            // parse incoming string, comma as a delimeter
            vector<string> svector = parseString(received_string, ',');
    
//            svector         = parseString(received_string,",");
            
            // remove "orig=" from first item in svector
            temp_string     = stripBlankEnds(biteString(svector[0],'='));
            
            // convert the original value (string) to unsigned long
            original_value  = strtoul(temp_string.c_str(),NULL,0);
            
            // remove "primes:" to get only string of primes
            prime_string_in = stripBlankEnds(biteString(svector[4],'='));
        }
    }
	
    return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool PrimeFactorTester2::OnConnectToServer()
{
    RegisterVariables();
    return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool PrimeFactorTester2::Iterate()
{
    cout << "Tester" << received_string_copy << endl;
    Factorize(original_value);
    
    if (prime_string_in == prime_string_test)
        outgoing_string = received_string_copy + ",valid=true";
    else
        outgoing_string = received_string_copy + "'valid=false";
    
    m_Comms.Notify("PRIME_RESULT_VALID", outgoing_string);
    
    m_iterations++;
    return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool PrimeFactorTester2::OnStartUp()
{
    list<string> sParams;
    m_MissionReader.EnableVerbatimQuoting(false);
    if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
        list<string>::iterator p;
        for(p=sParams.begin(); p!=sParams.end(); p++) {
            string original_line = *p;
            string param = stripBlankEnds(toupper(biteString(*p, '=')));
            string value = stripBlankEnds(*p);
            
            if(param == "FOO") {
                outgoing_var = value;
            }
        }
    }
    
    m_timewarp = GetMOOSTimeWarp();
    
    RegisterVariables();
    return(true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void PrimeFactorTester2::RegisterVariables()
{
    m_Comms.Register("PRIME_RESULT", 0);
}

//---------------------------------------------------------
// Procedure: Factorize

void PrimeFactorTester2::Factorize(uint64_t num) {
    for (int i = 2; i < (num/2); i++) {
        if ( (((num/2)/i) == 0) && (IsPrime(i)) ) {
            prime_string_test += (toString(i) + ":");
        }
    }
}

//---------------------------------------------------------
// Procedure: IsPrime

bool PrimeFactorTester2::IsPrime(int num) {
    if ((num == 0) || (num == 1))
        return false;
    for (int i = 2; i < (num/2); i++) {
        if ((num/i) == 0)
            return false;
        else
            return true;
    }
}

//---------------------------------------------------------
// Procedure: to String

string PrimeFactorTester2::toString(const int t) {
    std::ostringstream ss;
    ss << t;
    return ss.str();
}
