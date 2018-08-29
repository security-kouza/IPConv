#! /bin/sh

perl -ne '
  while(/\bast_node\((AST_[0-9A-Za-z_]*)/){
    print($1,"\n");
    s/\bast_node\(AST_[0-9A-Za-z_]*//;
  }' "$@" | sort | uniq
