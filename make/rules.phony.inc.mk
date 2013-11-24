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

