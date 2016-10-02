#ifndef _STATUS_H_
#define _STATUS_H_

typedef enum
{
	SUCCESS = 0,
	ARGS_ERROR,
	FILE_OPEN_ERROR,
	FILE_READ_ERROR,
	FILE_FORMAT_ERROR,
} status_t;

#endif
