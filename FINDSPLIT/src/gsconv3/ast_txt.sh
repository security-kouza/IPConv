#! /bin/sh

perl -ne '
  while(
    /\blist\((AST_[0-9A-Za-z_]*)\s*,\s*location/
  ){
    print($1,"\n");
    s/\blist\(AST_[0-9A-Za-z_]*\s*,\s*location//;
  }' "$@" | sort | uniq
