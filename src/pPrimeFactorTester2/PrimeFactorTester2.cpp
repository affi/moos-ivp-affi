/************************************************************/
/*    NAME: Janille Maragh                                  */
/*    ORGN: MIT                                             */
/*    FILE: PrimeFactorTester2.cpp                          */
/*    DATE: February 24, 2014                               */
/*    SUMMARY: This application subscribes to the MOOS      */
/*    variable "PRIME_RESULT", which is generated by the    */
/*    app pPrimeFactor2. It parses the string "PRIME_RESULT"*/
/*    for a number to be factorized (orig) and the string   */
/*    of its primes (also generated by pPrimeFactor). This  */
/*    app then generates the string of primes again, then   */
/*    compares it to pPrimeFactor's string of primes. It    */
/*    then publishes a new string, which is the received    */
/*    string concatenated with a statement: ",valid=true"   */
/*    if the strings of primes match up or ",valid=false"   */
/*    if they do not.                                       */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "PrimeFactorTester2.h"

using namespace std;

//---------------------------------------------------------
// Constructor

PrimeFactorTester2::PrimeFactorTester2()
{
    buffer_length = 100;
    nums.resize(buffer_length);
    
    for (int i = 0; i < buffer_length; i++) {
        nums.at(i).original_value         = 0;
        nums.at(i).current_num            = 0;
        nums.at(i).string_in              = "";
        nums.at(i).prime_string_in        = "";
        nums.at(i).prime_string_test      = "";
        nums.at(i).string_out             = "";
    }
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
        cout << "------ Start of Mail for loop\n";
        
        CMOOSMsg &msg = *p;
        
        if (msg.GetKey() == "PRIME_RESULT") {
            cout << "----- Start of check for PRIME_RESULT\n";
            for (int i = 0; i < buffer_length; i++) {
                if (nums.at(i).original_value == 0) {
                    nums.at(i).string_in = msg.GetString();
                    
                    // parse incoming string, comma as a delimeter
                    svector         = parseString(nums.at(i).string_in,",");
                    
                    // remove "orig=" from first item in svector
                    temp_string     = stripBlankEnds(biteString(svector[0],'='));
                    
                    // convert the original value (string) to unsigned long
                    nums.at(i).original_value  = strtoul((svector[0]).c_str(),NULL,0);
                    nums.at(i).current_num     = nums.at(i).original_value;
                    
                    // remove "primes:" to get only string of primes
                    temp_string = stripBlankEnds(biteString(svector[3],'='));
                    
                    nums.at(i).prime_string_in = svector[3];
                    cout << "primes in " << nums.at(i).prime_string_in << endl;
                    
                    break;
                }
            }
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
    for (int i = 1; i < buffer_length; i++) {
        
        Factorize(nums.at(i));
        
        if (nums.at(i).current_num == 1) {
            
            // if they match, string was valid, else it was not
            if (nums.at(i).prime_string_in == nums.at(i).prime_string_test)
                nums.at(i).string_out = nums.at(i).string_in + ",valid=true";
            else
                nums.at(i).string_out = nums.at(i).string_in + ",valid=false";
            
            m_Comms.Notify("PRIME_RESULT_VALID", nums.at(i).string_out);
            ClearNumWithFeatures(nums.at(i));
        }
    }
    
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

void PrimeFactorTester2::Factorize(struct NumWithFeatures& item) {
    uint64_t i = 2;
    
    // increment i if not a factor of current_num
    while (item.current_num%i != 0)
        i++;
    
    
    // is the factor prime?
    if (IsPrime(i)) {
        
        // adds colon delimeter only if prime_string is not empty
        if(!item.prime_string_test.empty())
            item.prime_string_test += ":";
        
        item.prime_string_test += toString(i);
    }
    
    // update current_num by dividing it by i
    item.current_num = (item.current_num)/i;
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

//---------------------------------------------------------
// Procedure: ClearNumWithFeatures

void PrimeFactorTester2::ClearNumWithFeatures(struct NumWithFeatures& item) {
    
    item.original_value         = 0;
    item.current_num            = 0;
    item.string_in              = "";
    item.prime_string_in        = "";
    item.prime_string_test      = "";
    item.string_out             = "";
}

