#ifndef BRUTEFORCE_H
#define BRUTEFORCE_H
#include "optimizer.h"

namespace findsplit {
  using namespace std ;
  using namespace boost ;

  struct bruteforce: virtual optimizer, score_storage_t {
    void BruteForce(integer_t limit_);
  } ;

  ostream & operator<<(ostream& out, bruteforce & g) ;
}

#endif /* BRUTEFORCE_H */
