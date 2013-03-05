/*
 * Use an arduino to drive our translux display (it has 4x32 LED modules, each 5x7 pixels)
 *
 * Note: the pin connections for the 2x5 connector are as follows:
 *
 *            +----+
 *   gnd top  |2  1| clk top
 *   gnd top  |4  3| input top
 * enable top |6  5| enable bottom
 * input btm. |8  7| gnd bottom
 *   clk btm. |10 9| gnd bottom
 *            +----+
 *
 * Note that all the ground pins are tied together on the board. Also note: if you connect
 * the wrong enable pin, the board will display whatever random data happens to be in the 
 * shift registers (only one line, since the row select bits will be random too), and never
 * change no matter what data you send. Not that I would ever spend almost an entire 
 * Focused Hack Night trying to figure out why the LEDs don't change no matter what data 
 * I send. Sheesh.
 *
 * PERMANENT BRAIN MODE:
 * 
 * Goals:
 * - reduce the number of pins required
 * - reduce flicker
 *
 * Plan:
 * Use shared clock and row-enable lines
 *
 *      GND     -      black   (black jumper wire)
 */
#define CLOCKpin  13 //  brown   (blue jumper wire)
#define ENABLEpin 12 //  red     (white jumper wire)
#define INPUT1pin 11 //  orange
#define INPUT2pin 10 //  yellow
#define INPUT3pin  9 //  green
#define INPUT4pin  8 //  blue
/*
 * 10-Pin Ribbon Cable 1
 * 1 - clock top      CLK,    13, brown  (blue jmp)
 * 2 - gnd top        GND      -, black  (black jmp)
 * 3 - input top      INPUT1  11, orange
 * 4 - gnd top        GND      -, black  (black jmp)
 * 5 - enable bottom  ENBL    12, red    (white jmp)
 * 6 - enable top     ENBL    12, red    (white jmp)
 * 7 - gnd bottom     GND      -, black  (black jmp)
 * 8 - input bottom   INPUT2  10, yellow
 * 9 - gnd bottom     GND      -, black  (black jmp)
 * 10- clock bottom   CLK     13, brown  (blue jmp)
 * 
 * 10-Pin Ribbon Cable 2
 * 1 - clock top      CLK     13, brown  (blue)
 * 2 - gnd top        GND      -, black  (black)
 * 3 - input top      INPUT3   9, green
 * 4 - gnd top        GND      -, black  (black)
 * 5 - enable bottom  ENBL    12, red    (white)
 * 6 - enable top     ENBL    12, red    (white)
 * 7 - gnd bottom     GND      -, black  (black)
 * 8 - input bottom   INPUT4   8, blue
 * 9 - gnd bottom     GND      -, black  (black)
 * 10- clock bottom   CLK     13, brown  (blue)
 * 
 */

#define CLOCKbit  5
#define ENABLEbit 4
#define INPUT1bit 3
#define INPUT2bit 2
#define INPUT3bit 1
#define INPUT4bit 0

#define CLOCKmask   (1<<CLOCKbit)
#define ENABLEmask  (1<<ENABLEbit)
#define INPUT1mask  (1<<INPUT1bit)
#define INPUT2mask  (1<<INPUT2bit)
#define INPUT3mask  (1<<INPUT3bit)
#define INPUT4mask  (1<<INPUT4bit)

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

/*
 *   Consider 4 lines of LED modes, where each module has 7 rows and 5 columns:
 * 
 *       col 0 1 2 3 4 5           0 1 2 3 4 5
 *     row 0                     0 * - - * - -      ... etc ...
 *         1                     1
 *         2                     3
 * 
 * Since each line of LED modules is sharing the same enable pin, we have to send out 4 different
 * bits on each clock pulse (1 for each line of modules.)
 *
 * This handily works out as 2 bits per row of leds for each byte of pre-rendered data.
 *
 * So portbytes[2][21] is has data for the 3rd row of leds on each line of modules, where
 *
 * bit 8 - input1 bit 1
 *     7 - input2 bit 1
 *     6 - input3 bit 1
 *     5 - input4 bit 1
 *     4 - input1 bit 2
 *     3 - input2 bit 2
 *     2 - input3 bit 2
 *     1 - input4 bit 2
 */
void renderportbytes(char msg[4][32], byte portbytes[7][80]) 
{
    // complicated bit math. for now, just put in test pattern
    /*
    int linebitpos=0;

    for(int charpos=0; charpos<32; charpos++) {
        unsigned char rowbytes[4];
        for(int i=0; i<4; i++) { rowbytes[0] = rowdots(0, msg[0][charpos]); }
        //...
    }
    */
    for (int i=0; i<80; i++) {
        for (int j=0; j<7; j++) {
            portbytes[j][i] = B00001111;
        }
    }
}

// For this version, we have to do an additional pre-compliation step.
// However, it lets us hold all the data in portbits
// 
void sendportbits(int row, // 0 - 6, which line of LEDs on the modules
                  byte portbits[7][80]) 
{
    byte baseByte = B00010000;

    for (int i=0; i<80; i++) {
        // set clock low, enable off, all data bits 0
        PORTB = baseByte;

        // send the low 4 bits from byte i as our 4 inputs...
        PORTB |= (portbits[row][i] & B00001111);

        // pulse clock high
        bitSet(PORTB, CLOCKbit);

        PORTB = baseByte;

        // send the high 4 bits from byte i as our 4 inputs...
        PORTB |= (portbits[row][i] >> 4);

        // pulse clock high
        bitSet(PORTB, CLOCKbit);

        PORTB = baseByte;
    }

    // send the 3 "row select" bits
    for (int rn=2; rn>=0; rn--) {
        int bit = (row+1)>>rn & 0x01;
        PORTB = baseByte;
        if (bit) {
            PORTB |= input1Bit | input2Bit | input3Bit | input4Bit;
        }
        bitSet(PORTB, clockPinBit);
        PORTB = baseByte;
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
int getnewmsg(char *msg, int len, byte portbytes[7][80]) {
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

	renderportbytes(msg, portbytes);
	return 1;
    }
    return 0;
}


void setup() {
  for (int line=0; line < 2; line++) {
      pinMode(CLOCKpin,  OUTPUT);
      pinMode(ENABLEpin, OUTPUT);
      pinMode(INPUT1pin, OUTPUT);
      pinMode(INPUT2pin, OUTPUT);
      pinMode(INPUT3pin, OUTPUT);
      pinMode(INPUT4pin, OUTPUT);
      
      digitalWrite(CLOCKpin, LOW);
      digitalWrite(ENABLEpin, HIGH);
      digitalWrite(INPUT1pin, LOW);
      digitalWrite(INPUT2pin, LOW);
      digitalWrite(INPUT3pin, LOW);
      digitalWrite(INPUT4pin, LOW);
  }

  Serial.begin(9600); // init serial port at 9600 baud
}


void rowdisable() 
{
    digitalWrite(ENABLEpin, 1);
}

void rowenable() 
{
    digitalWrite(ENABLEpin, 0);
}

void fastdisplay(byte portbytes[7][80], int duration_ms) {
    // we have to send rows 1 at a time, since the data is fed to the LED drivers 
    // directly from the serial-to-parallel chips, then we have to dwell for long 
    // enough for the image to persist

    int row_dwell_ms = 1;
    int duration_cycles = duration_ms / (7 * row_dwell_ms);
    
    for(int i=0; i<duration_cycles; i++) {
        for (int r=0; r<7; r++) {
            rowdisable();
            sendportbytes(r, portbytes);
            rowenable();
            delay(row_dwell_ms);
        }
    }
}

void loop() {
    char msg[4][32] = {"{ ROGUE HACK LAB               ",
                       "  Focused Hack Night, Mon 6pm  ",
                       "                               ",
                       "                               "};
    // compiler NULL terminates the strings, so explicitly set last byte to ' ' char after.
    msg[0][31] = ' '; msg[1][31] = ' '; msg[2][31] = ' '; msg[3][31] = ' ';

    byte portbytes[7][80];

    // pre-calculate all the bits to send from the message to make our display routine fast
    renderportbytes(msg, portbytes);

    while (true) {
        getnewmsg(msg[0], 32, portbytes); // FIXME: use interrupts to get new message?

        Serial.print('.');
	fastdisplay(msg, portbytes, 1000);
	delay(300); // leave off for 300ms per cycle
    }
}
