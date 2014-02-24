/************************************************************/
/*    NAME: Janille Maragh                                              */
/*    ORGN: MIT                                             */
/*    FILE: PrimeFactor2.h                                          */
/*    DATE: February 19, 2014                                                */
/*    SMRY:
 /************************************************************/

#ifndef PrimeFactor2_HEADER
#define PrimeFactor2_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include <stdint.h>
#include <vector>
#include <stdlib.h>

using namespace std;

class PrimeFactor2 : public CMOOSApp
{
public:
    PrimeFactor2();
    ~PrimeFactor2();
    
protected:
    bool OnNewMail(MOOSMSG_LIST &NewMail);
    bool Iterate();
    bool OnConnectToServer();
    bool OnStartUp();
    void RegisterVariables();
    
    
    /*
     The struct NumberWithFeatures contains all the characteristics 
     associated with each received MOOS variable: NUM_VALUE. Its 
     parameters are defined as follows:
     
     orig_num:         the number received that is to be factorized
     
     current_num:      current_num is recalculated every time a prime
                       factor is found by dividing itself by that
                       prime factor. Initially current_num = orig_num
     
     index_received:   the nth NUM_VALUE to be received, where
                       n = index received
     
     index_calculated: the nth NUM_VALUE to have all its prime factors
                       calculated and published, n = index_calculated
     
     start_time:       the timestamp of receiving NUM_VALUE
     
     end_time:         the timestamp of the end of the calculation
     
     solve_time:       end_time - start_time
     
     prime_string:     the string of primes to be published
     
     username:         my username. always 'affi'
     */
    
    struct NumberWithFeatures {
        uint64_t orig_num;
        uint64_t current_num;
        int      index_received;
        int      index_calculated;
        double   start_time;
        double   end_time;
        double   solve_time;
        string   prime_string;
        string   username;
    };
    
    // vector which contains all the "NumberWithFeatures"s
    // this vector will be the buffer
    
    std::vector<struct NumberWithFeatures> num_values;
    
    int        calculations;    // total number of complete calculations/publications
    int        total_received;  // how many NUM_VALUEs received?
    string     outgoing_var;
    string     output_string;   // string to be published
    int        buffer_length;   // length of vector storing NumberWithFeatures's

private:
    
    // returns true if its argument is prime
    bool       IsPrime(int num);
    
    // finds smallest prime factor of its argument, updates current_num
    void       Factorize(struct NumberWithFeatures& item, int& num_rec);
    
    // converts double to string for easy string concatenation
    string     toString(const double t);
    
    // re-initializes a struct: NumberWithFeatures
    void       ClearNumberWithFeatures(struct NumberWithFeatures& item);
    
private: // State variables
    unsigned int m_iterations;
    double       m_timewarp;
};

#endif 
