/**
 * Implementace prekladace jazyka IFJ18
 *
 * xstepa60     Daniel Stepan
 * xcalad01     Filip Caladi
 * xtimko02     Jakub Timko
 * xfasan00     Erika Fasankova
 *
 */

#ifndef IFJ_STACK_H
#define IFJ_STACK_H

// A structure to represent a stack
struct Stack
{
	int top;
	unsigned capacity;
	struct Stack_elem* array;
};

struct Stack_elem
{
	char type; // bude nabyvat hodnot 'i'/'E'
	int const_or_var;
	char *const_val;
	char *var_name;
};

struct Stack* createStack(unsigned capacity);
int isFull(struct Stack* stack);
int isEmpty(struct Stack* stack);
void push(struct Stack* stack, struct Stack_elem item);
struct Stack_elem* pop(struct Stack* stack);
struct Stack_elem* top(struct Stack* stack);
void disposeStack(struct Stack* stack);

#endif
