#include "pairing_graph.h"
#include <iostream>

namespace findsplit {

using namespace std   ;
using namespace boost ;

#define g (*this)

int pairing_graph::devnull ;

int pairing_graph::not_found(const string & symbol, int & i){
  boost::unordered_map<string, int>::const_iterator f
    = symbol_to_number.find(symbol) ;
  if(f == symbol_to_number.end()){
    i = -1 ;
    return 1 ;
  }else{
    i = f->second ;
    return 0 ;
  }
}

vertex_t pairing_graph::register_symbol(
  const string & symbol,
  const string & texlabel
){
  if(not_found(symbol)){
    vertex_t v = add_vertex(g) ;
    number_to_vertex.push_back(v) ;
    symbol_to_number[symbol] = num_nodes   ;
    g[v].number              = num_nodes++ ;
    g[v].symbol              = symbol      ;
    g[v].texlabel            = texlabel    ;
    return v ;
  }else{
    int i = symbol_to_number[symbol] ;
    return number_to_vertex[i] ;
  }
}

void pairing_graph::register_pairing(
  const string & symbol0,
  const string & symbol1
){
  vertex_t v0 = symbol_to_vertex(symbol0) ;
  vertex_t v1 = symbol_to_vertex(symbol1) ;

  string head0 = symbol0 + "," + symbol1 ;
  string name0 = head0 + "[0]" ;

  if(not_found(name0)){

    // ==========================================================
    // [0]
    // ==========================================================

    string head1     = symbol1 + "," + symbol0 ;
    string head2     = "p"  + to_string(++num_pairings) ;
    string head3     = "p_{" + to_string(num_pairings) + "}";
    string name1     = head1 + "[0]" ;
    string name2     = head2 + "[0]" ;
    string texlabel2 = "$" + head3 + "[0]" + "$" ;

    register_alias(name0) ;
    register_alias(name1) ;
    vertex_t p0 = register_symbol(name2,texlabel2) ;
    g[p0].is_pairing    = true ;
    g[p0].pairing_id    = num_pairings ;
    g[p0].pairing_subid = 0 ;

    add_edge(v0, p0, g) ;

    // ==========================================================
    // [1]
    // ==========================================================

    string name3     = head0 + "[1]" ;
    string name4     = head1 + "[1]" ;
    string name5     = head2 + "[1]" ;
    string texlabel5 = "$" + head3 + "[1]" + "$" ;

    register_alias(name3) ;
    register_alias(name4) ;
    vertex_t p1 = register_symbol(name5,texlabel5) ;
    g[p1].is_pairing    = true ;
    g[p1].pairing_id    = num_pairings ;
    g[p1].pairing_subid = 1 ;

    add_edge(v1, p1, g) ;

    // ==========================================================
    // [0],[1]
    // ==========================================================

    g[p0].pair_id = g[p1].number ;
    g[p1].pair_id = g[p0].number ;

  }
}

vertex_t pairing_graph::register_constraint(){
  string symbol   = "c["  + to_string(++num_constraints) + "]" ;
  string texlabel = "$" + symbol + "$" ;
  return register_symbol(symbol,texlabel) ;
}

#undef g

#define out_keyval(v,field) out << # field "=" << v.field
#define out_bool(v,field) if(v.field) out << # field

ostream & operator<<(
  ostream& out, const pairing_graph_vertex_property & v
){
  out_keyval(v,number)                  << "," ;
  out_keyval(v,symbol)                  << "," ;
  out_keyval(v,texlabel)                << "," ;
  out_bool(v,is_normal())               << "," ;
  out_bool(v,is_pairing)                << "," ;
  out_bool(v,is_bottom)                 << "," ;
  out_bool(v,is_prohibited)             << "," ;
  out_bool(v,is_constraint)             << "," ;
  out_bool(v,is_cycle)                  << "," ;
  out_bool(v,is_fix)                    << "," ;
  out_bool(v,is_G[0])                   << "," ;
  out_bool(v,is_G[1])                   << "," ;
  out_bool(v,is_trivial)                << "," ;
  if(v.value!=-1) out_keyval(v,value)   << "," ;
  out_keyval(v,weight)                  << "," ;
  out_keyval(v,priority)                << "," ;
  out_keyval(v,vartype)                 << "," ;
  if(v.is_pairing){
    out_keyval(v,pairing_id)            << "," ;
    out_keyval(v,pairing_subid)         << "," ;
    out_keyval(v,pair_id)               << "," ;
  }
  return out ;
}

static void output_raw_dot(ostream& out, const pairing_graph & g){
  graph_traits<pairing_graph>::vertex_iterator vi, vi_end ;
  graph_traits<pairing_graph>::adjacency_iterator ai, ai_end ;

  out << "digraph " << g.graph_title << "{" << endl ;
  for(tie(vi, vi_end) = vertices(g); vi != vi_end; vi++){
    if( ! g[*vi].is_constraint || g.show_constraint ){
      out << "  " ;
      out << g[*vi].number << " [label = \"" << g[*vi].symbol << "\"" ;
      if(g[*vi].is_pairing               )out << ";shape = Mrecord" ;
      if((!g.no_dup) && g[*vi].is_G[0] && g[*vi].is_G[1])
        out << ";peripheries = 2" ;
      out << "]; " ;
  
      for(tie(ai,ai_end)=adjacent_vertices(*vi,g); ai!=ai_end; ai++){
        if( ! g[*ai].is_constraint || g.show_constraint ){
          out << g[*vi].number << "->" << g[*ai].number << ";" ;
        }
      }
      out << endl ;
    }
  }
  out << "  " ;
  out << "{rank=same" ;
  for(tie(vi, vi_end) = vertices(g); vi != vi_end; vi++){
    if(g[*vi].is_pairing||(g.show_constraint && g[*vi].is_constraint)){
      out << ";" << g[*vi].number ;
    }
  }
  out << "}" << endl ;
  out << "}" << endl ;
}

string replace_all(
  const string & a, // with
  const string & b, // in
  const string & c
){
  string result = c ;
  size_t index  = 0 ;
  size_t a_len  = a.size() ;
  size_t b_len  = b.size() ;
  while((index = result.find(a,index))!=string::npos){
    result.replace(index, a_len, b);
    index += b_len ;
  }
  return result ;
}

static string changeDash(
  const string & node,
  boost::unordered_map<string, int> & dashes
){
  if((node.find("'")!=string::npos) && (!dashes[node])){
    string node2 = replace_all("'","_dash",node) ;
    return node2 + " [label = \"" + node + "\"];" ;
  }
  return "" ;
}

static void output_tango_dot(ostream& out, const pairing_graph & g){
  graph_traits<pairing_graph>::vertex_iterator vi, vi_end ;
  graph_traits<pairing_graph>::adjacency_iterator ai, ai_end ;
  string dot_text = "" ;
  boost::unordered_map<string, int> dashes ;

  dot_text = "digraph " + g.graph_title + "{" ;

  for(tie(vi, vi_end) = vertices(g); vi != vi_end; vi++){
    if( ! g[*vi].is_constraint || g.show_constraint ){
      string src = g[*vi].is_pairing ?
        to_string(
          g[*vi].pairing_id * 10 + g[*vi].pairing_subid
        ) : g[*vi].symbol ;
      for(tie(ai,ai_end)=adjacent_vertices(*vi,g); ai!=ai_end; ai++){
        if( ! g[*ai].is_constraint || g.show_constraint ){
          string dst = g[*ai].is_pairing ?
            to_string(
              g[*ai].pairing_id * 10 + g[*ai].pairing_subid 
            ) : g[*ai].symbol ;
          dot_text += changeDash(src, dashes) ;
          dot_text += changeDash(dst, dashes) ;
          string src2 = replace_all("'","_dash",src) ;
          string dst2 = replace_all("'","_dash",dst) ;
          dot_text += src2 + " -> " + dst2 + ";" ;
        }
      }
      if(g[*vi].is_pairing){
        dot_text += src + " [shape = Mrecord;label = \""
                        + g[*vi].symbol + "\"];" ;
      }
      if((!g.no_dup) && g[*vi].is_G[0] && g[*vi].is_G[1]){
        dot_text += src + " [peripheries = 2];" ;
      }
    }
  }
  dot_text += "}" ;
  out << dot_text << endl ;
}

static const char * tex_dot_header =
"        d2tdocpreamble = \"\\usetikzlibrary{shapes,arrows}\";\n"
"	d2tfigpreamble = \"\n"
"	\\tikzstyle{n}=[draw,shape=circle,minimum size=2.6em,inner sep=0pt];\n"
"	\\tikzstyle{nd}=[draw,shape=circle,double,minimum size=2.6em,inner sep=0pt];\n"
"	\\tikzstyle{a}=[draw,shape=circle,minimum size=2.6em,inner sep=0pt,fill=red!20];\n"
"	\\tikzstyle{ad}=[draw,shape=circle,double,minimum size=2.6em,inner sep=0pt,fill=red!20];\n"
"	\\tikzstyle{p}=[draw,shape=rectangle,rounded corners,minimum size=2.6em, inner sep=0pt, fill=gray!10];\"\n"
"//graph [concentrate=true];\n"
"node [style=\"n\"];\n"
"edge [style=\"-latex\"];\n"
"\n" ;

string symbol_to_texlabel(const string & symbol){
  string buf = symbol ;
  buf = replace_all("$","\\$",buf) ;
  buf = replace_all("_","\\_",buf) ;
  return buf ;
}

static void output_tex_dot(ostream& out, const pairing_graph & g){
  graph_traits<pairing_graph>::vertex_iterator vi, vi_end ;
  graph_traits<pairing_graph>::adjacency_iterator ai, ai_end ;
  string dot_text = 
    "digraph " + g.graph_title + "{\n" + tex_dot_header ;
  for(tie(vi, vi_end) = vertices(g); vi != vi_end; vi++){
    if( ! g[*vi].is_constraint || g.show_constraint ){
      string src = to_string(g[*vi].number) ;
      string texlabel = (g[*vi].texlabel=="") ?
	  symbol_to_texlabel(g[*vi].symbol) : g[*vi].texlabel ;

      dot_text += src + " [texlbl=\"" + texlabel + "\"";
  
      if(g[*vi].is_pairing || g[*vi].is_constraint)
        dot_text += " ,style=\"p\"" ;
  
      if((!g.no_dup) && g[*vi].is_G[0] && g[*vi].is_G[1])
        dot_text += " ,style=\"nd\"" ;
  
      dot_text += "];" ;
      for(tie(ai,ai_end)=adjacent_vertices(*vi,g); ai!=ai_end; ai++){
        if( ! g[*ai].is_constraint || g.show_constraint ){
          string dst = to_string(g[*ai].number) ;
          dot_text += src + "->" + dst + ";" ;
        }
      }
      dot_text += "\n" ;
    }
  }

  dot_text += "{rank=same" ;
  for(tie(vi, vi_end) = vertices(g); vi != vi_end; vi++){
    if(g[*vi].is_pairing||(g.show_constraint && g[*vi].is_constraint)){
      dot_text += ";" + to_string(g[*vi].number) ;
    }
  }
  dot_text += "}\n" ;
  dot_text += "}\n" ;
  out << dot_text ;
}

static void output_detail(ostream& out, const pairing_graph & g){
  graph_traits<pairing_graph>::vertex_iterator vi, vi_end ;
  graph_traits<pairing_graph>::adjacency_iterator ai, ai_end ;

  out << "/*" << endl ;
  out << "-- vertex list --" << endl ;

  for(tie(vi, vi_end) = vertices(g); vi != vi_end; vi++){
    out << g[*vi].symbol << " : " << g[*vi] << endl ;
  }

  out << "-- adjacency list --" << endl ;

  for(tie(vi, vi_end) = vertices(g); vi != vi_end; vi++){
    out << g[*vi].symbol << " --> " ;
    for(tie(ai, ai_end) = adjacent_vertices(*vi, g); ai!=ai_end; ai++){
      out << g[*ai].symbol << "," ;
    }
    out << endl ;
  }

  out << "-- pairing graph properties --" << endl ;
  out_keyval(g,num_nodes)         << endl ;
  out_keyval(g,num_pairings)      << endl ;
  out_keyval(g,num_priorities)    << endl ;
  out << "-- boost graph properties --" << endl ;
  out << "num_vertices=" << num_vertices(g) << endl ;
  out << "num_edges=" << num_edges(g) << endl ;
  out << "*/" << endl ;
}

ostream & operator<<(ostream& out, const pairing_graph & g){
  /**/ if(g.format == raw  ) output_raw_dot  (out,g) ;
  else if(g.format == tango) output_tango_dot(out,g) ;
  else if(g.format == tex  ) output_tex_dot  (out,g) ;
  /**/ if(g.format != tango) output_detail   (out,g) ;
  return out ;
}

#undef out_keyval
#undef out_bool

}
