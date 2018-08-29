#ifndef SYMBOL_STACK
#define SYMBOL_STACK
#ifdef __cplusplus
extern "C" {
#endif

int  symbol_stack_pointer();
void push_symbol(char * symbol, int symbol_type, char * type_name);
void restore_symbol_stack(int n);
int  symbol_to_symbol_type(char * symbol);
const char * symbol_to_type_name(char * symbol);

#ifdef __cplusplus
}
#endif
#endif /* SYMBOL_STACK */
