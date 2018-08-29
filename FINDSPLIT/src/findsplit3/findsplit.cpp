#include "driver.h"
#include "mysignal.h"
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>

using namespace findsplit ;

static void sec_print(ostream & out, real_t time){
  string unit = " sec" ;
  if(time >= 60){ time /= 60.0 ; unit = " min"  ;
    if(time >= 60){ time /= 60.0 ; unit = " hours";
      if(time >= 24){ time /= 24.0 ; unit = " days" ;
        if(time >= 30){ time /= 30.0 ; unit = " months" ;
          if(time >= 12){ time /=(365.0/30.0) ; unit = " years";
            if(time >=100){ time /=100.0 ; unit = " centuries";
              if(time >=10){ time /=10.0 ; unit = " millennia"; }}}}}}}
  out << time << unit  ;
}

static void usage(const string & message){
  if(message.size()){
    cerr << "option error: " << message << "." << endl ;
  }
  cout                                                            <<endl
  <<"usage: findsplit [options...] filename"                      <<endl
                                                                  <<endl
  <<"options for output filename:"                                <<endl
  <<"  -o{filename}: set output file name (default: G.dot)"       <<endl
  <<"  -f{filename}: set G0 file name (default: G0.dot)"          <<endl
  <<"  -h{filename}: set G1 file name (default: G1.dot)"          <<endl
  <<"  -l{filename}: set lp file name (default: G.lp)"            <<endl
  <<"  -s{filename}: set solution file name (default: G.sol)"     <<endl
  <<"  -b{basename}: set all of above basename.(dot,g0,g1,lp,sol)"<<endl
                                                                  <<endl
  <<"options for output format:"                                  <<endl
  <<"  --raw       : raw format"                                  <<endl
  <<"  --tango_dot : tango dot format"                            <<endl
  <<"  --tex       : dot2tex format (default)"                    <<endl
                                                                  <<endl
  <<"options for IP solver:"                                      <<endl
  <<"  --anysolver : use any one of the followings (default)"     <<endl
  <<"  --gurobi    : use gurobi_cl"                               <<endl
  <<"  --scip      : use scip"                                    <<endl
  <<"  --cbc       : use cbc"                                     <<endl
  <<"  --glpsol    : use glpsol"                                  <<endl
  <<"  --lp_solve  : use lp_solve"                                <<endl
  <<"  --nosolver  : just output lp file"                         <<endl
  <<"  --crypto2016: crypto2016 version (for comparison)"         <<endl
  <<"  --omittrivial: omit trivial const (for comparison)"        <<endl
                                                                  <<endl
  <<"options for backtracking:"                                   <<endl
  <<"  --montecarlo: Monte Carlo method (2^n)"                    <<endl
  <<"  --bruteforce: backtrack without pruning test"              <<endl
  <<"  --backtrack : backtrack with pruning test"                 <<endl
  <<"  --naive     : same as bruteforce, but naive implementation (for debug)" <<endl
  <<"  --crypto2014: crypto2014 version (for comparison)"         <<endl
  <<"  --unlimit   : unlimited backtrack mode"                    <<endl
  <<"  --limit=n   : set backtrack-limit = 2^n (n>=0)"            <<endl
  <<"  --limit     : set backtrack-limit = 2^22 (default)"        <<endl
                                                                  <<endl
  <<"options for optimality check:"                               <<endl
  <<"  --check=n   : set check-limit = 2^n (n>=0)"                <<endl
  <<"  --check     : set check-limit = 2^16"                      <<endl
  <<"  --nocheck   : do not check (default)"                      <<endl
                                                                  <<endl
  <<"other options:"                                              <<endl
  <<"  --ratio=r   : set G1/G0-ratio = r (>=0,default: 1.0)"      <<endl
  <<"  --ratio     : set G1/G0-ratio = 2.0"                       <<endl
  <<"  --visible   : show constraint nodes (default)"             <<endl
  <<"  --invisible : hide constraint nodes"                       <<endl
  <<"  --          : end of options"                              <<endl
  ;
  exit(1) ;
}

template<typename number>struct ston_traits {
  static inline number ston(const string & s){return number(s);}
};
template<>struct ston_traits<long double> {
  static inline long double ston(const string & s){return stold(s);}
};
template<>struct ston_traits<int> {
  static inline int ston(const string & s){return stoi(s);}
};
template<>struct ston_traits<long long> {
  static inline long long ston(const string & s){return stoll(s);}
};
template<typename number>
void ston(number & n, const string & s, const string & name){
  try{
    n = ston_traits<number>::ston(s) ;
    if(n < 0)throw std::exception();
  }catch(const std::exception & e){
    usage("invalid "+name+": "+s);
  }
}

int main(int argc, char **argv) {
  int       mode        = 1        ;
  int       lp_mode     = anysolver;
  real_t    ratio       = 1.0      ;
  int       format      = tex      ;
  int       limit       = 22       ;
  int       check_limit = -1       ;
  bool_t    visible     = true     ;

  string   filename     = ""       ;
  string   o_file       = "G.dot"  ;
  string   f_file       = "G0.dot" ;
  string   h_file       = "G1.dot" ;
  string   l_file       = "G.lp"   ;
  string   s_file       = "G.sol"  ;

  if(argc<=1){
    usage("") ;
  }else{
    int opt = 1 ;
    for(int i=1;i<argc;i++){
      string argn  = argv[i] ;
      string arg1  = "" ;
      string arg2  = "" ;
      string rest2 = "" ;
      string arg7  = "" ;
      string rest7 = "" ;
      string arg8  = "" ;
      string rest8 = "" ;
      try{
        arg1  = argn.substr(0,1) ;
        arg2  = argn.substr(0,2) ;
        rest2 = argn.substr(2)   ;
        arg7  = argn.substr(0,7) ;
        rest7 = argn.substr(7)   ;
        arg8  = argn.substr(0,8) ;
        rest8 = argn.substr(8)   ;
      }catch(std::exception){}
      if(!opt){
        filename = argn;
      }else{
        /* options for output filename: */
        /**/ if(arg2=="-o"          ) o_file   = rest2 ;
        else if(arg2=="-f"          ) f_file   = rest2 ;
        else if(arg2=="-h"          ) h_file   = rest2 ;
        else if(arg2=="-l"          ) l_file   = rest2 ;
        else if(arg2=="-s"          ) s_file   = rest2 ;
        else if(arg2=="-b"          ){
	  o_file   = rest2 + ".dot" ;
          f_file   = rest2 + ".g0"  ;
          h_file   = rest2 + ".g1"  ;
          l_file   = rest2 + ".lp"  ;
          s_file   = rest2 + ".sol" ;
        }
        /* options for output format: */
        else if(argn=="--raw"       ) format   = raw   ;
        else if(argn=="--tango_dot" ) format   = tango ;
        else if(argn=="--tex"       ) format   = tex   ;

        /* options for integer programing: */
        else if(argn=="--anysolver" ) lp_mode  = anysolver;
        else if(argn=="--gurobi"    ) lp_mode  = gurobi   ;
        else if(argn=="--scip"      ) lp_mode  = scip     ;
        else if(argn=="--cbc"       ) lp_mode  = cbc      ;
        else if(argn=="--glpsol"    ) lp_mode  = glpsol   ;
        else if(argn=="--lp_solve"  ) lp_mode  = lp_solve ;
        else if(argn=="--nosolver"  ) lp_mode  = nosolver ;
        else if(argn=="--crypto2016") mode     = 7 ;
        else if(argn=="--omittrivial")mode     = 8 ;

        /* options for brute force method: */
        else if(argn=="--montecarlo") mode     = 2 ;
        else if(argn=="--bruteforce") mode     = 5 ;
        else if(argn=="--backtrack" ) mode     = 4 ;
        else if(argn=="--naive"     ) mode     = 3 ;
        else if(argn=="--crypto2014") mode     = 6 ;
        else if(argn=="--unlimit"   ) limit    = -1 ;
        else if(arg8=="--limit="    ) ston(limit,rest8,"limit");
        else if(arg7=="--limit"     ) limit    = 22 ;

        /* options for optimality checking: */
        else if(arg8=="--check="    ) ston(check_limit,rest8,"check");
        else if(arg7=="--check"     ) check_limit = 16 ;
        else if(argn=="--nocheck"   ) check_limit = -1 ;

        /* other options: */
        else if(arg8=="--ratio="    ) ston(ratio,rest8,"ratio");
        else if(arg7=="--ratio"     ) ratio    = 2.0   ;
        else if(argn=="--visible"   ) visible  = true  ;
        else if(argn=="--invisible" ) visible  = false ;
        else if(argn=="--"          ) opt      = 0     ;

        /* otherwise: */
        else if(arg1=="-"           ) usage("unknown option: " + argn);
        else                          filename = argn ;
      }
    }
  }

  if(freopen(filename.c_str(),"r",stdin)==NULL){
    fatal_error("fail to open " + filename);
  }

  driver g ;

  cerr << "success to load/decide." << endl ;
  cerr << "time to load     = " ;
  sec_print(cerr, g.interval_to_load);
  cerr << endl ;
  cerr << "time to decide   = " ;
  sec_print(cerr, g.interval_to_decide);
  cerr << " (includes gauss)" << endl ;
  cerr << "time to gauss    = " ;
  sec_print(cerr, g.interval_to_gauss);
  cerr << endl ;

  struct rusage rusage ;
  real_t start, end, interval, interval_to_save ;

  g.lp_mode           = lp_mode ;
  g.lp_filename       = l_file ;
  g.solution_filename = s_file ;

// ==========================================
// split
// ==========================================

  signal_handler_init() ;
  start = now();
  if(g.is_consistent()){
    g.SetRatio(ratio) ;
    /**/ if(mode == 1) g.ExecuteSolver           (      ) ;
    else if(mode == 2) g.CoinFlipping            (limit ) ;
    else if(mode == 3) g.NaiveBruteForce         (limit ) ;
    else if(mode == 4) g.BackTrack               (limit ) ;
    else if(mode == 5) g.BruteForce              (limit ) ;
    else if(mode == 6) g.BruteForceCrypto2014    (limit ) ;
    else if(mode == 7) g.ExecuteSolverCrypto2016 (      ) ;
    else if(mode == 8) g.ExecuteSolverOmitTrivial(      ) ;
    else unexpected_error("unknown mode");
  }
  end = now();
  interval = end - start ;

  if(g.is_split) g.Check(check_limit) ;

// ==========================================
// G.dot
// ==========================================

  ofstream Gout(o_file) ;
  if(!Gout.good())fatal_error("fail to open " + o_file) ;

  g.format          = format  ;
  g.no_dup          = true    ;
  g.show_constraint = visible ;

  start = now();
  Gout << g ;

// ==========================================
// G0.dot, G1.dot
// ==========================================

  if(g.is_split){
    ofstream G0out(f_file) ;
    if(!G0out.good())fatal_error("fail to open " + f_file) ;
    ofstream G1out(h_file) ;
    if(!G1out.good())fatal_error("fail to open " + h_file) ;
    G0out << g.to_G0() ;
    G1out << g.to_G1() ;
  }
  end = now();
  interval_to_save = end - start ;

// ==========================================
//
// ==========================================

  switch(g.status){
    case success:
      cerr << "success to split." << endl ;
      if((mode == 1)||(mode == 7)||(mode == 8)){
        cerr << "time to convert  = " ;
        sec_print(cerr, g.interval_to_convert) ;
        cerr << endl ;
        cerr << "time to solve    = " ;
        sec_print(cerr, g.interval_to_solve) ;
        cerr << endl ;
        cerr << "time to loadsol  = " ;
        sec_print(cerr, g.interval_to_loadsol) ;
        cerr << endl ;
      }
      cerr << "performance time = " ;
      sec_print(cerr, interval);
      cerr << endl ;
      cerr << "  (= convert + solve + loadsol)" << endl ;
      cerr << "time to save     = " ;
      sec_print(cerr, interval_to_save);
      cerr << endl ;
      return 0 ;
    break ;
    case cannot_optimize:
      cerr << "success to split but fail to optimize."  << endl ;
      cerr << "progress rate = " << g.progress_rate     << endl ;
      cerr << "performance time = "; sec_print(cerr, interval); cerr<<endl;
      cerr << "estimated time to complete = " ; 
      {
        real_t time = interval/g.progress_rate ;
        sec_print(cerr, time);
      }
      if(mode==4){
        cerr << " (is utterly unreliable."
	        " pruning may accelerate the latter search)" ;
      }
      cerr << endl ;
      return 1 ;
    break ;
    case cannot_split:
      cerr << "fail to split." << endl ;
      exit(1) ;
      return 1 ;
    break ;
  }
}
