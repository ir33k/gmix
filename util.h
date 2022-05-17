#ifndef UTIL_H_
#define UTIL_H_

/* Print message with given FMT format like in printf and exit program
 * with error code 1.  If FMT string ends with ':' then after message
 * print last system or stdlib error.  Implementation is shamelessly
 * stollen from Suckless "dwm" program source code. */
void die(char *fmt, ...);

#endif	/* UTIL_H_ */
