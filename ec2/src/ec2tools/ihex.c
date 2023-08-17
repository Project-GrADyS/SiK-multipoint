/**	Intel HEX read/write functions, Paul Stoffregen, paul@ece.orst.edu
  * This code is in the public domain.  Please retain my name and
  * email address in distributed copies, and let me know about any bugs
  * 
  * I, Paul Stoffregen, give no warranty, expressed or implied for
  * this software and/or documentation provided, including, without
  * limitation, warranty of merchantability and fitness for a
  * particular purpose.
  * Origional source obtained from http://www.pjrc.com/tech/8051/ihex.c
  *
  *=======================================================================
  *
  * Modifications by Ricky white
  *   Copyright (C) 2005 by Ricky White
  *   rickyw@neatstuff.co.nz
  *
  *   This program is free software; you can redistribute it and/or modify
  *   it under the terms of the GNU General Public License as published by
  *   the Free Software Foundation; either version 2 of the License, or
  *   (at your option) any later version.
  *
  *   This program is distributed in the hope that it will be useful,
  *   but WITHOUT ANY WARRANTY; without even the implied warranty of
  *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  *   GNU General Public License for more details.
  *
  *   You should have received a copy of the GNU General Public License
  *   along with this program; if not, write to the
  *   Free Software Foundation, Inc.,
  *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
  */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "ihex.h"


/** Parses a line of intel hex code, stores the data in bytes[]
  * and the beginning address in addr, and returns a 1 if the
  * line was valid, or a 0 if an error occured.  The variable
  * num gets the number of bytes that were stored into bytes[]
  *
  * \param theline pointer to a buffer containing the line to process
  * \param bytes[]	Buffer to recieve the data read from the line
  * \param addr		Recieves the start address from the line being processed
  * \param num		Recieves the number of bytes written into byte[]
  * \param code		Recieves the record type for the line
  *
  * \returns 		0 on failure, 1 on success
  */
int parse_hex_line( char *theline,
					int bytes[],
					uint32_t *addr,
					uint32_t *num,
					int *code)
{
	int sum, len, cksum;
	char *ptr;
	
	*num = 0;
	if (theline[0] != ':') return 0;
	if (strlen(theline) < 11) return 0;
	ptr = theline+1;
	if (!sscanf(ptr, "%02x", &len)) return 0;
	ptr += 2;
	if ( strlen(theline) < (11 + (len * 2)) ) return 0;
	if (!sscanf(ptr, "%04x", addr)) return 0;
	ptr += 4;
	  /* printf("Line: length=%d Addr=%d\n", len, *addr); */
	if (!sscanf(ptr, "%02x", code)) return 0;
	ptr += 2;
	sum = (len & 255) + ((*addr >> 8) & 255) + (*addr & 255) + (*code & 255);
	while(*num != len) {
		if (!sscanf(ptr, "%02x", &bytes[*num])) return 0;
		ptr += 2;
		sum += bytes[*num] & 255;
		(*num)++;
		if (*num >= 256) return 0;
	}
	if (!sscanf(ptr, "%02x", &cksum)) return 0;
	if ( ((sum & 255) + (cksum & 255)) & 255 ) return 0; /* checksum error */
	return 1;
}

/** Load an intel hex file into the supplied buffer
  *
  * \param filename		Name of the file to load
  * \param memory		Buffer to recieve the file contents in binary form.
  *						Must be at least 65536 bytes in length
  * \param start		will recieve the lowest address read
  * \param end			will recieve the highest address read
  */
void ihex_load_file( char *filename, uint8_t *memory,
					 uint32_t *start, uint32_t *end  )
{
	char line[1000];
	FILE *fin;
	uint32_t addr;
	int status, bytes[256];
	uint32_t n;
	uint32_t i, total=0, lineno=1;
	uint32_t minaddr=0x1FFFF, maxaddr=0;
	uint32_t offset=0;
	
	if (strlen(filename) == 0) {
		printf("   Can't load a file without the filename.");
		printf("  '?' for help\n");
		return;
	}
	fin = fopen(filename, "r");
	if (fin == NULL) {
		printf("   Can't open file '%s' for reading.\n", filename);
		return;
	}
	while (!feof(fin) && !ferror(fin)) {
		line[0] = '\0';
		fgets(line, 1000, fin);
		if (line[strlen(line)-1] == '\n') line[strlen(line)-1] = '\0';
		if (line[strlen(line)-1] == '\r') line[strlen(line)-1] = '\0';
		if (parse_hex_line(line, bytes, &addr, &n, &status))
		{
			if (status == 0) {  /* data */
				for(i=0; i<=(n-1); i++) {
					memory[addr+offset] = bytes[i] & 255;
					total++;
					if ((addr+offset) < minaddr) minaddr = addr+offset;
					if ((addr+offset) > maxaddr) maxaddr = addr+offset;
					addr++;
				}
			}
			if (status == 1) {  /* end of file */
				fclose(fin);
				printf("   Loaded %d bytes between:", total);
				printf(" %04X to %04X\n", minaddr, maxaddr);
				*start = minaddr;
				*end = maxaddr;
				return;
			}
			if( status==2 )  /* begin of file Extended address record */
			{
				uint32_t segment;
				segment = (bytes[0]<<8)|bytes[1];	// upper 16 bits of address.
				offset = segment<<4;
				printf("Segment = 0x%04x, addr = 0x%08x\n", segment, offset );
			}
			if (status == 4)	/* Linear address record */
			{
				offset = (bytes[0]<<8)|bytes[1];	// upper 16 bits of address.
				printf("Linear Address = 0x%04x\n", offset );
			}
		}
		else
		{
			printf("   Error: '%s', line: %d\n", filename, lineno);
		}
		lineno++;
	}
}


/** Save a block of data an an intel hex file
  *
  * \param filename		Name to save the file as
  * \param memory		Buffer containing the data to write
  * \param start_addr	Address torepresent the first address in memory in the output file
  * \param len			Number of bytes to write
  */
void ihex_save_file( char *filename, uint8_t *memory,
					 uint32_t start_addr, uint32_t len )
{
	int begin	= start_addr;
	int end		= begin+len-1;
	int addr;
	FILE *fhex;

	fhex = fopen(filename, "w");
	if( fhex == NULL )
	{
		printf("   Can't open '%s' for writing.\n", filename);
		return;
	}
	for (addr=begin; addr <= end; addr++)
		ihex_hexout(fhex, memory[addr], addr, 0);
	ihex_hexout( fhex, 0, 0, 1 );
	printf( "Memory %04X to %04X written to '%s'\n", begin, end, filename );
}



#define MAXHEXLINE 32	/* the maximum number of bytes to put in one line */

/** produce intel hex file output... call this routine with
  * each byte to output and it's memory location.  The file
  * pointer fhex must have been opened for writing.  After
  * all data is written, call with end=1 (normally set to 0)
  * so it will flush the data from its static buffer
  *
  * \param fhex				valid file handle
  * \param byte				byte to output to file
  * \param memory_location	address of byte
  * \param end				call with end=0 for all but the last byte of the
  *							file in which case end=1
  */
void ihex_hexout( FILE *fhex, int byte, uint32_t memory_location, uint32_t end )
{
	static int byte_buffer[MAXHEXLINE];
	static uint32_t last_mem, buffer_pos, buffer_addr;
	static int writing_in_progress=0;
	int seg_swap = 0;
	register int i, sum;

	if (!writing_in_progress)
	{
		/* initial condition setup */
		last_mem = memory_location-1;
		buffer_pos = 0;
		buffer_addr = memory_location;
		writing_in_progress = 1;
	}

	uint32_t last_seg, new_seg;	// using extended linear address
	last_seg = last_mem>>16;
	new_seg = memory_location>>16;
	seg_swap = last_seg!=new_seg;
	if(seg_swap) printf("seg_swap 0x%08x 0x%08x\n",last_seg,new_seg);

	if ( (memory_location != (last_mem+1)) || (buffer_pos >= MAXHEXLINE) \
			 || ((end) && (buffer_pos > 0)) || seg_swap)
	{
		/* it's time to dump the buffer to a line in the file */
		fprintf(fhex, ":%02X%04X00", buffer_pos, buffer_addr);
		sum = buffer_pos + ((buffer_addr>>8)&255) + (buffer_addr&255);
		for (i=0; i < buffer_pos; i++) {
			fprintf(fhex, "%02X", byte_buffer[i]&255);
			sum += byte_buffer[i]&255;
		}
		fprintf(fhex, "%02X\n", (-sum)&255);
		buffer_addr = memory_location;
		buffer_pos = 0;
	}
	if(seg_swap)
	{
		// write new extended linear address
		fprintf(fhex, ":04000002" );
		sum = 0x04+0x00+0x00+0x02;
		fprintf(fhex, "%02X%02X",(new_seg>>8)&0xff,new_seg&0xff );
		sum += (new_seg>>8)&0xff;
		sum += new_seg&0xff;
		fprintf(fhex, "%02X\n", (-sum)&255);
	}
	
	if (end) {
		fprintf(fhex, ":00000001FF\n");  /* end of file marker */
		fclose(fhex);
		writing_in_progress = 0;
	}
		
	last_mem = memory_location;
	byte_buffer[buffer_pos] = byte & 255;
	buffer_pos++;
}

