#ifndef usart_H
#define usart_H

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------//	
#include "gd32f30x.h"
#include <stdio.h>

//---------------------------------------------------------------------//
void usart0_config(void);
int DBG_put_msg (char *, int );

//---------------------------------------------------------------------//
extern char DBG_buf[30];

#ifdef __cplusplus
}
#endif

#endif
