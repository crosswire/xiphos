#ifndef __DEBUG_H_
#define __DEBUG_H_

#include <glib.h>

/*
 * Implementation debug macros
 *      - logging debug message
 */

#ifdef DEBUG

#define DEBUG_ERR g_error
#undef G_DISABLE_ASSERT
#define DEBUG_MSG g_print
#define CHECK_MEMORY(expression, message) if (expression) { \
   g_error ("%s\n", message); \
}

#else

#define DEBUG_ERR g_warning
#define G_DISABLE_ASSERT
#define CHECK_MEMORY(expression, message)

/* If GNU C compiler, then we can fully remove all debug info */
#ifdef __GNUC__
#define DEBUG_MSG(format, args...)
 /**/
#else							/* !__GNUC__ */
#define DEBUG_MSG g_none
/*
 * Function: g_none
 * Arguments:
 *      format  - string
 *      ...     - parametrs
 * Return value:
 *      none
 * Description:
 *      none
 */
static void g_none(gchar * format, ...)
{
	return;
};

#endif

#endif


#endif							/* __DEBUG_H_ */
