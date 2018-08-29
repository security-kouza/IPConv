#ifndef BACKTRACK_H
#define BACKTRACK_H
#include "optimizer.h"

namespace findsplit {
  using namespace std ;
  using namespace boost ;

  struct backtrack: virtual optimizer, score_storage_t {
    void BackTrack(integer_t limit_) ;
  } ;

  ostream & operator<<(ostream& out, backtrack & g) ;
}

#endif /* BACKTRACK_H */
