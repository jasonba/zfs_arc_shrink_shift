#-------------------------------------------------------------
# UNIX Makefile for the zfs_arc_shrink_shift program
# Tested against a MacOS X 10.8 system (BSD)
# but should work on most Unices
#-------------------------------------------------------------

# What's our C compiler - expecting the standard, ie: not gcc
CC=cc

# Compiler flags, we want debugging initially
# may be reset to nothing for production
CFLAGS=

all: zfs_arc_shrink_shift

# How to make the debug binary
debug: CC += -DDEBUG -g
debug: zfs_arc_shrink_shift

# Here's how we build the program
zfs_arc_shrink_shift: zfs_arc_shrink_shift.o
	$(CC) $(CFLAGS) -o zfs_arc_shrink_shift zfs_arc_shrink_shift.c

clean:
	rm zfs_arc_shrink_shift
