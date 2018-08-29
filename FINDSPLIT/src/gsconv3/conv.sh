#! /bin/sh

./gsconv3 < $1.csdl > $1.gsconv          && \
../../gsconv < $1.gsconv > $1.tango      && \
../../findsplit --ratio=4 -b$1 $1.tango  && \
./gsconv3 $1.dot < $1.csdl > $1.result
