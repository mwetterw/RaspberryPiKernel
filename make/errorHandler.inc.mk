define errorHandler
	2> $(MISCDIR)$(notdir $2).err; \
		if [ $$? -ne 0 ]; then \
			echo " $(COLOR_RED)[ KO ]$(COLOR_END)"; \
			$(call printError,$1,$2,$3) \
			false; \
		else \
			rm -f $(MISCDIR)$(notdir $2).err; \
			echo " $(COLOR_GREEN)[ OK ]$(COLOR_END)"; \
		fi;
endef

define printError
	echo ""; \
	echo -n "$(COLOR_ERROR)There are$(COLOR_END) "; \
	case $3 in \
	"pre") \
		echo -n "$(COLOR_PRE)PREPROCESSING$(COLOR_END)"; \
		;; \
	"cc") \
		echo -n "$(COLOR_CC)COMPILING$(COLOR_END)"; \
		;; \
	"asm") \
		echo -n "$(COLOR_ASM)ASSEMBLING$(COLOR_END)"; \
		;; \
	"ld") \
		echo -n "$(COLOR_LN)LINKING$(COLOR_END)"; \
		;; \
	"genlist") \
		echo -n "$(COLOR_GEN)LISTING GENERATION$(COLOR_END)"; \
		;; \
	"genimg") \
		echo -n "$(COLOR_GEN)KERNEL GENERATION$(COLOR_END)"; \
		;; \
	esac; \
	echo " $(COLOR_ERROR)errors:$(COLOR_END)"; \
	cat $(MISCDIR)$(notdir $2).err; \
	echo ""; \
	echo "$(COLOR_ERROR)BUILD FAILED$(COLOR_END)";
endef

