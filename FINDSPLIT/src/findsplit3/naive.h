#ifndef NAIVE_H
#define NAIVE_H
#include "optimizer.h"

namespace findsplit {
  using namespace std ;
  using namespace boost ;

  struct naive : virtual optimizer, score_storage_t {
    void NaiveBruteForce(integer_t limit_);
  } ;
  
  ostream & operator<<(ostream& out, naive & g) ;
}

#endif /* NAIVE_H */
