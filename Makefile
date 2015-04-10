#
# Centaurean SpookyHash
#
# Copyright (c) 2015, Guillaume Voirin
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of Centaurean nor the
#       names of its contributors may be used to endorse or promote products
#       derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# 6/02/15 14:38
#

SPOOKYHASH = spookyhash
CFLAGS = -Ofast -fomit-frame-pointer -std=c99

SRC_DIRECTORY = ./src/

# Search for Clang
CLANG_COMPILER := $(@shell clang -v)
FLTO = -flto
ifeq ($(CLANG_COMPILER),"")
	COMPILER = $(CC)
else
	COMPILER = clang
	ifeq ($(OS),Windows_NT)
		FLTO = 
	endif
endif

# OS specifics
ifeq ($(OS),Windows_NT)
    bold =
    normal =
    DYN_EXT = .dll
    STAT_EXT = .lib
    ARROW = ^-^>
else
    bold = `tput bold`
    normal = `tput sgr0`
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        DYN_EXT = .so
    endif
    ifeq ($(UNAME_S),Darwin)
        DYN_EXT = .dylib
    endif
    STAT_EXT = .a
    ARROW = \-\>
endif

SPOOKYHASH_SRC = $(wildcard $(SRC_DIRECTORY)*.c)
SPOOKYHASH_OBJ = $(SPOOKYHASH_SRC:.c=.o)
ALL_OBJ = $(SPOOKYHASH_OBJ)

.PHONY: compile-header compile-library-header link-header

all: $(SPOOKYHASH)$(DYN_EXT) $(SPOOKYHASH)$(STAT_EXT)

error:
	@echo ${bold}Error${normal}
	@echo Please install one of the supported compilers (Clang, GCC).
	exit 1

%.o: %.c
	@echo $^ $(ARROW) $@
	@$(COMPILER) -c $(CFLAGS) $(FLTO) $< -o $@

compile-header:
	@echo ${bold}Compiling SpookyHash${normal} using $(COMPILER) ...

compile-library-header:
	@$(eval CFLAGS = $(CFLAGS) -fPIC)
	@echo ${bold}Compiling SpookyHash as a library${normal} using $(COMPILER) ...

compile: compile-header $(SPOOKYHASH_OBJ)
	@echo Done.
	@echo

compile-library: compile-library-header $(SPOOKYHASH_OBJ)
	@echo Done.
	@echo

link-header: compile-library
	@echo ${bold}Linking SpookyHash${normal} ...

$(SPOOKYHASH)$(DYN_EXT): link-header $(ALL_OBJ)
	@echo *.o $(ARROW) ${bold}$(SPOOKYHASH)$(DYN_EXT)${normal}
	@$(COMPILER) -shared $(FLTO) -o $(SPOOKYHASH)$(DYN_EXT) $(ALL_OBJ)
	@$(RM) $(DENSITY).exp
	@echo Done.
	@echo

$(SPOOKYHASH)$(STAT_EXT): link-header $(ALL_OBJ)
	@echo *.o $(ARROW) ${bold}$(SPOOKYHASH)$(STAT_EXT)${normal}
	@ar rcs $(SPOOKYHASH)$(STAT_EXT) $(ALL_OBJ)
	@echo Done.
	@echo

clean:
	@echo ${bold}Cleaning SpookyHash objects${normal} ...
	@$(RM) $(SPOOKYHASH_OBJ)
	@$(RM) $(SPOOKYHASH)$(DYN_EXT)
	@$(RM) $(SPOOKYHASH)$(STAT_EXT)
	@echo Done.
	@echo