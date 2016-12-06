#include "cio.h"
#include <cstring>
#include "debug.h"
#include <cstdlib>

#define BUFFER_MAX 1024


void dump_nal_buffer(nal_buffer_t * pnal_buffer) {
	for (int i = 0; i < pnal_buffer->posmax; i++) {
		if (i % 16 == 0) {
			debug("\t%04x:  ", i);
		}
		debug("%02x ", pnal_buffer->data[i]);
		if (i % 16 == 15) {
			debug("\n");
		}
	}
	debug("\n");
}

void copy_to_nal_buf(nal_buffer_t * pnal_buffer, uint8 c) {
	if (pnal_buffer->pos >= NAL_BUFFER_MAX )
	{
		fprintf(stderr, "\nNAL unit is too big. NAL_BUFFER_MAX need to be increased to fix.\n at %s, line %d.\n",
              __FILE__, __LINE__);
		exit(1);
	}
	pnal_buffer->data[pnal_buffer->pos] = c;
	pnal_buffer->pos++;
}

//write nal_buffer_t to file as is
void copy_nal_to_file(nal_buffer_t* pnal_buffer, FILE* f)
{
    const uint8 SODB_CODE[1] = { 0x03 };
	const uint8 START_CODE[3] = { 0x00, 0x00, 0x01 };
	const uint8 ZERO_BITS[1] = { 0x00 };
	if (pnal_buffer->posmax)
	{
		fwrite(ZERO_BITS, sizeof(ZERO_BITS), 1, f);
		fwrite(START_CODE, sizeof(START_CODE), 1, f);
		fwrite(pnal_buffer->data, pnal_buffer->posmax, 1, f);
	}

}

//write nal_buffer data with encapsulation of an SODB 
void write_nal_data_to_file(nal_buffer_t* pnal_buffer, FILE* f)
{
	const uint8 SODB_CODE[1] = { 0x03 };
	const uint8 START_CODE[3] = { 0x00, 0x00, 0x01 };
	const uint8 ZERO_BITS[1] = { 0x00 };

    fwrite(ZERO_BITS, sizeof(ZERO_BITS), 1, f);
	debug("%02X ", ZERO_BITS[0]);
	fwrite(START_CODE, sizeof(START_CODE), 1, f);
	debug("%02X %02X %02X ", START_CODE[0],START_CODE[1], START_CODE[2]);
	for (int i = 0; i <= pnal_buffer->posmax; i++)
	{
		if (i + 2 < pnal_buffer->posmax &&
			(
				(pnal_buffer->data[i] == 0 && pnal_buffer->data[i + 1] == 0 && pnal_buffer->data[i + 2] == 0) ||
				(pnal_buffer->data[i] == 0 && pnal_buffer->data[i + 1] == 0 && pnal_buffer->data[i + 2] == 1) ||
				(pnal_buffer->data[i] == 0 && pnal_buffer->data[i + 1] == 0 && pnal_buffer->data[i + 2] == 2) ||
				(pnal_buffer->data[i] == 0 && pnal_buffer->data[i + 1] == 0 && pnal_buffer->data[i + 2] == 3)
			)
		)
		{
			fwrite(&pnal_buffer->data[i], sizeof(uint8), 1, f);
			debug("%02X ", pnal_buffer->data[i]);
			fwrite(&pnal_buffer->data[i + 1], sizeof(uint8), 1, f);
			debug("%02X ", pnal_buffer->data[i + 1]);
			fwrite(SODB_CODE, sizeof(SODB_CODE), 1, f);
			debug("%02X ", SODB_CODE[0]);
			i += 1;
		}
		else
		{
			fwrite(&pnal_buffer->data[i], sizeof(uint8), 1, f);
			debug("%02X ", pnal_buffer->data[i]);
		}
	}

	if (pnal_buffer->data[pnal_buffer->posmax] == 0)
	{
		fwrite(SODB_CODE, sizeof(SODB_CODE), 1, f);
		debug("%02X ", SODB_CODE[0]);
	}
}

