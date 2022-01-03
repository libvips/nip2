/* Graph actions.
 */

/* A strict action on the graph.
 */
typedef void (*ActionFn)( Reduce *, Compile *,
	int, const char *, HeapNode **, PElement *, void * );

/* A sort of reducer (eg. lazy or strict, or hyperstrict)
 */
typedef void (*ReduceFunction)( Reduce *, PElement * );

#define OPERATOR_NAME( OP ) ( \
	(int) (OP) >= 0 && (int) (OP) < noperator_table ? \
		operator_table[(int) (OP)] : "<unknown>" \
)

extern const char *operator_table[];
extern const int noperator_table;

void action_proc_uop( Reduce *rc, Compile *compile,
	int op, const char *name, HeapNode **arg, PElement *out );
void action_proc_construct( Reduce *rc, Compile *compile, 
	HeapNode **arg, PElement *out );

void action_proc_bop( Reduce *rc, Compile *compile, 
	BinOp bop, HeapNode **arg );

void action_dispatch( Reduce *rc, Compile *compile, ReduceFunction rfn, 
	int op, const char *name, gboolean override,
	ActionFn afn, int nargs, HeapNode **arg, void *user );
