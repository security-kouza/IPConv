#ifndef CRYPTO2014_H
#define CRYPTO2014_H
#include "optimizer.h"

namespace findsplit {
  using namespace std ;
  using namespace boost ;

  struct crypto2014 : virtual optimizer, score_storage_t {
    void BruteForceCrypto2014(integer_t limit_);
  } ;
  
  ostream & operator<<(ostream& out, crypto2014 & g) ;
}

#endif /* CRYPTO2014_H */
