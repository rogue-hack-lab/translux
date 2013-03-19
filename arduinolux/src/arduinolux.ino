#include <avr/pgmspace.h>
#include "msg.h"

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
    
    Serial.begin(9600); // init serial port at 9600 baud
    //Serial.write("arduinolux firmware v1.0b\r\n");
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
            //rendermsgbits(msg, msgbits);
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
    msg_t msg;
    msg_copy_in(&msg, "{ ROGUE HACK LAB               {", 0);
    msg_copy_in(&msg, "  Focused Hack Night, Mon 6pm   ", 1);
    msg_copy_in(&msg, "  Open Hack Night, Wed 6:30pm   ", 2);
    msg_copy_in(&msg, "  www.roguehacklab.com          ", 3);

    // pre-calculate all the bits to send from the message to make our display routine fast
    msg_render(&msg);

    while (true) {
        Serial.print(".\r\n");
	display(msg.bits, 2000);
        //serialcontrol(msg);

        delay(600); // leave off for 300ms per cycle
    }
}
