/*
	Windows GUI Calculator
	Version 1.2

	Author: Rafael Sabe
	Email: rafaelmsabe@gmail.com
*/

#include "math_handler.h"
#include "strdef.h"
#include <math.h>

#define MATH_PI 3.141592653589793

DOUBLE first_input = 0.0;
DOUBLE final_input = 0.0;
INT math_op = MATHOP_NULL;
BOOL ang_unit = FALSE;

extern VOID WINAPI math_sum(VOID);
extern VOID WINAPI math_sub(VOID);
extern VOID WINAPI math_mult(VOID);
extern VOID WINAPI math_div(VOID);
extern VOID WINAPI math_pow(VOID);
extern VOID WINAPI math_root(VOID);
extern VOID WINAPI math_log(VOID);
extern VOID WINAPI math_radsin(VOID);
extern VOID WINAPI math_degsin(VOID);
extern VOID WINAPI math_radcos(VOID);
extern VOID WINAPI math_degcos(VOID);
extern VOID WINAPI math_radtan(VOID);
extern VOID WINAPI math_degtan(VOID);
extern VOID WINAPI math_fac(VOID);
extern VOID WINAPI math_bin(VOID);

BOOL WINAPI calculate(VOID)
{
	switch(math_op)
	{
		case MATHOP_SUM:
			math_sum();
			return TRUE;

		case MATHOP_SUB:
			math_sub();
			return TRUE;

		case MATHOP_MULT:
			math_mult();
			return TRUE;

		case MATHOP_DIV:
			math_div();
			return TRUE;

		case MATHOP_POW:
			math_pow();
			return TRUE;

		case MATHOP_ROOT:
			math_root();
			return TRUE;

		case MATHOP_LOG:
			math_log();
			return TRUE;

		case MATHOP_SIN:
			if(ang_unit) math_degsin();
			else math_radsin();
			return TRUE;

		case MATHOP_COS:
			if(ang_unit) math_degcos();
			else math_radcos();
			return TRUE;

		case MATHOP_TAN:
			if(ang_unit) math_degtan();
			else math_radtan();
			return TRUE;

		case MATHOP_FAC:
			math_fac();
			return TRUE;

		case MATHOP_BIN:
			math_bin();
			return TRUE;
	}

	return FALSE;
}

BOOL WINAPI math_op_uses_second_input(INT op)
{
	if(op == MATHOP_NULL) return FALSE;
	if(op == MATHOP_SIN) return FALSE;
	if(op == MATHOP_COS) return FALSE;
	if(op == MATHOP_TAN) return FALSE;
	if(op == MATHOP_FAC) return FALSE;

	return TRUE;
}

BOOL WINAPI math_op_uses_ang_unit(INT op)
{
	if(op == MATHOP_SIN) return TRUE;
	if(op == MATHOP_COS) return TRUE;
	if(op == MATHOP_TAN) return TRUE;

	return FALSE;
}

VOID WINAPI math_sum(VOID)
{
	DOUBLE result = 0.0;
	result = first_input + final_input;
	__SPRINTF(textbuf, TEXTBUF_SIZE_CHARS, TEXT("%lf"), result);
	return;
}

VOID WINAPI math_sub(VOID)
{
	DOUBLE result = 0.0;
	result = first_input - final_input;
	__SPRINTF(textbuf, TEXTBUF_SIZE_CHARS, TEXT("%lf"), result);
	return;
}

VOID WINAPI math_mult(VOID)
{
	DOUBLE result = 0.0;
	result = first_input*final_input;
	__SPRINTF(textbuf, TEXTBUF_SIZE_CHARS, TEXT("%lf"), result);
	return;
}

VOID WINAPI math_div(VOID)
{
	DOUBLE result = 0.0;

	if(final_input == 0.0) __SPRINTF(textbuf, TEXTBUF_SIZE_CHARS, TEXT("NO SOLUTION"));
	else
	{
		result = first_input/final_input;
		__SPRINTF(textbuf, TEXTBUF_SIZE_CHARS, TEXT("%lf"), result);
	}

	return;
}

VOID WINAPI math_pow(VOID)
{
	DOUBLE result = 0.0;
	result = pow(first_input, final_input);
	__SPRINTF(textbuf, TEXTBUF_SIZE_CHARS, TEXT("%lf"), result);
	return;
}

VOID WINAPI math_root(VOID)
{
	DOUBLE result = 0.0;

	if((first_input < 0.0) || (final_input == 0.0)) __SPRINTF(textbuf, TEXTBUF_SIZE_CHARS, TEXT("NO SOLUTION"));
	else
	{
		final_input = 1.0/final_input;
		result = pow(first_input, final_input);
		__SPRINTF(textbuf, TEXTBUF_SIZE_CHARS, TEXT("%lf"), result);
	}

	return;
}

VOID WINAPI math_log(VOID)
{
	DOUBLE result = 0.0;

	if((first_input <= 0.0) || (final_input <= 0.0) || (final_input == 1.0)) __SPRINTF(textbuf, TEXTBUF_SIZE_CHARS, TEXT("NO SOLUTION"));
	else
	{
		result = log10(first_input)/log10(final_input);
		__SPRINTF(textbuf, TEXTBUF_SIZE_CHARS, TEXT("%lf"), result);
	}

	return;
}

VOID WINAPI math_radsin(VOID)
{
	DOUBLE result = 0.0;
	result = sin(final_input);
	__SPRINTF(textbuf, TEXTBUF_SIZE_CHARS, TEXT("%lf"), result);
	return;
}

VOID WINAPI math_degsin(VOID)
{
	final_input *= (MATH_PI/180.0);
	math_radsin();
	return;
}

VOID WINAPI math_radcos(VOID)
{
	DOUBLE result = 0.0;
	result = cos(final_input);
	__SPRINTF(textbuf, TEXTBUF_SIZE_CHARS, TEXT("%lf"), result);
	return;
}

VOID WINAPI math_degcos(VOID)
{
	final_input *= (MATH_PI/180.0);
	math_radcos();
	return;
}

VOID WINAPI math_radtan(VOID)
{
	DOUBLE result = 0.0;

	if(cos(final_input) == 0.0) __SPRINTF(textbuf, TEXTBUF_SIZE_CHARS, TEXT("NO SOLUTION"));
	else
	{
		result = tan(final_input);
		__SPRINTF(textbuf, TEXTBUF_SIZE_CHARS, TEXT("%lf"), result);
	}

	return;
}

VOID WINAPI math_degtan(VOID)
{
	final_input *= (MATH_PI/180.0);
	math_radtan();
	return;
}

VOID WINAPI math_fac(VOID)
{
	ULONG_PTR a = 0u;
	ULONG_PTR tmp = 0u;

	if(final_input < 0.0)
	{
		__SPRINTF(textbuf, TEXTBUF_SIZE_CHARS, TEXT("NO SOLUTION"));
		return;
	}

	a = (ULONG_PTR) round(final_input);
	if(((DOUBLE) a) != final_input)
	{
		__SPRINTF(textbuf, TEXTBUF_SIZE_CHARS, TEXT("NO SOLUTION"));
		return;
	}

	if(a == 0u) a = 1u;
	else if(a > 2u) for(tmp = (a - 1u); tmp > 1u; tmp--) a *= tmp;

#ifdef _WIN64
	__SPRINTF(textbuf, TEXTBUF_SIZE_CHARS, TEXT("%llu"), a);
#else
	__SPRINTF(textbuf, TEXTBUF_SIZE_CHARS, TEXT("%lu"), a);
#endif

	return;
}

VOID WINAPI math_bin(VOID)
{
	ULONG_PTR a = 0u;
	ULONG_PTR b = 0u;
	ULONG_PTR c = 0u;
	ULONG_PTR tmp = 0u;

	if((first_input < 0.0) || (final_input < 0.0))
	{
		__SPRINTF(textbuf, TEXTBUF_SIZE_CHARS, TEXT("NO SOLUTION"));
		return;
	}

	a = (ULONG_PTR) round(first_input);
	b = (ULONG_PTR) round(final_input);

	if((((DOUBLE) a) != first_input) || (((DOUBLE) b) != final_input))
	{
		__SPRINTF(textbuf, TEXTBUF_SIZE_CHARS, TEXT("NO SOLUTION"));
		return;
	}

	if(a < b)
	{
		__SPRINTF(textbuf, TEXTBUF_SIZE_CHARS, TEXT("NO SOLUTION"));
		return;
	}

	c = a - b;

	if(a == 0u) a = 1u;
	else if(a > 2u) for(tmp = (a - 1u); tmp > 1u; tmp--) a *= tmp;

	if(b == 0u) b = 1u;
	else if(b > 2u) for(tmp = (b - 1u); tmp > 1u; tmp--) b *= tmp;

	if(c == 0u) c = 1u;
	else if(c > 2u) for(tmp = (c - 1u); tmp > 1u; tmp--) c *= tmp;

	tmp = a/(b*c);

#ifdef _WIN64
	__SPRINTF(textbuf, TEXTBUF_SIZE_CHARS, TEXT("%llu"), tmp);
#else
	__SPRINTF(textbuf, TEXTBUF_SIZE_CHARS, TEXT("%lu"), tmp);
#endif

	return;
}
