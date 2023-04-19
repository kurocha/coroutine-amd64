/*
 *  This file is part of the "Coroutine" project and released under the MIT License.
 *
 *  Created by Samuel Williams on 10/5/2018.
 *  Copyright, 2018, by Samuel Williams. All rights reserved.
*/

#pragma once

#include <assert.h>
#include <string.h>
#include <stdint.h>

#if __cplusplus
extern "C" {
#endif

#define COROUTINE __attribute__((noreturn)) void

const size_t COROUTINE_REGISTERS = 6;

typedef struct
{
	void **stack_pointer;
	void *argument;
} CoroutineContext;

typedef COROUTINE(* CoroutineStart)(CoroutineContext *from, CoroutineContext *self);

void coroutine_trampoline();

static inline void coroutine_initialize(
	CoroutineContext *context,
	CoroutineStart start,
	void *argument,
	void *stack_pointer,
	size_t stack_size
) {
	/* 	The i386 System V ABI has guaranteed/required for years that ESP+4 is 16B-aligned on entry to a function. (i.e. ESP must be 16B-aligned before a CALL instruction, so args on the stack start at a 16B boundary. This is the same as for x86-64 System V.) */
	context->stack_pointer = (void**)((uintptr_t)stack_pointer & ~0xF);
	
	context->argument = argument;

	if (!start) {
		assert(!context->stack_pointer);
		/* We are main coroutine for this thread */
		return;
	}

	*--context->stack_pointer = NULL;
	*--context->stack_pointer = (void*)start;
	
	context->stack_pointer -= COROUTINE_REGISTERS;
	memset(context->stack_pointer, 0, sizeof(void*) * COROUTINE_REGISTERS);
}

CoroutineContext * coroutine_transfer(CoroutineContext * current, CoroutineContext * target);

#if __cplusplus
}
#endif
