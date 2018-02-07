define errorHandler
	2> $(MISCDIR)$(notdir $1).err; \
		if [ $$? -ne 0 ]; then \
			echo " $(COLOR_RED)[ KO ]$(COLOR_END)"; \
			$(call printError,$3,$4) \
			cat $(MISCDIR)$(notdir $1).err; \
			echo ""; \
			case $3 in \
			"build") \
				echo "$(COLOR_ERROR)BUILD FAILED$(COLOR_END)"; \
				;; \
			"sdcard") \
				echo "$(COLOR_ERROR)DEPLOY FAILED$(COLOR_END)"; \
				;; \
			*) \
				echo "$(COLOR_ERROR)UNDEFINED ACTION FAILED$(COLOR_END)"; \
			esac; \
			false; \
		else \
			rm -f $(MISCDIR)$(notdir $1).err; \
			echo " $(COLOR_GREEN)[ OK ]$(COLOR_END)"; \
		fi;
endef

define printError
	echo ""; \
	echo -n "$(COLOR_ERROR)There are$(COLOR_END) "; \
	case $1 in \
	"build") \
		case $2 in \
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
		*) \
			echo -n "$(COLOR_GEN)BUILD MISC$(COLOR_END)"; \
		esac; \
		;; \
	"sdcard") \
		echo -n "$(COLOR_SDCARD)SDCARD$(COLOR_END)"; \
		;; \
	*) \
		echo -n "$(COLOR_RED)UNDEFINED$(COLOR_END)"; \
	esac; \
	echo " $(COLOR_ERROR)errors:$(COLOR_END)";
endef

