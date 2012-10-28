/*	$OpenBSD: calendar.c,v 1.18 2002/06/23 03:07:21 deraadt Exp $	*/
/*	$Id: calendar.c,v 1.2 2002/11/27 00:00:04 drey Exp $	*/

/*
 * Copyright (c) 1989, 1993, 1994
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <err.h>
#include <errno.h>
#include <locale.h>
#include <pwd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "pathnames.h"
#include "calendar.h"

char *calendarFile = "calendar";  /* default calendar file */
char *calendarHome = ".calendar"; /* HOME */
char *calendarNoMail = "nomail";  /* don't sent mail if this file exists */

struct passwd *pw;
time_t f_time = 0;
int bodun_always = 0;

int f_dayAfter = 0; /* days after current date */
int f_dayBefore = 0; /* days before current date */

struct specialev spev[NUMEV];

void childsig(int);

int
main(argc, argv)
	int argc;
	char *argv[];
{
	int ch;
	char *caldir;

	(void)setlocale(LC_ALL, "");

	while ((ch = getopt(argc, argv, "-abf:t:A:B:")) != -1)
		switch (ch) {
		case '-':		/* backward contemptible */
		case 'b':
			bodun_always++;
			break;

		case 'f': /* other calendar file */
		        calendarFile = optarg;
			break;

		case 't': /* other date, undocumented, for tests */
			if ((f_time = Mktime(optarg)) <= 0)
				errx(1, "specified date is outside allowed range");
			break;

		case 'A': /* days after current date */
			f_dayAfter = atoi(optarg);
			break;

		case 'B': /* days before current date */
			f_dayBefore = atoi(optarg);
			break;

		default:
			usage();
		}
	argc -= optind;
	argv += optind;

	if (argc)
		usage();

	/* use current time */
	if (f_time <= 0)
	    (void)time(&f_time);

	if (f_dayBefore) {
		/* Move back in time and only look forwards */
		f_dayAfter += f_dayBefore;
		f_time -= SECSPERDAY * f_dayBefore;
		f_dayBefore = 0;
	}
	settime(&f_time);

	if ((caldir = getenv("CALENDAR_DIR")) != NULL) {
		if(!chdir(caldir))
			cal();
	} else
		cal();

	exit(0);
}


void
usage()
{
	(void)fprintf(stderr,
	    "usage: calendar [-a] [-A num] [-b] [-B num] [-t [[[cc]yy][mm]]dd] "
	    "[-f calendarfile]\n");
	exit(1);
}


void
childsig(sig)
	int sig;
{
}
