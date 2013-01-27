/*
 * Use an arduino to drive our translux display (it has 4x32 LED modules, each 5x7 pixels)
 *
 * Note: the pin connections for the 2x5 connector are as follows:
 *
 *            +----+
 *   gnd top  |2  1| clk top
 *   gnd top  |4  3| input top
 * enable top |5  6| enable bottom
 * input btm. |7  8| gnd bottom
 *   clk btm. |10 9| gnd bottom
 *            +----+
 *
 * Note that all the ground pins are tied together on the board. Also note: if you connect
 * the wrong enable pin, the board will display whatever random data happens to be in the 
 * shift registers (only one line, since the row select bits will be random too), and never
 * change no matter what data you send. Not that I would ever spend almost an entire 
 * Focused Hack Night trying to figure out why the LEDs don't change no matter what data 
 * I send. Sheesh.
 */

typedef struct _linePins {
  int clock, data, rowEnable;
} linePins;

linePins pins[2] = {
  // { clk, data, rowEnable}
     {   5,    6,         7}, // first line of LEDs
     {  11,   12,        13}  // second line of LEDs
};

// font starts at ascii 0x20 (space) and goes up to 0x7e
unsigned char font1[][5] = {
{0x00, 0x00, 0x00, 0x00, 0x00}, // (space)
{0x00, 0x00, 0x5F, 0x00, 0x00}, // !
{0x00, 0x07, 0x00, 0x07, 0x00}, // "
{0x14, 0x7F, 0x14, 0x7F, 0x14}, // #
{0x24, 0x2A, 0x7F, 0x2A, 0x12}, // $
{0x23, 0x13, 0x08, 0x64, 0x62}, // %
{0x36, 0x49, 0x55, 0x22, 0x50}, // &
{0x00, 0x05, 0x03, 0x00, 0x00}, // '
{0x00, 0x1C, 0x22, 0x41, 0x00}, // (
{0x00, 0x41, 0x22, 0x1C, 0x00}, // )
{0x08, 0x2A, 0x1C, 0x2A, 0x08}, // *
{0x08, 0x08, 0x3E, 0x08, 0x08}, // +
{0x00, 0x50, 0x30, 0x00, 0x00}, // ,
{0x08, 0x08, 0x08, 0x08, 0x08}, // -
{0x00, 0x60, 0x60, 0x00, 0x00}, // .
{0x20, 0x10, 0x08, 0x04, 0x02}, // /
{0x3E, 0x51, 0x49, 0x45, 0x3E}, // 0
{0x00, 0x42, 0x7F, 0x40, 0x00}, // 1
{0x42, 0x61, 0x51, 0x49, 0x46}, // 2
{0x21, 0x41, 0x45, 0x4B, 0x31}, // 3
{0x18, 0x14, 0x12, 0x7F, 0x10}, // 4
{0x27, 0x45, 0x45, 0x45, 0x39}, // 5
{0x3C, 0x4A, 0x49, 0x49, 0x30}, // 6
{0x01, 0x71, 0x09, 0x05, 0x03}, // 7
{0x36, 0x49, 0x49, 0x49, 0x36}, // 8
{0x06, 0x49, 0x49, 0x29, 0x1E}, // 9
{0x00, 0x36, 0x36, 0x00, 0x00}, // :
{0x00, 0x56, 0x36, 0x00, 0x00}, // ;
{0x00, 0x08, 0x14, 0x22, 0x41}, // <
{0x14, 0x14, 0x14, 0x14, 0x14}, // =
{0x41, 0x22, 0x14, 0x08, 0x00}, // >
{0x02, 0x01, 0x51, 0x09, 0x06}, // ?
{0x32, 0x49, 0x79, 0x41, 0x3E}, // @
{0x7E, 0x11, 0x11, 0x11, 0x7E}, // A
{0x7F, 0x49, 0x49, 0x49, 0x36}, // B
{0x3E, 0x41, 0x41, 0x41, 0x22}, // C
{0x7F, 0x41, 0x41, 0x22, 0x1C}, // D
{0x7F, 0x49, 0x49, 0x49, 0x41}, // E
{0x7F, 0x09, 0x09, 0x01, 0x01}, // F
{0x3E, 0x41, 0x41, 0x51, 0x32}, // G
{0x7F, 0x08, 0x08, 0x08, 0x7F}, // H
{0x00, 0x41, 0x7F, 0x41, 0x00}, // I
{0x20, 0x40, 0x41, 0x3F, 0x01}, // J
{0x7F, 0x08, 0x14, 0x22, 0x41}, // K
{0x7F, 0x40, 0x40, 0x40, 0x40}, // L
{0x7F, 0x02, 0x04, 0x02, 0x7F}, // M
{0x7F, 0x04, 0x08, 0x10, 0x7F}, // N
{0x3E, 0x41, 0x41, 0x41, 0x3E}, // O
{0x7F, 0x09, 0x09, 0x09, 0x06}, // P
{0x3E, 0x41, 0x51, 0x21, 0x5E}, // Q
{0x7F, 0x09, 0x19, 0x29, 0x46}, // R
{0x46, 0x49, 0x49, 0x49, 0x31}, // S
{0x01, 0x01, 0x7F, 0x01, 0x01}, // T
{0x3F, 0x40, 0x40, 0x40, 0x3F}, // U
{0x1F, 0x20, 0x40, 0x20, 0x1F}, // V
{0x7F, 0x20, 0x18, 0x20, 0x7F}, // W
{0x63, 0x14, 0x08, 0x14, 0x63}, // X
{0x03, 0x04, 0x78, 0x04, 0x03}, // Y
{0x61, 0x51, 0x49, 0x45, 0x43}, // Z
{0x00, 0x00, 0x7F, 0x41, 0x41}, // [
{0x02, 0x04, 0x08, 0x10, 0x20}, // "\"
{0x41, 0x41, 0x7F, 0x00, 0x00}, // ]
{0x04, 0x02, 0x01, 0x02, 0x04}, // ^
{0x40, 0x40, 0x40, 0x40, 0x40}, // _
{0x00, 0x01, 0x02, 0x04, 0x00}, // `
{0x20, 0x54, 0x54, 0x54, 0x78}, // a
{0x7F, 0x48, 0x44, 0x44, 0x38}, // b
{0x38, 0x44, 0x44, 0x44, 0x20}, // c
{0x38, 0x44, 0x44, 0x48, 0x7F}, // d
{0x38, 0x54, 0x54, 0x54, 0x18}, // e
{0x08, 0x7E, 0x09, 0x01, 0x02}, // f
{0x08, 0x14, 0x54, 0x54, 0x3C}, // g
{0x7F, 0x08, 0x04, 0x04, 0x78}, // h
{0x00, 0x44, 0x7D, 0x40, 0x00}, // i
{0x20, 0x40, 0x44, 0x3D, 0x00}, // j
{0x00, 0x7F, 0x10, 0x28, 0x44}, // k
{0x00, 0x41, 0x7F, 0x40, 0x00}, // l
{0x7C, 0x04, 0x18, 0x04, 0x78}, // m
{0x7C, 0x08, 0x04, 0x04, 0x78}, // n
{0x38, 0x44, 0x44, 0x44, 0x38}, // o
{0x7C, 0x14, 0x14, 0x14, 0x08}, // p
{0x08, 0x14, 0x14, 0x18, 0x7C}, // q
{0x7C, 0x08, 0x04, 0x04, 0x08}, // r
{0x48, 0x54, 0x54, 0x54, 0x20}, // s
{0x04, 0x3F, 0x44, 0x40, 0x20}, // t
{0x3C, 0x40, 0x40, 0x20, 0x7C}, // u
{0x1C, 0x20, 0x40, 0x20, 0x1C}, // v
{0x3C, 0x40, 0x30, 0x40, 0x3C}, // w
{0x44, 0x28, 0x10, 0x28, 0x44}, // x
{0x0C, 0x50, 0x50, 0x50, 0x3C}, // y
{0x44, 0x64, 0x54, 0x4C, 0x44}, // z
{0x1c, 0x3e, 0x6b, 0x14, 0x00}, // RHL alien head
//{0x00, 0x08, 0x36, 0x41, 0x00}, // {
{0x00, 0x00, 0x7F, 0x00, 0x00}, // |
{0x00, 0x41, 0x36, 0x08, 0x00}, // }
{0x08, 0x04, 0x08, 0x10, 0x08}, // ~
{0x1c, 0x3e, 0x6b, 0x14, 0x00} // RHL alien head
//{0x08, 0x1C, 0x2A, 0x08, 0x08} // <-
};

// pulse serial clock high
void scl(int line) {
    digitalWrite(pins[line].clock, LOW);
    digitalWrite(pins[line].clock, HIGH);
    digitalWrite(pins[line].clock, LOW);
}

// turn on row LEDs
void rowenable(int line) {
    digitalWrite(pins[line].rowEnable, LOW);
}

// turn off row LEDs
void rowdisable(int line) {
    digitalWrite(pins[line].rowEnable, HIGH);
}

// clock 1 data bit out on the tx pin
void serbit(int line, int bit) {
    digitalWrite(pins[line].data, (bit ? HIGH : LOW));
    scl(line);
}

// our font data is all indexed by character, so font[c] -> {byte1, byte2, ... byte5}
// where each byte is a column of the bit data for the repr of that char
// (plus an extra ignored bit.)
// But we have to send whole rows of bits at once to the board, so this rotates that
// and gives back a char (of which you should only use the low 5 bits.)
unsigned char rowdots(int row, char c) {
    unsigned char result = 0x00;

    if ((row < 0) || (row > 6)) return 0x55;
    if (c < 0x20 || c > 0xfe) return 0xaa;

    for (int column=0; column<5; column++) {
        unsigned char columnByte = font1[c - 0x20][column];
        unsigned char mybit = 0;
        if (columnByte & (1 << row)) { mybit = 1; }
        result |= (mybit << column);
    }
    
    return result;
}

// send the 80 bits that correspond to a row slice out of
// the character images in msg[], then the 3 row select bits
// for an 83 bit frame, total
void sendrow(int line, int row, char msg[16]) {
    if (row < 0 || row > 6) { return; }

    // send the 80 pixel bits
    for (int i=0; i<16; i++) {
        unsigned char rowbyte = rowdots(row, msg[i]);
        for (int bitpos = 0; bitpos < 5; bitpos++) {
            serbit(line, (rowbyte>>bitpos) & 0x1);
        }
    }

    // send the 3 "row select" bits
    for (int rn=2; rn>=0; rn--) {
        serbit(line, (row+1)>>rn & 0x01);
    }
}

//
// Precalculate all the row-oriented data in packed bytes for
// sending via shiftOut
//
void rendermsg(char msg[32], char msgbits[7][20]) {
  for (int row=0; row < 7; row++) {

    // zero all bytes in output first
    for (int c=0; c < 20; c++) { msgbits[row][c] = 0; }

    int renderedbyte = 0;
    int renderedbit = 0;
    
    for (int srcbyte=0; srcbyte < 32; srcbyte++) {
      unsigned char rowbyte = rowdots(row, msg[srcbyte]);
      for (int srcbit=0; srcbit < 5; srcbit++) {
	// do stuff
	int bvalue = ((rowbyte>>srcbit) & 0x1);
	msgbits[row][renderedbyte] |= (bvalue << renderedbit);
	
	renderedbit++;
	if (renderedbit == 8) {
	  renderedbit = 0;
	  renderedbyte++;
	}
      }
    }
  }
}

//
// only send the row as arg to figure out 3 rowbits to put out, otherwise we are pre-selected
//
void sendrowbits(int line, int row, char msgbits[10]) {
    // send the 80 pixel bits
    for (int i=0; i<10; i++) {
	shiftOut(pins[line].data, pins[line].clock, LSBFIRST, msgbits[i]);
    }

    // send the 3 "row select" bits
    for (int rn=2; rn>=0; rn--) {
        serbit(line, (row+1)>>rn & 0x01);
    }
}

//
// Ok, *really* need to figure out what the interface/handshaking should look like.
// 
// Considering, initial byte gives type of command, rest of the data follows.
//
// 1<data up to newline> 
//  reset message line 1 (same for 2, 3, 4)
//
// b<160 * 7 * 4 bits>
//  send the board all pre-rendered data
//
// e\n
//  erase board
//
// various commands to adjust delays, durations, scrolling, etc.?
//
//
// Right now, all we do is read data until a newline, store it in msg1
//
int getnewmsg(char *msg, int len, char msgbits[7][20]) {
    if (Serial.available() > 0) {
        char newmsg[len];
        for (int i=0; i<len; i++) { newmsg[i] = ' '; }

        // read until we get a newline, just discard anything that would overflow string
        int inx = 0;
        int byteread = ' ';
        while (true) {
            byteread = Serial.read();
            if (byteread < 0) { delay(100); continue; }      // read returns -1 on timeout, delay 100ms then try again
            if (byteread == '\n' || byteread == '\r') break; // don't include newline in message to display

            if (inx < len) {
                newmsg[inx++] = char(byteread);
            }
        }

        Serial.println();
	Serial.print("OK, old message: "); Serial.println(msg);
	for (int i=0; i<len; i++) { msg[i] = newmsg[i]; }

	Serial.print("OK, new message: ");
	Serial.println(msg);

	rendermsg(msg, msgbits);
	return 1;
    }
    return 0;
}


void setup() {
  for (int line=0; line < 2; line++) {
      pinMode(pins[line].rowEnable, OUTPUT);
      rowdisable(line);
      pinMode(pins[line].clock,     OUTPUT);
      pinMode(pins[line].data,      OUTPUT);
      
      digitalWrite(pins[line].clock, LOW);
      digitalWrite(pins[line].data,  LOW);
  }

  Serial.begin(9600); // init serial port at 9600 baud
}

// Just display one line
//
void displaymsg(int line, char msg[32], int duration_ms) {
  // we have to send rows 1 at a time, then dwell for long enough for the image to persist
  int duration_cycles = duration_ms / 7;

  for(int i=0; i<duration_cycles; i++) {
    for (int r=0; r<7; r++) {
      rowdisable(line);
      sendrow(line, r, msg);
      sendrow(line, r, msg+16);
      rowenable(line);
      delay(1);
      rowdisable(line);
    }
  }
}

// The "naiive" version of the display code, with our column oriented font data.
// Should fix to use the row oriented stuff that John checked in.
//
void displaymsgs(char msg0[32], char msg1[32], int duration_ms) {
  // we have to send rows 1 at a time, then dwell for long enough for the image to persist
  int row_dwell_ms = 1;
  int duration_cycles = duration_ms / (7 * row_dwell_ms);

  for(int i=0; i<duration_cycles; i++) {
    for (int r=0; r<7; r++) {
      rowdisable(0);
      rowdisable(1);

      sendrow(0, r, msg0);
      sendrow(0, r, msg0+16);
      sendrow(1, r, msg1);
      sendrow(1, r, msg1+16);

      rowenable(0);
      rowenable(1);

      delay(row_dwell_ms);
      rowdisable(0);
      rowdisable(1);
    }
  }
}

// Use the pre-calculated bits, packed into full bytes, so that we try
// to speed up our output code and reduce flicker.
// 
// This is probably still too slow to do all 4 lines at once w/ arduino,
// investigate SPI stuff? Sharing clock lines?  Not sure what to do.
//
void fastdisplay(char msgbits0[7][20], char msgbits1[7][20], int duration_ms) {
  // we have to send rows 1 at a time, then dwell for long enough for the image to persist
  int row_dwell_ms = 1;
  int duration_cycles = duration_ms / (7 * row_dwell_ms);

  for(int i=0; i<duration_cycles; i++) {
    for (int r=0; r<7; r++) {
      rowdisable(0);
      rowdisable(1);

      sendrowbits(0, r, msgbits0[r]);
      sendrowbits(0, r, msgbits0[r]+10);
      sendrowbits(1, r, msgbits1[r]);
      sendrowbits(1, r, msgbits1[r]+10);

      rowenable(0);
      rowenable(1);

      delay(row_dwell_ms);
      rowdisable(0);
      rowdisable(1);
    }
  }
}

void loop() {
    //               1234567890123467890123456789012
    char msg0[32] = "{ ROGUE HACK LAB               "; // compiler zero terminates
    char msg1[32] = "  Focused Hack Night, Mon 6pm  "; // compiler zero terminates
    //char msg0[32] = "{ ROGUE HACK LAB               "; // compiler zero terminates
    msg0[31] = ' ';
    msg1[31] = ' ';

    // pre-calculate all the bits into full byte sized arrays so we can use
    // shiftOut to make it fast enough for less flicker.  Still need to investigate the SPI 
    // library stuff.
    char msgbits0[7][20];
    char msgbits1[7][20];
    
    rendermsg(msg0, msgbits0);
    rendermsg(msg1, msgbits1);

    while (true) {
        getnewmsg(msg1, 32, msgbits1); // get to read new msg every 1+.3+1+.3 == 2.6 seconds

        Serial.print('.');
	fastdisplay(msgbits0, msgbits1, 1000);
	delay(300); // leave off for 300ms per cycle
	fastdisplay(msgbits1, msgbits0, 1000); // flip messages over each time
	delay(300);
    }

    /*
    while (true) {
        getnewmsg(msg1, 32, msgbits1);
        Serial.print('.');
	displaymsgs(msg0, msg1, 1000);
	delay(300);
	displaymsgs(msg1, msg0, 1000);
	delay(300);
    }
    */
}
