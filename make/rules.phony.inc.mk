MAKEFLAGS += --output-sync=none
.NOTPARALLEL:

emu: $(KERNELIMG)
	$(HIDE)$(QEMU) $(QEMU_FLAGS)

run: $(KERNELIMG)
	$(PRINTF) "%-13s <qemu>...\n" "Launching"
	$(HIDE)$(QEMU) $(QEMU_FLAGS) > $(MISCDIR)qemu.stdout 2> $(MISCDIR)qemu.stderr & \
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
