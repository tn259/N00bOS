/* https://ftp.gnu.org/old-gnu/Manuals/ld-2.9.1/html_chapter/ld_3.html */

ENTRY(_start)
OUTPUT_FORMAT(binary)
SECTIONS /* sections or segments */
{
    /* the ',' character is the location counter */
    . = 1M; /* kernel or sections will be loaded at 1M into memory */
    .text : ALIGN(4096) /* code */
    {
        *(.text) /* (.x) means include all input files under the .text section */
    }

    .rodata : ALIGN(4096) /* read only global/static data */
    {
        *(.rodata)
    }

    .data : ALIGN(4096) /* all initialised global/static data */
    {
        *(.data)
    }

    .bss : ALIGN(4096) /* block started by symbol (all unitialised) global/static data */
    {
        *(COMMON) 
        *(.bss)
    }

    /* without the asm section tag, as long as kernel.asm.o is the first object file to be linked (in the text section) it will be first to be run */
    .asm : ALIGN(4096) /* this section is at the bottom and not before .text to protect and from any misalignment of the C code */
    {
        /* within here asm code is not aligined */
        *(.asm)
    }
}
