#ifndef _TATO_LOG_H
#define _TATO_LOG_H

#include <syslog.h>
#include <stdarg.h>
#include <stdbool.h>

void tato_log_open();
void tato_log_msg_printf(const int level, const char *fmt, ...);
void tato_log_msg(const int level, char const *msg);
void tato_log_close();

#endif
