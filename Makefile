THIS := $(lastword $(MAKEFILE_LIST))
MAKEFLAGS += --output-sync=target

#--------DIRECTORIES--------#
BUILDDIR = build/
DEPDIR = $(BUILDDIR)dep/
PREDIR = $(BUILDDIR)pre/
ASMDIR = $(BUILDDIR)asm/
OBJDIR = $(BUILDDIR)obj/
BINDIR = $(BUILDDIR)bin/
MISCDIR = $(BUILDDIR)misc/
SRCDIR = src/
MAKEINCDIR = make/
GDBDIR = gdb/
GDBDEFAULT = $(GDBDIR)default.gdb

CLEANDIR = $(DEPDIR) $(PREDIR) $(ASMDIR) $(OBJDIR) $(MISCDIR)

#--------AUTOMATIC RECURSIVE FILE FINDING--------#
SOURCES_C = $(shell find $(SRCDIR) -name "*.c")
SOURCES_ASM = $(shell find $(SRCDIR) -name "*.s")
THIS += $(wildcard $(MAKEINCDIR)*.mk)

#--------INTERMIDIATE FILES--------#
PRE = $(addprefix $(PREDIR), $(notdir $(SOURCES_C:%.c=%.i)))
ASM = $(addprefix $(ASMDIR), $(notdir $(SOURCES_C:%.c=%.s)))
OBJC = $(addprefix $(OBJDIR), $(notdir $(ASM:%.s=%.c.o)))
OBJASM += $(addprefix $(OBJDIR), $(notdir $(SOURCES_ASM:%.s=%.s.o)))
OBJ = $(OBJC) $(OBJASM)
DEP = $(addprefix $(DEPDIR), $(notdir $(SOURCES_C:%.c=%.d)))

#--------FINAL FILES--------#
TARGETNAME = kernel
KERNELIMG = $(BINDIR)$(TARGETNAME).img
KERNELELF = $(BINDIR)$(TARGETNAME).elf
KERNELLIST = $(MISCDIR)$(TARGETNAME).list
LINKERSCRIPT = $(MAKEINCDIR)$(TARGETNAME).ld
MAPFILE = $(MISCDIR)$(TARGETNAME).map

-include $(MAKEINCDIR)config.inc.mk

#--------COMMANDS--------#
HIDE ?= @
PREFIX ?= arm-none-eabi-
CMD_PREFIX ?= $(HIDE)$(PREFIX)
GDB ?= $(PREFIX)gdb
QEMU ?= qemu-system-arm
MKDIR = $(HIDE)mkdir -p
ECHO = $(HIDE)echo
RM = $(HIDE)rm -rf
PRINTF = $(HIDE)printf

#--------COMP, ASM & EXE OPTIONS--------#
CC_FLAGS = -Wall -Wextra -Werror -nostdlib -fomit-frame-pointer -mno-apcs-frame -nostartfiles -ffreestanding -g -march=armv6z -marm
ASM_FLAGS = -g -march=armv6z
QEMU_FLAGS = -kernel $(KERNELELF) -cpu arm1176 -m 512 -M raspi2 -nographic -no-reboot -S -s

include $(MAKEINCDIR)colors.inc.mk
include $(MAKEINCDIR)errorHandler.inc.mk

#--------SPECIAL RULES--------#
.PHONY: all clean mrproper emu run list deploy sdcopy umount default
.PRECIOUS: $(PRE) $(ASM) $(OBJ) $(DEP)
.SECONDEXPANSION:

#--------RULES--------#
include $(MAKEINCDIR)rules.inc.mk

#--------PHONY RULES--------#
include $(MAKEINCDIR)rules.phony.inc.mk

#--------AUTOMATIC DEPENDENCIES--------#
-include $(DEP)
