#! /bin/sh

echo '#include "operation.h"'
perl -ne '
  if(/type_declare\(([0-9A-Za-z_]+)\)/){
    print("type_define($1);\n");
  }' "$@"
