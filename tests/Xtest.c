/**
	XPression Engine C (c)2025
	
	@author: Joshua (Codeshark37)
	@contact: labprogramming7@gmail.com
	
	Xtest.c
*/

#include "Xtest.h"

volatile bool g_timer_active = false;

 TestConfig g_test_config = {0};
 TestStats g_test_stats   = {0};
 TestState g_test_state   = {0};
 jmp_buf g_timeout_jmp;


#ifdef _WIN32
 HANDLE g_timeout_timer = NULL;
 volatile bool g_timeout_triggered = 0;
#else
 volatile sig_atomic_t g_timeout_triggered = 0;
#endif
