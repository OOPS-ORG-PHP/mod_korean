/*
 * see also http://phpschool.com/bbs2/inc_view.html?id=9185&code=tnt2
 */
#include <malloc.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "gd.h"

#define GIFBITS 12
#define HSIZE 5003		/* 80% occupancy */
#define MAXCODE(n_bits)	(((int) 1 << (n_bits)) - 1)
typedef long int count_int;

//전역변수 정의
int a_count, curx, cury, free_ent, clear_flg, g_init_bits, ClearCode, EOFCode;
int n_bits, cur_bits, maxcode, maxbits, maxmaxcode;
long total_bits;
gdIOCtx* g_outfile;
char accum[ 256 ];
count_int htab [HSIZE];
unsigned short codetab [HSIZE];
int hsize = HSIZE;		/* for dynamic table sizing */
unsigned long cur_accum = 0;
unsigned long masks[] = { 
	0x0000, 0x0001, 0x0003, 0x0007, 0x000F, 0x001F, 0x003F, 0x007F, 0x00FF,
	0x01FF, 0x03FF, 0x07FF, 0x0FFF, 0x1FFF, 0x3FFF, 0x7FFF, 0xFFFF 
};

//보조함수 정의
void output (int code);
int gif_step (gdImagePtr im);
void cl_hash (register count_int hsize);
void char_out (int c);

void gdImageGif(gdImagePtr im, FILE *outFile) {
	gdIOCtx *out = gdNewFileCtx(outFile);
	gdImageGifCtx(im, out);
	out->gd_free(out);
}

void* gdImageGifPtr(gdImagePtr im, int *size) {
	void    *rv;
	gdIOCtx *out = gdNewDynamicCtx(2048, NULL);
	gdImageGifCtx(im, out);
	rv = gdDPExtractData(out,size);
	out->gd_free(out);
	return rv;
}

void gdImageGifCtx(gdImagePtr im, gdIOCtx *out) {
	//지역변수 초기화
	int bpp, InitCodeSize;
	register long fcode;
	register int i, c, ent, disp, hsize_reg, hshift;
	for (bpp=1; bpp<9 && ( (im->colorsTotal -1 ) >>bpp != 0); bpp++);
	InitCodeSize = (bpp < 2) ? 2 : bpp;

	//전역변수 초기화
	curx = cury = a_count = cur_accum = cur_bits = n_bits = clear_flg = 0;
	hsize = HSIZE;
	maxbits = GIFBITS;
	maxmaxcode = 1 << GIFBITS;
	total_bits = (long)(im->sx) * (long)(im->sy);

	//GIF파일 내용 쓰기
	gdPutBuf( im->transparent < 0 ? "GIF87a" : "GIF89a", 6, out ); //Image Header
	Putword( im->sx, out );
	Putword( im->sy, out );

	gdPutC( 0x80 | ((bpp - 1) << 5) | (bpp - 1), out );		
	gdPutC( 0, out ); //Background
	gdPutC( 0, out );

	for(i=0; i<(1<<bpp); ++i ) {
		gdPutC( im->red[i], out );
		gdPutC( im->green[i], out );
		gdPutC( im->blue[i], out );
	}

	if ( im->transparent >= 0 ) {
		gdPutC( '!', out );
		gdPutC( 0xf9, out );
		gdPutC( 4, out );
		gdPutC( 1, out );
		gdPutC( 0, out );
		gdPutC( 0, out );
		gdPutC( (unsigned char) im->transparent, out );
		gdPutC( 0, out );
	}

	gdPutC( ',', out );
	Putword( 0, out );
	Putword( 0, out );
	Putword( im->sx, out );
	Putword( im->sy, out );
	gdPutC( 0x00, out );
	gdPutC( InitCodeSize, out );

	//여태까지 쓴 내용 압축하기
	g_init_bits = InitCodeSize+1;
	g_outfile = out;
	maxcode = MAXCODE(n_bits = g_init_bits);
	ClearCode = (1 << InitCodeSize);
	EOFCode = ClearCode + 1;
	free_ent = ClearCode + 2;
	ent = gif_step( im );
	hshift = 0;
	for ( fcode = (long) hsize; fcode < 65536L; fcode *= 2L ) hshift++;
	hshift = 8 - hshift;		/* set hash code range bound */
	hsize_reg = hsize;
	cl_hash( (count_int) hsize_reg);	/* clear hash table */
	output( (int)ClearCode );

	while ( (c = gif_step( im )) != EOF ) { /* } */
		fcode = (long) (((long) c << maxbits) + ent);
		i = (((int)c << hshift) ^ ent);	/* xor hashing */

		if ( htab[i] == fcode ) {
			ent = codetab[i];
			continue;
		} 
		else if ( (long)htab[i] < 0 ) goto nomatch;
	
		disp = hsize_reg - i;		/* secondary hash (after G. Knott) */
		if ( i == 0 ) disp = 1;

		probe:
			if ( (i -= disp) < 0 ) i += hsize_reg;
			if ( htab[i] == fcode ) {
				ent = codetab[i];
				continue;
			}
			if ( (long)htab[i] > 0 ) goto probe;

		nomatch:
			output ( (int) ent );
			ent = c;

		if ( free_ent < maxmaxcode ) { /* } */
			codetab[i] = free_ent++; /* code -> hashtable */
			htab[i] = fcode;
		} 
		else {
			cl_hash ( (count_int) hsize );
			free_ent = ClearCode + 2;
			clear_flg = 1;
			output( (int)ClearCode );
		}
	}

	output( (int)ent );
	output( (int) EOFCode );

	//GIF 파일 쓰기 끝
	gdPutC( 0, out );
	gdPutC( ';', out );
}

void output(int code) {
	cur_accum &= masks[ cur_bits ];
	if( cur_bits > 0 ) cur_accum |= ((long)code << cur_bits);
	else cur_accum = code;
	cur_bits += n_bits;

	while( cur_bits >= 8 ) {
		char_out( (unsigned int)(cur_accum & 0xff) );
		cur_accum >>= 8;
		cur_bits -= 8;
	}

	if ( free_ent > maxcode || clear_flg ) {
		if( clear_flg ) {
			maxcode = MAXCODE (n_bits = g_init_bits);
			clear_flg = 0;
		} 
		else {
			++n_bits;
			if ( n_bits == maxbits ) maxcode = maxmaxcode;
			else maxcode = MAXCODE(n_bits);
		}
	}

	if( code == EOFCode ) {
		while( cur_bits > 0 ) {
			char_out( (unsigned int)(cur_accum & 0xff) );
			cur_accum >>= 8;
			cur_bits -= 8;
		}
		if( a_count > 0 ) {
			gdPutC( a_count, g_outfile );
			gdPutBuf( accum, a_count, g_outfile );
			a_count = 0;
		}
	}
}

int gif_step(gdImagePtr im) {
	int pixel;
	if( total_bits == 0 ) return EOF;
	total_bits--;
	pixel = gdImageGetPixel(im, curx++, cury);
	if( curx == im->sx ) { //End of line
		curx = 0;
		cury++;
	}
	return pixel;
}

void cl_hash(register count_int hsize) {
	register count_int *htab_p = htab+hsize;
	register long i = hsize - 16;
	int j;

	do {			/* might use Sys V memset(3) here */
		for (j=1; j<17; j++) *(htab_p-j) = -1;
		htab_p -= 16;
	} while ((i -= 16) >= 0);

	for ( i += 16; i > 0; --i ) *--htab_p = -1;
}

void char_out(int c) {
	accum[ a_count++ ] = c;
	if( a_count >= 254 ) {
		gdPutC( a_count, g_outfile );
		gdPutBuf( accum, a_count, g_outfile );
		a_count = 0;
	}
}
