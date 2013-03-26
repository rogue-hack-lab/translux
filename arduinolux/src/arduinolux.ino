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
 *                     HARNESS   PROTOBOARD-JUMPERS
 *      GND     -        black   (black)
 */
#define CLOCKpin  13 //  brown   (blue)
#define ENABLEpin 12 //  red     (white)
#define DATA1pin  11 //  orange
#define DATA2pin  10 //  yellow
#define DATA3pin   9 //  green
#define DATA4pin   8 //  blue

#define PHOTOpin  A0
/*
 * 10-Pin Ribbon Cable 1
 * 1 - clock top      CLK,    13, brown  (blue jmp)
 * 2 - gnd top        GND      -, black  (black jmp)
 * 3 - input top      DATA1   11, orange
 * 4 - gnd top        GND      -, black  (black jmp)
 * 5 - enable bottom  ENBL    12, red    (white jmp)
 * 6 - enable top     ENBL    12, red    (white jmp)
 * 7 - gnd bottom     GND      -, black  (black jmp)
 * 8 - input bottom   DATA2   10, yellow
 * 9 - gnd bottom     GND      -, black  (black jmp)
 * 10- clock bottom   CLK     13, brown  (blue jmp)
 * 
 * 10-Pin Ribbon Cable 2
 * 1 - clock top      CLK     13, brown  (blue)
 * 2 - gnd top        GND      -, black  (black)
 * 3 - input top      DATA3    9, green
 * 4 - gnd top        GND      -, black  (black)
 * 5 - enable bottom  ENBL    12, red    (white)
 * 6 - enable top     ENBL    12, red    (white)
 * 7 - gnd bottom     GND      -, black  (black)
 * 8 - input bottom   DATA4    8, blue
 * 9 - gnd bottom     GND      -, black  (black)
 * 10- clock bottom   CLK     13, brown  (blue)
 * 
 */

// The IO mapped register ports and pins differ for Uno/atmega328p and Leonardo
// 0 == Uno
// 1 == Boarduino (atmega328p)
// 2 == Leonardo

#define BOARD 2
//#define BOARD 1

#if (BOARD == 2)
#define CLOCKPORT  PORTC
#define ENABLEPORT PORTD
#define DATAPORT   PORTB
#define CLOCKbit   7 // PC7
#define ENABLEbit  6 // PD6
#define DATA1bit   7 // PB7
#define DATA2bit   6 // PB6
#define DATA3bit   5 // PB5
#define DATA4bit   4 // PB4

#else

#define CLOCKPORT  PORTB
#define ENABLEPORT PORTB
#define DATAPORT   PORTB
#define CLOCKbit  5
#define ENABLEbit 4
#define DATA1bit  3
#define DATA2bit  2
#define DATA3bit  1
#define DATA4bit  0
#endif

#define CLOCKmask   (1<<CLOCKbit)
#define ENABLEmask  (1<<ENABLEbit)
#define DATA1mask   (1<<DATA1bit)
#define DATA2mask   (1<<DATA2bit)
#define DATA3mask   (1<<DATA3bit)
#define DATA4mask   (1<<DATA4bit)

int DATAPINS[4] = {DATA1pin, DATA2pin, DATA3pin, DATA4pin};
int DATABITS[4] = {DATA1bit, DATA2bit, DATA3bit, DATA4bit};

int dataPinToBit[13] = {0, 0, 0, 0, 0, 0, 0, 0, // pins 0 - 7 don't map to anything
     DATA4bit, DATA3bit, DATA2bit, DATA1bit};

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


void setup() {
    pinMode(CLOCKpin,  OUTPUT);
    pinMode(ENABLEpin, OUTPUT);
    for (int i=0; i<4; i++) { 
        pinMode(DATAPINS[i], OUTPUT);
    }
    
    digitalWrite(CLOCKpin, LOW);
    digitalWrite(ENABLEpin, HIGH);
    for (int i=0; i<4; i++) {
        digitalWrite(DATAPINS[i], LOW);
    }

    pinMode(PHOTOpin, INPUT);

    Serial.begin(9600); // init serial port at 9600 baud
    //Serial.write("arduinolux firmware v1.0b\r\n");
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

void rendermsgbits(char msg[4][32], byte msgbits[7][20][4]) {
    for (int row=0; row < 7; row++) {
        
        // zero all bytes in output first
        for (int cpos=0; cpos<20; cpos++) {
            for (int line=0; line < 4; line++) { msgbits[row][cpos][line] = 0; }
        }
        
        int renderedbyte = 0;
        int renderedbit = 0;
        
        for (int srcbyte=0; srcbyte < 32; srcbyte++) {
            unsigned char rowbytes[4];
            rowbytes[0] = rowdots(row, msg[0][srcbyte]);
            rowbytes[1] = rowdots(row, msg[1][srcbyte]);
            rowbytes[2] = rowdots(row, msg[2][srcbyte]);
            rowbytes[3] = rowdots(row, msg[3][srcbyte]);
            
            for (int srcbit=0; srcbit < 5; srcbit++) {
                // do stuff
                for (int i=0; i<4; i++) {
                    int bvalue = ((rowbytes[i]>>srcbit) & 0x1);
                    msgbits[row][renderedbyte][i] |= (bvalue << renderedbit);
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

//#define DIRECTIO 0 // NOT using directio
#define DIRECTIO 1 // USING directio

inline void clockLow() {
#if DIRECTIO
    bitClear(CLOCKPORT, CLOCKbit);
#else
    digitalWrite(CLOCKpin, LOW);
#endif
}

inline void clockHigh() {
#if DIRECTIO
    bitSet(CLOCKPORT, CLOCKbit);
#else
    digitalWrite(CLOCKpin, HIGH);
#endif
}

inline void setData(int dataPin, bool value) {
#if DIRECTIO
    if (value) 
        bitSet(DATAPORT, dataPinToBit[dataPin]);
    else 
        bitClear(DATAPORT, dataPinToBit[dataPin]);
#else
    digitalWrite(dataPin, value);
#endif
}

void rowdisable() {
#if DIRECTIO
    bitSet(ENABLEPORT, ENABLEbit);
#else
    digitalWrite(ENABLEpin, 1);
#endif
}

void rowenable() {
#if DIRECTIO
    bitClear(ENABLEPORT, ENABLEbit);
#else
    digitalWrite(ENABLEpin, 0);
#endif
}

void allDataLow() {
#if DIRECTIO
    DATAPORT ^= DATA1mask | DATA2mask | DATA3mask | DATA4mask;
#else
    for (int i=0; i<4; i++) {
        digitalWrite(DATAPINS[i], LOW);
    }
#endif
}

void shiftOutMultiple(byte b[4]) {
    for (int i=0; i<8; i++) {
        clockLow();
        setData(DATA1pin, b[0] & (1<<i));
        setData(DATA2pin, b[1] & (1<<i));
        setData(DATA3pin, b[2] & (1<<i));
        setData(DATA4pin, b[3] & (1<<i));
        clockHigh();
    }
    allDataLow();
}

void sendrowbits(int row) {
    for (int rn=2; rn>=0; rn--) {
        bool bit = ((row+1)>>rn) & 1;
        clockLow();
        for (int i=0; i<4; i++) { digitalWrite(DATAPINS[i], bit); }
        clockHigh();
        clockLow();
        //allDataLow();
    }
}

void sendmsgbits(int row, byte msgbits[7][20][4]) {
    for (int frame=0; frame<2; frame++) {
        for (int i=0; i<10; i++) {
            int charpos = (frame*10)+i;
            shiftOutMultiple(msgbits[row][charpos]);
        }
        sendrowbits(row);
    }
}

void display(byte msgbits[7][20][4], int duration_ms) {
    // we have to send rows 1 at a time, since the data is fed to the LED drivers 
    // directly from the serial-to-parallel chips, then we have to dwell for long 
    // enough for the image to persist

    int row_dwell_ms = 1;
    int duration_cycles = duration_ms / (7 * row_dwell_ms);
    
    for(int i=0; i<duration_cycles; i++) {
        for (int r=0; r<7; r++) {
            rowdisable();
            sendmsgbits(r, msgbits);
            rowenable();
            delay(row_dwell_ms);
            if (Serial.available()) break;
        }
        rowdisable();
        if (Serial.available()) break;
    }
}

void readtonewline() {
    while (Serial.available()) {
        int tmp = Serial.read();
        if (tmp == '\n') break;
    }
}

void flushtonewline() {
    while (Serial.available()) {
        int b = Serial.read();
        if (char(b) == '\n') break;
    }
}

void flushserial() {
    if (!Serial.available()) return;
    Serial.print("flushing serial buffer:\r\n");
    while (Serial.available()) {
        int b = Serial.read();
        Serial.print(b);
    }
    Serial.print("\r\n");
}

#define HELP_CMD       '?'
#define READ_CMD       'r'
#define SETLINE_CMD    's'
#define SETTMPMSG_CMD  'm'
#define SETPIXELS_CMD  'p'
#define SETFONT_CMD    'f'

const char help_msg[] = "arduinolux serial interface v0.9, commands are:\r\n"
    "? (help), r (read current msg), s (set msg for given line), m (set temp msg for whole sign), p (set pixel data directly), f (set font data)\r\n";

// returns true on error (I know, backwards)
bool readmsglinedata(char msgline[32]) {
    int i = 0;
    for (i=0; i<32; i++) { msgline[i] = ' '; }

    i = 0;
    while (true) {
        int b = Serial.read();
        if (b < 0) { delay(10); continue; }
        if (b == '\r') { break; }
        if (b == '\n') { break; }
        Serial.print((char)b);
        msgline[i] = (char)(b);
        i++;

        if (i >= 32) break;
    }
    return false;
}

int serialcontrol(char msg[4][32], byte msgbits[7][20][4]) {
    if (!Serial.available()) return 0;

    bool error = true;
    int linechar = 0;
    int lineinx = 0;
    int i;

    int cmd = Serial.read(); // read the command byte

    //Serial.write((char)(cmd));
    //Serial.write("\n\r");

    switch(cmd) {
    case HELP_CMD:
        Serial.write(help_msg);
        error = false;
        break;
    case READ_CMD:
        for (i=0; i<4; i++) {
            Serial.write((const uint8_t *)(msg[i]), 32);
            Serial.write("\r\n");
        }
        error = false;
        break;
    case SETLINE_CMD: 
        Serial.write("type line number (1 - 4):\r\n");
    getline:
        linechar = Serial.read(); // read the line number
        if (linechar == -1) goto getline;
        lineinx = linechar - '1';
        //flushtonewline();
        if (lineinx < 0 || lineinx > 3) {
            flushtonewline();
            Serial.write("invalid line number\r\n");
            error = true;
        } else {
            Serial.write("type line data (will truncate to 32 chars):\r\n");
            error = readmsglinedata(msg[lineinx]);
            rendermsgbits(msg, msgbits);
            //Serial.write("done with rendermsgbits\r\n");
            error = false;
        }
        break;
    case SETTMPMSG_CMD:
    case SETPIXELS_CMD:
    case SETFONT_CMD:
        Serial.write("unimplemented command\r\n");
        error = true;
        break;
    default:
        Serial.write("unkown command\r\n");
        error = true;
        break;
    }

    if (error) {
        Serial.write("ERROR\r\n");
    } else {
        Serial.write("OK\r\n");
    }

    flushserial();
    return error;
}


void loop() {
    char msg[4][32] = {"{ ROGUE HACK LAB              {",
                       "  Focused Hack Night, Mon 6pm  ",
                       "  Open Hack Night, Wed 6:30pm  ",
                       "  www.roguehacklab.com         "};
    // compiler NULL terminates the strings, so explicitly set last byte to ' ' char after.
    msg[0][31] = ' '; msg[1][31] = ' '; msg[2][31] = ' '; msg[3][31] = ' ';

    byte msgbits[7][20][4];

    // pre-calculate all the bits to send from the message to make our display routine fast
    rendermsgbits(msg, msgbits);

    // photoresistor seems pretty sensitive. reading 64 @ 7:12pm on March 21, sun is down at home, not sure about Medford
    const int photoLimit = 15;

    while (true) {
        int photoValue = analogRead(PHOTOpin);
        Serial.print("photoResistor reading: "); Serial.print(photoValue);
        if (photoValue > photoLimit) {
            Serial.print("/1024 ON\r\n");
            display(msgbits, 2000);
        } else {
            Serial.print("/1024 OFF\r\n");
            display(msgbits, 10);
        }
        serialcontrol(msg, msgbits);

        delay(600); // leave off for 300ms per cycle
    }
}



#if 0
// =========================================================================
// -----------  EXPERIMENTAL * NOT WORKING YET  ----------------------------
// =========================================================================

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
/*
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
*/

#endif

