//--------------------------------------------------------------------------------------------
//! @author korr237i
//! @file	tools.h
//! This file contains special tools like macros, structures etc.
//--------------------------------------------------------------------------------------------

#ifndef TOOLS_H_
#define TOOLS_H_

#include <stdlib.h>
#include <stdio.h>

//--------------------------------------------------------------------------------------------
//! @def green
//! Sets green color.
//--------------------------------------------------------------------------------------------
#define green		puts("\033[1;32m")

//--------------------------------------------------------------------------------------------
//! @def blue
//! Sets blue color.
//--------------------------------------------------------------------------------------------
#define blue		puts("\033[1;34m")

//--------------------------------------------------------------------------------------------
//! @def yellow
//! Sets yellow color.
//--------------------------------------------------------------------------------------------
#define yellow		puts("\033[01;33m")

//--------------------------------------------------------------------------------------------
//! @def red
//! Sets red color.
//--------------------------------------------------------------------------------------------
#define red			puts("\033[1;31m")

//--------------------------------------------------------------------------------------------
//! @def reset_color
//! Resets color.
//--------------------------------------------------------------------------------------------
#define reset_color puts("\033[0m")


//--------------------------------------------------------------------------------------------
//! @def message( color, phrase )
//! Prints phrase in color.
//!
//! @param[in]	color	Color used in print from defined macros, e.g. red, yellow, etc.
//! @param[in]	phrase	Phrase to be printed.
//--------------------------------------------------------------------------------------------
#define message( color, phrase )	{color; printf(phrase"\n"); reset_color;}


//--------------------------------------------------------------------------------------------
//! @def $( x , type )
//! Printfs value x of type type, e.g. $(10, "d"), $("sdasd", "s").
//!
//! @param[in]	x		Variable to be printed.
//! @param[in]	type	Type of x in char* type: "s", "d", "f", etc.
//--------------------------------------------------------------------------------------------
#define $( x , type )	printf("%"type, (x));

//--------------------------------------------------------------------------------------------
//! @def $n ( x, type )
//! $ with \n.
//!
//! @param[in]	x		Variable to be printed.
//! @param[in]	type	Type of x in char* type: "s", "d", "f", etc.
//--------------------------------------------------------------------------------------------
#define $n( x , type )	printf("%"type"\n", (x));


//--------------------------------------------------------------------------------------------
//! @def $$( x, type, special )
//! $ with "x = ", e.g $$(x, "d", "# ") -> "# x = 10".
//!
//! @param[in]	x		Variable to be printed.
//! @param[in]	type	Type of x in char* type: "s", "d", "f", etc.
//! @param[in]	special	String added to the beginning of print.
//--------------------------------------------------------------------------------------------
#define $$( x , type , special )	printf(special#x" = %"type, (x));

//--------------------------------------------------------------------------------------------
//! @def $$n( x, type, special )
//! $$ with \n.
//!
//! @param[in]	x		Variable to be printed.
//! @param[in]	type	Type of x in char* type: "s", "d", "f", etc.
//! @param[in]	special	String to be added to the beginning of print.
//--------------------------------------------------------------------------------------------
#define $$n( x , type , special )	printf(special#x" = %"type"\n", (x));


//--------------------------------------------------------------------------------------------
//! @def MY_assert( check )
//! Custom soft assert (return value, no abort).
//! Prints function name, file name and line number where it occures.
//!
//! @param[in]	check	Expression to be compared to zero.
//!
//! @retval		0 if (check != 0), -1 else.
//--------------------------------------------------------------------------------------------
#define MY_assert(check)																							\
({																													\
	int error = 0;																									\
	if (!(check))																									\
	{																												\
		red;																										\
		printf("!## ASSERTION FAILED: function %s()\tFILE %s\tLINE %d\n", __PRETTY_FUNCTION__, __FILE__, __LINE__);	\
		printf("!## expression: %s\n", #check);																		\
		reset_color;																								\
		error = -1;																									\
	}																												\
	error;																											\
})

//--------------------------------------------------------------------------------------------
//! @enum errors_e
//! Contains custom errors
//--------------------------------------------------------------------------------------------
enum errors_e
{
	OK			=  0,
	ASSERT_FAIL = -1,
	INPUT_FAIL 	= -2,
	TESTS_FAIL  = -3,
	ALLOC_FAIL	= -4,
	OPEN_FAIL	= -5,
	FUN_ERROR	= -10
};

int runUnitTests(int tests_num, int (*tester)(int));

#endif /* TOOLS_H_ */
