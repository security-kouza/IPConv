#ifndef LINEAR_H
#define LINEAR_H

#include "optimizer.h"

namespace findsplit {
  using namespace std ;
  using namespace boost ;

  enum {anysolver, gurobi, scip, cbc, glpsol, lp_solve, nosolver} ;

  struct linear : virtual optimizer, score_storage_t {

    int    lp_mode             = anysolver ;
    string lp_filename         = "G.lp" ;
    string solution_filename   = "G.sol" ;

    real_t interval_to_solve   = 0 ;
    real_t interval_to_convert = 0 ;
    real_t interval_to_loadsol = 0 ;

    void IntegerPrograming(ostream & out) ;
    void IntegerPrograming(const string & filename) ;

    void IntegerProgramingOmitTrivial(ostream & out) ;
    void IntegerProgramingOmitTrivial(const string & filename) ;

    void IntegerProgramingCrypto2016(ostream & out) ;
    void IntegerProgramingCrypto2016(const string & filename) ;

    void LoadSolution(istream & in) ;
    void LoadSolution(const string & filename) ;

    void ExecuteSolver(void) ;
    void ExecuteSolverOmitTrivial(void) ;
    void ExecuteSolverCrypto2016(void) ;

  private:
    int  ExecuteSolverX(void) ;

  } ;

  ostream & operator<<(ostream& out, linear & g) ;
}

#endif /* LINEAR_H */
