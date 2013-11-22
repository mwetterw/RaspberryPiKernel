THIS := $(lastword MAKEFILE_LIST)

BUILDDIR = build/
DEPDIR = $(BUILDDIR)dep/
PREDIR = $(BUILDDIR)pre/
ASMDIR = $(BUILDDIR)asm/
OBJDIR = $(BUILDDIR)obj/
BINDIR = $(BUILDDIR)bin/
MISCDIR = $(BUILDDIR)misc/
SRCDIR = src/

EXTDEP = .d
EXTPRE = .i
EXTASM = .s
EXTOBJ = .o

SOURCES_C = $(shell find $(SRCDIR) -iname "*.c")
SOURCES_ASM = $(shell find $(SRCDIR) -iname "*.s")

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


#--------SPECIAL RULES--------#
.PHONY: all clean mrproper test
.PRECIOUS: $(PRE) $(ASM) $(OBJ) $(DEP)
.SECONDEXPANSION:

#--------RULES--------#
$(KERNELIMG): $(KERNELELF) $(KERNELLIST)
	$(MKDIR) $(BINDIR)
	$(PRINTF) "%-13s <$@>...\n" "Generating"
	$(CMD_PREFIX)objcopy $(KERNELELF) -O binary $(KERNELIMG)

$(KERNELLIST): $(KERNELELF)
	$(MKDIR) $(MISCDIR)
	$(PRINTF) "%-13s <$@>...\n" "Generating"
	$(CMD_PREFIX)objdump -d $< > $@

$(KERNELELF): $(OBJ) $(LINKERSCRIPT)
	$(MKDIR) $(BINDIR)
	$(MKDIR) $(MISCDIR)
	$(PRINTF) "%-13s <$@>...\n" "Linking"
	$(CMD_PREFIX)ld -Map $(MAPFILE) -o $@ -T $(LINKERSCRIPT) $(OBJ)
	$(ECHO)

$(OBJC): $(OBJDIR)%.o: $(ASMDIR)%.s
	$(MKDIR) $(OBJDIR)
	$(PRINTF) "%-13s <$<>...\n" "Assembling"
	$(CMD_PREFIX)as $(ASM_FLAGS) -o $@ $<

$(OBJASM): $(OBJDIR)%.o: $$(shell find $(SRCDIR) -iname '%.s')
	$(MKDIR) $(OBJDIR)
	$(PRINTF) "%-13s <$<>...\n" "Assembling"
	$(CMD_PREFIX)as $(ASM_FLAGS) -o $@ $<

$(ASM): $(ASMDIR)%.s: $(PREDIR)%.i
	$(MKDIR) $(ASMDIR)
	$(PRINTF) "%-13s <$<>...\n" "Compiling"
	$(CMD_PREFIX)gcc -S $(CC_FLAGS) -o $@ $<

$(PRE): $(PREDIR)%.i: $$(shell find $(SRCDIR) -iname '%.c')
	$(MKDIR) $(PREDIR)
	$(MKDIR) $(DEPDIR)
	$(PRINTF) "%-13s <$<>...\n" "Preprocessing"
	$(CMD_PREFIX)gcc -E -o $@ -MMD -MT $@ -MF $(addprefix $(DEPDIR), $(notdir $(<:.c=.d))) $<

mrproper:
	$(RM) $(BUILDDIR)
