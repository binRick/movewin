# ========================================================================
# Makefile - build lswin and movewin programs
# Andrew Ho (andrew@zeuscat.com)
#
# Copyright (c) 2014-2020, Andrew Ho.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# Redistributions of source code must retain the above copyright notice,
# this list of conditions and the following disclaimer.
#
# Redistributions in binary form must reproduce the above copyright
# notice, this list of conditions and the following disclaimer in the
# documentation and/or other materials provided with the distribution.
#
# Neither the name of the author nor the names of its contributors may
# be used to endorse or promote products derived from this software
# without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# ========================================================================

CC = gcc
CC_FLAGS = -Wall
LD = gcc
LD_FLAGS = -Wall -framework Carbon -lparson

DESTDIR =
PREFIX = $(DESTDIR)/usr/local
BINDIR = $(PREFIX)/bin
MKDIR = mkdir
CP = cp
RM = rm

TARGETS = lswin movewin
OBJECTS = lswin.o movewin.o winutils.o

all: $(TARGETS)

lswin: winutils.o lswin.o
	$(LD) $(LD_FLAGS) -o lswin winutils.o lswin.o

movewin: winutils.o movewin.o
	$(LD) $(LD_FLAGS) -o movewin winutils.o movewin.o

winutils.o: winutils.h winutils.c
	$(CC) $(CC_FLAGS) -c winutils.c

lswin.o: lswin.c
	$(CC) $(CC_FLAGS) -c lswin.c

movewin.o: movewin.c
	$(CC) $(CC_FLAGS) -c movewin.c

install: $(TARGETS)
	$(MKDIR) -p $(BINDIR)
	$(CP) $(TARGETS) $(BINDIR)

clean:
	@$(RM) -f $(TARGETS) $(OBJECTS) core
	@(cd examples && make clean)

examples: force
	(cd examples && make)

force:
	@true


# ========================================================================
