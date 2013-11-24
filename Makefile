THIS := $(lastword $(MAKEFILE_LIST))

BUILDDIR = build/
DEPDIR = $(BUILDDIR)dep/
PREDIR = $(BUILDDIR)pre/
ASMDIR = $(BUILDDIR)asm/
OBJDIR = $(BUILDDIR)obj/
BINDIR = $(BUILDDIR)bin/
MISCDIR = $(BUILDDIR)misc/
SRCDIR = src/

CLEANDIR = $(DEPDIR) $(PREDIR) $(ASMDIR) $(OBJDIR) $(MISCDIR)

SOURCES_C = $(shell find $(SRCDIR) -name "*.c")
SOURCES_ASM = $(shell find $(SRCDIR) -name "*.s")

PRE = $(addprefix $(PREDIR), $(notdir $(SOURCES_C:%.c=%.i)))
ASM = $(addprefix $(ASMDIR), $(notdir $(SOURCES_C:%.c=%.s)))
OBJC = $(addprefix $(OBJDIR), $(notdir $(ASM:%.s=%.o)))
OBJASM += $(addprefix $(OBJDIR), $(notdir $(SOURCES_ASM:%.s=%.o)))
OBJ = $(OBJC) $(OBJASM)
DEP = $(addprefix $(DEPDIR), $(notdir $(SOURCES_C:%.c=%.d)))


TARGETNAME = kernel
KERNELIMG = $(BINDIR)$(TARGETNAME).img
KERNELELF = $(BINDIR)$(TARGETNAME).elf
KERNELLIST = $(MISCDIR)$(TARGETNAME).list

HIDE = @
CMD_PREFIX = $(HIDE)arm-linux-gnueabi-

CC_FLAGS = -Wall -Wextra -Werror -nostdlib -fomit-frame-pointer -mno-apcs-frame -nostartfiles -ffreestanding -g -march=armv6z -marm
ASM_FLAGS = -g -march=armv6z

MKDIR = $(HIDE)mkdir -p
ECHO = $(HIDE)echo
RM = $(HIDE)rm -rf
PRINTF = $(HIDE)printf


LINKERSCRIPT = $(TARGETNAME).ld
MAPFILE = $(MISCDIR)$(TARGETNAME).map

QEMU = qemu-system-arm -kernel $(KERNELELF) -cpu arm1176 -m 512 -M versatilepb -nographic -no-reboot -serial stdio -append "rw earlyprintk loglevel=8 panic=120 keep_bootcon rootwait dma.dmachans=0x7f35 bcm2708_fb.fbwidth=1024 bcm2708_fb.fbheight=768 bcm2708.boardrev=0xf bcm2708.serial=0xcad0eedf smsc95xx.macaddr=B8:27:EB:D0:EE:DF sdhci-bcm2708.emmc_clock_freq=100000000 vc_mem.mem_base=0x1c000000 vc_mem.mem_size=0x20000000  dwc_otg.lpm_enable=0 kgdboc=ttyAMA0,115200 console=ttyS0 root=/dev/mmcblk0p2 rootfstype=ext4 elevator=deadline rootwait" -S -s

GDB = gdb-multiarch

COLOR_END = \033[0m
COLOR_CYAN = \033[1;36m
COLOR_PURPLE = \033[1;35m
COLOR_BLUE = \033[1;34m
COLOR_YELLOW = \033[1;33m
COLOR_GREEN = \033[1;32m
COLOR_RED = \033[1;31m
COLOR_WHITE = \033[1;37m
COLOR_GREY = \033[1;30m

#--------SPECIAL RULES--------#
.PHONY: all clean mrproper emu run
.PRECIOUS: $(PRE) $(ASM) $(OBJ) $(DEP)
.SECONDEXPANSION:

#--------RULES--------#
$(KERNELIMG): $(KERNELELF) $(KERNELLIST)
	$(MKDIR) $(BINDIR)
	$(PRINTF) "$(COLOR_WHITE)%-13s$(COLOR_END) <$@>...\n" "Generating"
	$(CMD_PREFIX)objcopy $(KERNELELF) -O binary $(KERNELIMG)

$(KERNELLIST): $(KERNELELF)
	$(MKDIR) $(MISCDIR)
	$(PRINTF) "$(COLOR_WHITE)%-13s$(COLOR_END) <$@>...\n" "Generating"
	$(CMD_PREFIX)objdump -D $< > $@

$(KERNELELF): $(OBJ) $(LINKERSCRIPT)
	$(MKDIR) $(BINDIR)
	$(MKDIR) $(MISCDIR)
	$(PRINTF) "$(COLOR_CYAN)%-13s$(COLOR_END) <$@>...\n" "Linking"
	$(CMD_PREFIX)ld -Map $(MAPFILE) -o $@ -T $(LINKERSCRIPT) $(OBJ)
	$(ECHO)

define assemble
	$(MKDIR) $(OBJDIR)
	$(PRINTF) "$(COLOR_PURPLE)%-13s$(COLOR_END) <$2>...\n" "Assembling"
	$(CMD_PREFIX)as $(ASM_FLAGS) -o $1 $2
endef

$(OBJC): $(OBJDIR)%.o: $(ASMDIR)%.s
	$(call assemble,$@,$<)

$(OBJASM): $(OBJDIR)%.o: $$(shell find $(SRCDIR) -name '%.s') $(THIS)
	$(call assemble,$@,$<)

$(ASM): $(ASMDIR)%.s: $(PREDIR)%.i
	$(MKDIR) $(ASMDIR)
	$(PRINTF) "$(COLOR_BLUE)%-13s$(COLOR_END) <$<>...\n" "Compiling"
	$(CMD_PREFIX)gcc -S $(CC_FLAGS) -o $@ $<

$(PRE): $(PREDIR)%.i: $$(shell find $(SRCDIR) -name '%.c') $(THIS)
	$(MKDIR) $(PREDIR)
	$(MKDIR) $(DEPDIR)
	$(PRINTF) "$(COLOR_GREY)%-13s$(COLOR_END) <$<>...\n" "Preprocessing"
	$(CMD_PREFIX)gcc -E -o $@ -MMD -MT $@ -MF $(addprefix $(DEPDIR), $(notdir $(<:.c=.d))) $<



#--------PHONY RULES--------#
emu: $(KERNELIMG)
	$(HIDE)$(QEMU)

run: $(KERNELIMG)
	$(PRINTF) "%-13s <qemu>...\n" "Launching"
	$(HIDE)$(QEMU) > $(MISCDIR)qemu.stdout 2> $(MISCDIR)qemu.stderr & \
	QEMU_PID=$$!; \
	printf "%-13s <gdb>...\n" "Launching"; \
	$(GDB) $(KERNELELF) -x gdb/init.gdb; \
	printf "%-13s <qemu>...\n" "Killing"; \
	kill -9 $$QEMU_PID;

all: mrproper $(KERNELIMG)

clean:
	$(RM) $(CLEANDIR)

mrproper:
	$(RM) $(BUILDDIR)

-include $(DEP)
