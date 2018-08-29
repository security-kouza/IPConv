#! /bin/sh

cat <<EOS
#include "ast.h"

#define ast_node_define(name) char * name = # name

EOS

sed 's/\(^.*$\)/ast_node_define (\1) ;/' "$@"
