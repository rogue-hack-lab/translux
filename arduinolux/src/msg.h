#include "font.h"

#define BUFSIZE 256

enum msg_types {
    rowdata,
    blockdata
};

typedef struct _msg_t {
    font_t font;
    byte bits[7][20][4];
    char buf[BUFSIZE];
    unsigned short rowlengths[4]; // sum(rowlengths) must be <= BUFSIZE
    byte rowscrolled[4];
    bool inrowmode;
} msg_t;


// given a null terminated string in s, and a row number in [0, 4), 
// copy the string data into msg buffer, updating msg fields as 
// appropriate.
//
// if row is < 0, switch out of row mode, and just store whole string 
// directly in buffer.
//
void msg_copy_in(msg_t *m, char *s, int row) 
{
    if (!m) return;
    if (row < 0 || row > 3) return;

    int i = 0;
    for (int r=0; i<row; r++) { i += m->rowlengths[r]; }

    char *p = s;
    int j = i;
    while ((i<BUFSIZE) && (*p != '\n') && (*p != '\r')) {
        m->buf[i] = *p;
        i++; p++;
    }
    m->rowlengths[row] = i - j;
}


void msg_init(msg_t *m) 
{
    int i, r;

    if (!m) return;
    font_init(&m->font);
    for (i=0; i<BUFSIZE; i++) { m->buf[i] = ' '; }
    for (r=0; r<4; r++) { 
        m->rowlengths[i] = 0;
        m->rowscrolled[i] = 0;
    }
    m->inrowmode = true;
};


void msg_render(msg_t *m) 
{
    if (!m) return;

    for (int row=0; row < 7; row++) {
        
        // zero all bytes in output first
        for (int cpos=0; cpos<20; cpos++) {
            for (int line=0; line < 4; line++) { m->bits[row][cpos][line] = 0; }
        }
        
        int renderedbyte = 0;
        int renderedbit = 0;
        
        for (int srcbyte=0; srcbyte < 32; srcbyte++) {
            unsigned char rowbytes[4];
            int off=0;
            int r=0;
            while (r<4) {
                if (m->rowlengths[r] < srcbyte) 
                    rowbytes[r] = font_rowdots(&m->font, row, ' ');
                else
                    rowbytes[r] = font_rowdots(&m->font, row, m->buf[off + srcbyte]);
                off += m->rowlengths[r];
                r++;
            }
            
            for (int srcbit=0; srcbit < 5; srcbit++) {
                // do stuff
                for (int i=0; i<4; i++) {
                    int bvalue = ((rowbytes[i]>>srcbit) & 0x1);
                    m->bits[row][renderedbyte][i] |= (bvalue << renderedbit);
                }
                renderedbit++;
                if (renderedbit == 8) {
                    renderedbit = 0;
                    renderedbyte++;
                }
            }
        }
    }
}
