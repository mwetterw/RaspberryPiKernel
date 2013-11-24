$(KERNELIMG): $(KERNELELF)
	$(MKDIR) $(BINDIR)
	$(PRINTF) "$(COLOR_WHITE)%-13s$(COLOR_END) %-30s" "Generating" "<$(notdir $@)>..."
	$(CMD_PREFIX)objcopy $(KERNELELF) -O binary $(KERNELIMG) \
	$(call errorHandler,$@,$<,genimg)
	$(ECHO)
	$(ECHO) "$(COLOR_SUCCESS)BUILD SUCCESSFUL$(COLOR_END)"

$(KERNELLIST): $(KERNELELF)
	$(MKDIR) $(MISCDIR)
	$(PRINTF) "$(COLOR_GEN)%-13s$(COLOR_END) %-30s" "Generating" "<$(notdir $@)>..."
	$(CMD_PREFIX)objdump -D $< > $@ \
	$(call errorHandler,$@,$<,genlist)

$(KERNELELF): $(OBJ) $(LINKERSCRIPT)
	$(MKDIR) $(BINDIR)
	$(MKDIR) $(MISCDIR)
	$(PRINTF) "$(COLOR_LN)%-13s$(COLOR_END) %-30s" "Linking" "<$(notdir $@)>..."
	$(CMD_PREFIX)ld -Map $(MAPFILE) -o $@ -T $(LINKERSCRIPT) $(OBJ) \
	$(call errorHandler,$@,$<,ld)
	$(ECHO)

define assemble
	$(MKDIR) $(OBJDIR)
	$(PRINTF) "$(COLOR_ASM)%-13s$(COLOR_END) %-30s" "Assembling" "<$(notdir $2)>..."
	$(CMD_PREFIX)as $(ASM_FLAGS) -o $1 $2 \
	$(call errorHandler,$1,$2,asm)
endef

$(OBJC): $(OBJDIR)%.o: $(ASMDIR)%.s
	$(call assemble,$@,$<)

$(OBJASM): $(OBJDIR)%.o: $$(shell find $(SRCDIR) -name '%.s') $(THIS)
	$(MKDIR) $(MISCDIR)
	$(call assemble,$@,$<)

$(ASM): $(ASMDIR)%.s: $(PREDIR)%.i
	$(MKDIR) $(ASMDIR)
	$(PRINTF) "$(COLOR_CC)%-13s$(COLOR_END) %-30s" "Compiling" "<$(notdir $<)>..."
	$(CMD_PREFIX)gcc -S $(CC_FLAGS) -o $@ $< \
	$(call errorHandler,$@,$<,cc)

$(PRE): $(PREDIR)%.i: $$(shell find $(SRCDIR) -name '%.c') $(THIS)
	$(MKDIR) $(PREDIR)
	$(MKDIR) $(DEPDIR)
	$(MKDIR) $(MISCDIR)
	$(PRINTF) "$(COLOR_PRE)%-13s$(COLOR_END) %-30s" "Preprocessing" "<$(notdir $<)>..."
	$(CMD_PREFIX)gcc -E -o $@ -MMD -MT $@ -MF $(addprefix $(DEPDIR), $(notdir $(<:.c=.d))) $< \
	$(call errorHandler,$@,$<,pre)
