#include "gd32f30x.h"

static enum 
{
	SM_WAIT = 0,
	SM_FIRST, 
	SM_NULL,
	SM_LAST,
	SM_RESET,
} state;

static enum 
{
	PULSE_STATE_DEFAULT     = 0x00,
	PULSE_STATE_FIRST_DIR 	= 0x02,
	PULSE_STATE_FIRST_REV 	= 0x01, 
	PULSE_STATE_SECOND    	= 0x03,
	PULSE_STATE_LAST_DIR	= 0x01,
	PULSE_STATE_LAST_REV	= 0x02,
} enc_state;

