/* https://ftp.gnu.org/old-gnu/Manuals/ld-2.9.1/html_chapter/ld_3.html */

ENTRY(_start)
OUTPUT_FORMAT(binary)
SECTIONS /* sections or segments */
{
    /* the ',' character is the location counter */
    . = 1M; /* kernel or sections will be loaded at 1M into memory */
    .text : /* code */
    {
        *(.text) /* (.x) means include all input files under the .text section */
    }

    .rodata : /* read only global/static data */
    {
        *(.rodata)
    }

    .data : /* all initialised global/static data */
    {
        *(.data)
    }

    .bss : /* block started by symbol (all unitialised) global/static data */
    {
        *(COMMON) 
        *(.bss)
    }
}
