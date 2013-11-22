ENTRY(_start)

SECTIONS
{
	. = 0x8000;
	_start = .;
	_text_start = .;
	.text :
	{
		*kernel_boot.o (.text)
		*(.text)
	}

    . = ALIGN(4096); /* align to page size */
    _text_end = .;

    _rodata_start = .;
    .rodata :
    {
        *(.rodata)
    }
    . = ALIGN(4096); /* align to page size */
    _rodata_end = .;

    _data_start = .;
    .data :
    {
        *(.data)
    }
    . = ALIGN(4096); /* align to page size */
    _data_end = .;

    _bss_start = .;
    .bss :
    {
        bss = .;
        *(.bss)
    }
    . = ALIGN(4096); /* align to page size */
    _bss_end = .;
    _end = .;
}
