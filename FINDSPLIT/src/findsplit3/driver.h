#ifndef DRIVER_H
#define DRIVER_H

#include "crypto2014.h"
#include "naive.h"
#include "bruteforce.h"
#include "backtrack.h"
#include "montecarlo.h"
#include "linear.h"
#include <fstream>

namespace findsplit {
  using namespace std   ;
  using namespace boost ;

  struct driver_base :
    crypto2014, 
    naive, 
    bruteforce, 
    backtrack, 
    montecarlo,
    linear
  {
    integer_t     check_limit      = 10            ;
    pairing_graph G            [2]                 ;
    bool_t        is_generated [2] = {false,false} ;
  } ;

  struct driver : driver_base {
    void Check(integer_t limit_) ;
    pairing_graph & to_G(int i, const char * title) ;
    inline pairing_graph & to_G0(){return to_G(0,"F");} ;
    inline pairing_graph & to_G1(){return to_G(1,"H");} ;
  } ;

  ostream & operator<<(ostream& out, driver & g) ;
}

#endif /* DRIVER_H */
