/*
	Windows GUI Calculator
	Version 1.2

	Author: Rafael Sabe
	Email: rafaelmsabe@gmail.com
*/

#ifndef MATH_HANDLER_H
#define MATH_HANDLER_H

#include "globldef.h"

#define MATHOP_NULL 0
#define MATHOP_SUM 1
#define MATHOP_SUB 2
#define MATHOP_MULT 3
#define MATHOP_DIV 4
#define MATHOP_POW 5
#define MATHOP_ROOT 6
#define MATHOP_LOG 7
#define MATHOP_SIN 8
#define MATHOP_COS 9
#define MATHOP_TAN 10
#define MATHOP_FAC 11
#define MATHOP_BIN 12

extern DOUBLE first_input;
extern DOUBLE final_input;
extern INT math_op;
extern BOOL ang_unit;

extern BOOL WINAPI calculate(VOID);
extern BOOL WINAPI math_op_uses_second_input(INT op);
extern BOOL WINAPI math_op_uses_ang_unit(INT op);

#endif //MATH_HANDLER_H
