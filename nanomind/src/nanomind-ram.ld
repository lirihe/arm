OUTPUT_FORMAT(elf32-littlearm)
OUTPUT_ARCH(arm)
ENTRY(_start)

MEMORY
{
    sram  : ORIGIN = 0x00000000,	LENGTH = 0x1000
    rom   : ORIGIN = 0x40000000,	LENGTH = 0x400000
    flash : ORIGIN = 0x48000000,	LENGTH = 0x400000
    ram   : ORIGIN = 0x50000000,	LENGTH = 0x200000
}

SECTIONS 
{

	. = 0;
	_sfixed = .;
    
    .text : {
    	KEEP(*(.init))
        . = 0x100;
        KEEP(*(.jump))
		*(.text)
		*(.text.*)
        *(.rodata*)
        . = ALIGN(8);
		PROVIDE(__init_array_start = .);
		KEEP(*(.init_array))
		PROVIDE(__init_array_end = .);	
	} >ram
	
	.ARM.extab (NOLOAD) : {
    	*(.ARM.extab* .gnu.linkonce.armextab.*)
    } >ram
    
    __exidx_start = .;
  	.ARM.exidx (NOLOAD) : {
  		*(.ARM.exidx* .gnu.linkonce.armexidx.*)
  	} >ram
   	__exidx_end = .;
	
	_efixed = .;

	.data : {
		_data_lma = LOADADDR(.data);
        _data_start = .;
        _data_end = .;
        *(.ramfunc)
        *(.data)
    	*(.data.*)
        PROVIDE (__command_start = .);
    	*(.commands);
    	PROVIDE (__command_end = .);
    } >ram
    
	.sram 0x40 : {
		_sram_lma = LOADADDR(.sram);
		_sram_start = .;
		*(.sram.text)
		*(.sram.data)
		_sram_end = .;
	} >sram AT >ram
	
    .bss (NOLOAD) : {
        _szero = .;
        *(.bss)
        *(.bss.*)
        *(COMMON)
        . = ALIGN(8);
        _ezero = .;
    } >ram
    
    __heap_start__ = .;
    __heap_end__ = 0x50200000 - 8;
        
    .ram_persist __heap_end__ (NOLOAD) : {
    	*(.ram_persist)
    } >ram
        
    _sstack = 0x50200000 - 8;

}
