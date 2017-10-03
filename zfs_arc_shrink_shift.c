#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <inttypes.h>

/*
 * Name   : zfs_arc_shrink_shift.c
 * Author : Jason Banham
 * Date   : 2012/10/10 - 2017/10/03
 * Version: 1.03
 * Usage  : zfs_arc_shrink_shift <int memory_size>
 * Purpose: Given an amount of memory, calculate the arc_shrink_shift
 *	    value to keep this between 200MB and 256MB of RAM
 *	    (figure obtained thanks to work performed by Kirill)
 *
 *	    Algorithm for calculating zfs_arc_shrink_shift is:
 *		round(log2((RAM_size * 4) + 1))
 *
 *	    Thus a 192GB system would have round(log2((192 * 4) + 1))
 *	        round(log2(769))
 *	        round(9.586)
 *	        = 10
 *
 *          The amount of memory we free is arc_c >> arc_shrink_shift
 *          If memory is 192GB then the arc_c is around 191GB which
 *  	    means 200278016Kb so shifting that by 10 = 195584KB or 191MB
 *
 * History: 1.00 - Initial version
 *          1.01 - Needed to include stdlib.h to get strtod() to return a non-zero value
 *	    1.02 - Now accepts ramsize as an argument into the code
 *          1.03 - More platform agnostic (tested on MacOS, Linux and Solaris)
 */

#ifdef SOLARIS
#include <sys/int_fmtio.h>
#endif

#ifdef MACOS
#define uint64_t u_int64_t
#endif

#ifdef LINUX
#define uint64_t u_int64_t
#endif

#define MAX(a,b) ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a > _b ? _a : _b; })
#define MIN(a,b) ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a < _b ? _a : _b; })
#define GIG_TO_BYTES 1073741824;
#define MEGABYTES 1048576;

int
main(int argc, char *argv[])
{
    char *ramsize_input;
    char *shift_input;
    char *endptr;
    double memsize;
    double shiftsize = 5;
    uint64_t tofree_min, tofree_max;
    uint64_t arc_c;
    uint64_t zfs_arc_shrink_shift_min, zfs_arc_shrink_shift_max;

    if (argc < 2) {
	printf("Invalid number of arguments\nPlease run %s ram_size [shift_value]\n", argv[0]);
        printf("If shift_value is missing, this defaults to 5\n");
	exit(0);
    }

    ramsize_input = argv[1];
    memsize = strtod(ramsize_input, &endptr); 
    if (*endptr == 0) {
	if (ramsize_input == endptr) {
	    printf("Could not perform the ramsize conversion.\n");
	    return(1);
	}
    }
    printf("System has %.0f GB memory\n", memsize);
    printf("shiftsize = %.0f\n", shiftsize);

    if (argc > 2) {
	shift_input = argv[2];
	shiftsize = strtod(shift_input, &endptr);
	if (*endptr == 0) {
    	    if (shift_input == endptr) {
		printf("Could not convert the shift size input.\n");	
		return(1);
	    }
	}
	printf("shiftsize = %.0f\n", shiftsize);
    }

    zfs_arc_shrink_shift_min = (int)MIN(shiftsize,round(log2(memsize*4+1)));
    zfs_arc_shrink_shift_max = (int)MAX(shiftsize,round(log2(memsize*4+1)));

    arc_c = (memsize - 1) * GIG_TO_BYTES;
    tofree_min = arc_c >> zfs_arc_shrink_shift_min;
    tofree_max = arc_c >> zfs_arc_shrink_shift_max;
    tofree_min = tofree_min / MEGABYTES;
    tofree_max = tofree_max / MEGABYTES;

    printf("zfs_arc_shrink_shift (min) value = %" PRId64 "\n", zfs_arc_shrink_shift_min);
    printf("tofree (min)= %" PRId64 " MB\n", tofree_min);
    printf("zfs_arc_shrink_shift (max) value = %" PRId64 "\n", zfs_arc_shrink_shift_max);
    printf("tofree (max)= %" PRId64 " MB\n", tofree_max);

    return(0);
}
