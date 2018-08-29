#ifndef MONTECARLO_H
#define MONTECARLO_H

#include "optimizer.h"

namespace findsplit {
  using namespace std ;
  using namespace boost ;

  struct montecarlo : virtual optimizer, score_storage_t {
    void CoinFlipping(integer_t limit_, bool omit_warnings = false) ;
  } ;

  ostream & operator<<(ostream& out, montecarlo & g) ;

}

#endif /* MONTECARLO_H */
