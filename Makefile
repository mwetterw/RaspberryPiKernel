THIS := $(lastword $(MAKEFILE_LIST))

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
CLEANDIR = $(DEPDIR) $(PREDIR) $(ASMDIR) $(OBJDIR) $(MISCDIR)

#--------AUTOMATIC RECURSIVE FILE FINDING--------#
SOURCES_C = $(shell find $(SRCDIR) -name "*.c")
SOURCES_ASM = $(shell find $(SRCDIR) -name "*.s")
THIS += $(wildcard $(MAKEINCDIR)*.mk)

#--------INTERMIDIATE FILES--------#
PRE = $(addprefix $(PREDIR), $(notdir $(SOURCES_C:%.c=%.i)))
ASM = $(addprefix $(ASMDIR), $(notdir $(SOURCES_C:%.c=%.s)))
OBJC = $(addprefix $(OBJDIR), $(notdir $(ASM:%.s=%.o)))
OBJASM += $(addprefix $(OBJDIR), $(notdir $(SOURCES_ASM:%.s=%.o)))
OBJ = $(OBJC) $(OBJASM)
DEP = $(addprefix $(DEPDIR), $(notdir $(SOURCES_C:%.c=%.d)))

#--------FINAL FILES--------#
TARGETNAME = kernel
KERNELIMG = $(BINDIR)$(TARGETNAME).img
KERNELELF = $(BINDIR)$(TARGETNAME).elf
KERNELLIST = $(MISCDIR)$(TARGETNAME).list
LINKERSCRIPT = $(TARGETNAME).ld
MAPFILE = $(MISCDIR)$(TARGETNAME).map

#--------COMMANDS--------#
HIDE ?= @
CMD_PREFIX ?= $(HIDE)arm-linux-gnueabi-
GDB ?= gdb-multiarch
QEMU ?= qemu-system-arm
MKDIR = $(HIDE)mkdir -p
ECHO = $(HIDE)echo
RM = $(HIDE)rm -rf
PRINTF = $(HIDE)printf

#--------COMP, ASM & EXE OPTIONS--------#
CC_FLAGS = -Wall -Wextra -Werror -nostdlib -fomit-frame-pointer -mno-apcs-frame -nostartfiles -ffreestanding -g -march=armv6z -marm
ASM_FLAGS = -g -march=armv6z
QEMU_FLAGS = -kernel $(KERNELELF) -cpu arm1176 -m 512 -M versatilepb -nographic -no-reboot -serial stdio -append "rw earlyprintk loglevel=8 panic=120 keep_bootcon rootwait dma.dmachans=0x7f35 bcm2708_fb.fbwidth=1024 bcm2708_fb.fbheight=768 bcm2708.boardrev=0xf bcm2708.serial=0xcad0eedf smsc95xx.macaddr=B8:27:EB:D0:EE:DF sdhci-bcm2708.emmc_clock_freq=100000000 vc_mem.mem_base=0x1c000000 vc_mem.mem_size=0x20000000  dwc_otg.lpm_enable=0 kgdboc=ttyAMA0,115200 console=ttyS0 root=/dev/mmcblk0p2 rootfstype=ext4 elevator=deadline rootwait" -S -s

include make/colors.inc.mk
include make/errorHandler.inc.mk

#--------SPECIAL RULES--------#
.PHONY: all clean mrproper emu run
.PRECIOUS: $(PRE) $(ASM) $(OBJ) $(DEP)
.SECONDEXPANSION:

#--------RULES--------#
include make/rules.inc.mk

#--------PHONY RULES--------#
include make/rules.phony.inc.mk

#--------AUTOMATIC DEPENDENCIES--------#
-include $(DEP)
