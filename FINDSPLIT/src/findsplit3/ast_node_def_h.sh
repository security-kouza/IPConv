#! /bin/sh

cat <<EOS
#ifndef AST_NODE_DEF_H
#define AST_NODE_DEF_H

#ifdef __cplusplus
extern "C" {
#endif

#define ast_node_declaration(name) extern char * name 

EOS

sed 's/\(^.*$\)/ast_node_declaration (\1) ;/' "$@"

cat <<EOS

#ifdef __cplusplus
}
#endif

#endif /* AST_NODE_DEF_H */
EOS
