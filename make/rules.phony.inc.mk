MAKEFLAGS += --output-sync=none
.NOTPARALLEL:

default: $(KERNELIMG)

emu: $(KERNELIMG)
	$(HIDE)$(QEMU) $(QEMU_FLAGS)

define qemugdblauncher
	$(PRINTF) "%-13s <qemu>...\n" "Launching"
	$(HIDE)$(QEMU) $(QEMU_FLAGS) > $(MISCDIR)qemu.stdout 2> $(MISCDIR)qemu.stderr & \
	QEMU_PID=$$!; \
	printf "%-13s <gdb>...\n" "Launching"; \
	$(GDB) $(KERNELELF) -x $1; \
	printf "%-13s <qemu>...\n" "Killing"; \
	kill -9 $$QEMU_PID;
endef

run: $(KERNELIMG)
	$(call qemugdblauncher,$(GDBDEFAULT))

%.gdb: default
	$(call qemugdblauncher,$@)

all: mrproper $(KERNELIMG)

clean:
	$(RM) $(CLEANDIR)

mrproper:
	$(RM) $(BUILDDIR)

list: $(KERNELLIST)

ifdef SDCARD
deploy: $(KERNELIMG) sdcopy umount
	$(ECHO)
	$(ECHO) "$(COLOR_SUCCESS)DEPLOY SUCCESSFUL$(COLOR_END)"

sdcopy: $(KERNELIMG)
	$(PRINTF) "$(COLOR_SDCARD)%-13s$(COLOR_END) %-30s" "Deploying" "<$(notdir $<)>..."
	@cp $< $(SDCARD) \
	$(call errorHandler,$@,$<,sdcard,$@)

umount:
	$(PRINTF) "$(COLOR_SDCARD)%-13s$(COLOR_END) %-30s" "Unmounting" "<SDCARD>..."
	@umount $(SDCARD) \
	$(call errorHandler,$@,$<,sdcard,$@)
else
deploy sdcopy umount:
	$(error "Please set SDCARD variable!")
endif
