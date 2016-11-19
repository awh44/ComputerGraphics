#ifndef _STATUS_H_
#define _STATUS_H_

typedef enum
{
	SUCCESS = 0,
	ARGS_ERROR,
	FILE_OPEN_ERROR,
	FILE_READ_ERROR,
	FILE_FORMAT_ERROR,
	OUT_OF_MEM,
} status_t;

#define IF_ERROR_GOTO(expr, errvar, label)\
	do\
	{\
		if ((errvar = (expr)))\
		{\
			goto label;\
		}\
	} while (0)

#endif
