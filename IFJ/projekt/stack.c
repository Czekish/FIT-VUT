/**
 * Implementace prekladace jazyka IFJ18
 *
 * xstepa60     Daniel Stepan
 * xcalad01     Filip Caladi
 * xtimko02     Jakub Timko
 * xfasan00     Erika Fasankova
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "stack.h"


// function to create a stack of given capacity. It initializes size of
// stack as 0
struct Stack* createStack(unsigned capacity)
{
	struct Stack* stack = (struct Stack*) malloc(sizeof(struct Stack));
	stack->capacity = capacity;
	stack->top = -1;
	stack->array = (struct Stack_elem *) malloc(stack->capacity * sizeof(struct Stack_elem));
	return stack;
}

// Stack is full when top is equal to the last index
int isFull(struct Stack* stack)
{   return stack->top == stack->capacity - 1; }

// Stack is empty when top is equal to -1
int isEmpty(struct Stack* stack)
{   return stack->top == -1;  }

// Function to add an item to stack.  It increases top by 1
void push(struct Stack* stack, struct Stack_elem item)
{
	if (isFull(stack))
		return;
	stack->top++;
	stack->array[stack->top] = item;
	//printf("%c pushed to stack\n", item.type);
}

// Function to remove an item from stack.  It decreases top by 1
struct Stack_elem* pop(struct Stack* stack)
{
	if (isEmpty(stack))
		return NULL;

	return &stack->array[stack->top--];
}

struct Stack_elem* top(struct Stack* stack)
{
	if(isEmpty(stack))
		return NULL;
	return &stack->array[stack->top];
}

void disposeStack(struct Stack* stack) {
	while(!isEmpty(stack)) {
		pop(stack);
	}

	free(stack->array);
	free(stack);
}
