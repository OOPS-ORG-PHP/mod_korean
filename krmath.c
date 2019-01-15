#include "php.h"
#include "php_krmath.h"
#include "php_krparse.h"

#include <math.h>
#include <float.h>

char * kr_math_number_format (double d, int dec, char dec_point, char thousand_sep)
{
	char * tmpbuf, * resbuf;
	char * s, * t;  /* source, target */
	int tmplen, reslen = 0;
	int count = 0;
	int is_negative = 0;

	if ( d < 0 ) {
		is_negative = 1;
		d = -d;
	} 
	dec = MAX (0, dec);
	tmpbuf = (char *) emalloc (1 + DBL_MAX_10_EXP + 1 + dec + 1);

	tmplen = sprintf (tmpbuf, "%.*f", dec, d);

	if ( ! isdigit ((int) tmpbuf[0]) )
		return tmpbuf;

	if ( dec ) {
		reslen = dec + 1 + (tmplen-dec - 1) + ((thousand_sep) ? (tmplen - 1 - dec - 1) / 3 : 0);
	} else {
		reslen = tmplen + ((thousand_sep) ? (tmplen - 1) / 3 : 0);
	}
	if ( is_negative )
		reslen++;

	resbuf = (char *) emalloc (reslen + 1);

	s = tmpbuf + tmplen - 1;
	t = resbuf + reslen;
	*t-- = 0;
									 
	if ( dec ) {
		while ( isdigit ((int) *s) )
			*t-- = *s--;

		*t-- = dec_point;  /* copy that dot */
		s--;
	}
									        
	while ( s >= tmpbuf ) {
		*t-- = *s--;
		if ( thousand_sep && (++count%3) == 0 && s >= tmpbuf ) {
			*t-- = thousand_sep;
		}
	}
	if ( is_negative )
		*t-- = '-';

	kr_safe_efree (tmpbuf);
	return resbuf;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
