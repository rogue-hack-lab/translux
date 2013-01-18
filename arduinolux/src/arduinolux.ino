/*
 * Use an arduino to drive our translux display (it has 4x32 LED modules, each 5x7 pixels)
 */
int txPin        = 11;
int clockPin     = 12;
int rowEnablePin = 13;

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
void scl() {
    digitalWrite(clockPin, LOW);
    digitalWrite(clockPin, HIGH);
    digitalWrite(clockPin, LOW);
}

// turn on row LEDs
void rowenable() {
    digitalWrite(rowEnablePin, LOW);
}

// turn off row LEDs
void rowdisable() {
    digitalWrite(rowEnablePin, HIGH);
}

// clock 1 data bit out on the tx pin
void serbit(int bit) {
    digitalWrite(txPin, (bit ? HIGH : LOW));
    scl();
}

// our font data is all indexed by character, so font[c] -> {byte1, byte2, ... byte5}
// where each byte is a column of the bit data for the repr of that char
// (plus an extra ignored bit.)
// But we have to send whole rows of bits at once to the board, so this rotates that
// and gives back a char (of which you should only use the low 5 bits.)
unsigned char rowdots(int row, char c) {
    unsigned char result = 0x00;
    
    //Serial.write(c);

    if ((row < 0) || (row > 6)) return 0x55;
    if (c < 0x20 || c > 0xfe) {
        //Serial.write("found char: ");
        //Serial.write(c); Serial.write("\n");
        return 0xaa;
    }

    for (int column=0; column<5; column++) {
        unsigned char columnByte = font1[c - 0x20][column];
        unsigned char mybit = 0;
        if (columnByte & (1 << row)) { mybit = 1; }
        result |= (mybit << column);
    }
    
    return result;
}

// send the 80 bits that correspond to a row slice out of
// the character images in msg[]
void sendrow(int row, char msg[16]) {
    if (row < 0 || row > 6) { return; }
    for (int i=0; i<16; i++) {
        unsigned char rowbyte = rowdots(row, msg[i]);
        //rowbyte = 16;
        //rowbyte = 0xaa;
        //Serial.write("rowbyte %d for char %c\n", rowbyte, msg[i]);
        for (int bitpos = 0; bitpos < 5; bitpos++) {
            serbit((rowbyte>>bitpos) & 0x1);// & (1<<bitpos));
            //serbit(bitpos % 3 == 0);
        }
    }
    for (int rn=2; rn>=0; rn--) {
        serbit((row+1)>>rn & 0x01);
    }
}


// Accept new data for display from the serial port
int get_new_msg(char msg[16]) {
    if (Serial.available() > 0) {
        char newmsg[16];
        for (int i=0; i<16; i++) { newmsg[i] = ' '; }

        byte count = Serial.readBytesUntil('\n', newmsg, 16);

        Serial.println();
	Serial.print("OK, old message: "); Serial.println(msg);
	for (int i=0; i<16; i++) { msg[i] = newmsg[i]; }

	Serial.print("OK, new message: "); // why do serial writes mess up our data?
	Serial.println(msg);
	return 1;
    }
    return 0;
}


void setup() {
  Serial.begin(9600); // init serial port at 9600 baud
  
  pinMode(txPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(rowEnablePin, OUTPUT);

  rowdisable();
  digitalWrite(txPin, LOW);
  digitalWrite(clockPin, LOW);
}

void displaymsg(char msg[16], int duration_ms) {
    rowdisable();
    for (int r=0; r<7; r++) {
	sendrow(r, msg);
    }
    rowenable();
    delay(duration_ms);
    rowdisable();
}

void loop() {
    int len = 16;
    //                   123456789012346
    char localmsg[16] = "{ ROGUEHACKLAB "; // compiler zero terminates
    localmsg[15] = ' ';

    while (true) {
        Serial.print('.');
	get_new_msg(localmsg);

	// this should give us 100*7*1ms ~= 700ms of same data before we have
	// an opportunity to load a new message
	displaymsg(localmsg, 3000); // try to display for 3 seconds
    }
}
