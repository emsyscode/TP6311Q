/****************************************************/
/* This is only one example of code structure       */
/* OFFCOURSE this code can be optimized, but        */
/* the idea is let it so simple to be easy catch    */
/* where we can do changes and look to the results  */
/****************************************************/

/*
Copyright (c) "emsys.pt"
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Code lines"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NON INFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#define VFD_data 8// If 0 write LCD, if 1 read of LCD
#define VFD_clk 9 // if 0 is a command, if 1 is a data0
#define VFD_ce 10 // Must be pulsed to LCD fetch data of bus
//
#define BUTTON_PIN 2 //Att check wich pins accept interrupts... Uno is 2 & 3
volatile byte buttonReleased = false;

bool flagSet = false;
bool flagReached = false;
unsigned int nFunction = 8;  //This value "8" force go to default position of switch case nFunction at first running.
uint8_t numberOfGrids = 0b00001011; //This define to 12 grids and 16 segments on TP6311Q
uint8_t numberOfSegments = 0b00010000; //Thise it to be used as a number definition of segments used
uint8_t numberOfDigits = 0x21; //This is a number used to define the number of digits on VFD.
//    Numbers design      
    unsigned char number0[]={(0b00110001),(0b00111000),(0b00000000)}; // 0
    unsigned char number1[]={(0b00100000),(0b00010000),(0b00000000)}; // 1
    unsigned char number2[]={(0b11010001),(0b00110000),(0b00000000)}; // 2
    unsigned char number3[]={(0b11100001),(0b00110000),(0b00000000)}; // 3
    unsigned char number4[]={(0b11100000),(0b00011000),(0b00000000)}; // 4
    unsigned char number5[]={(0b11100001),(0b00101000),(0b00000000)}; // 5
    unsigned char number6[]={(0b11110001),(0b00101000),(0b00000000)}; // 6
    unsigned char number7[]={(0b00100000),(0b00110000),(0b00000000)}; // 7
    unsigned char number8[]={(0b11110001),(0b00111000),(0b00000000)}; // 8
    unsigned char number9[]={(0b11100000),(0b00111000),(0b00000000)}; // 9
    //    Letters design         //Please note I only have created the message chars: "HI FOLKS"
    unsigned char letterA[]={(0b11100011),(0b00100011),(0b00000000)}; // A
    unsigned char letterB[]={(0b10101001),(0b01101001),(0b00000000)}; // B
    unsigned char letterC[]={(0b00000011),(0b01000010),(0b00000000)}; // C
    unsigned char letterD[]={(0b00101001),(0b01101000),(0b00000000)}; // D
    unsigned char letterE[]={(0b11000011),(0b01000011),(0b00000000)}; // E
    unsigned char letterF[]={(0b11010000),(0b00101000),(0b00000000)}; // F
    unsigned char letterG[]={(0b00000011),(0b01100011),(0b00000000)}; // G
    unsigned char letterH[]={(0b11110000),(0b00011000),(0b00000000)}; // H
    unsigned char letterI[]={(0b00000101),(0b00100010),(0b00000000)}; // I
    unsigned char letterJ[]={(0b00100001),(0b01100010),(0b00000000)}; // J
    unsigned char letterK[]={(0b01010010),(0b00001001),(0b00000000)}; // k
    unsigned char letterL[]={(0b00010001),(0b00001000),(0b00000000)}; // L
    unsigned char letterM[]={(0b00110110),(0b00100010),(0b00000000)}; // M
    unsigned char letterN[]={(0b00100110),(0b00110010),(0b00000000)}; // N
    unsigned char letterO[]={(0b00110001),(0b00111000),(0b00000000)}; // O
    unsigned char letterP[]={(0b11100011),(0b00000011),(0b00000000)}; // P
    unsigned char letterQ[]={(0b00100011),(0b01110010),(0b00000000)}; // Q
    unsigned char letterR[]={(0b10101001),(0b00011001),(0b00000000)}; // R
    unsigned char letterS[]={(0b11100001),(0b00101000),(0b00000000)}; // S
    unsigned char letterT[]={(0b00001001),(0b00001000),(0b00000000)}; // T
    unsigned char letterU[]={(0b00100010),(0b01100010),(0b00000000)}; // U
    unsigned char letterV[]={(0b00010010),(0b00000110),(0b00000000)}; // V
    unsigned char letterX[]={(0b00010100),(0b00010100),(0b00000000)}; // X
    unsigned char letterZ[]={(0b10010001),(0b01000100),(0b00000000)}; // Z
    unsigned char letterW[]={(0b00100010),(0b00110110),(0b00000000)}; // W
    unsigned char letterY[]={(0b10010100),(0b00001000),(0b00000000)}; // Y
    unsigned char letterSpace[]={(0b00000000),(0b00000000),(0b00000000)}; // Space
    unsigned char letterAllOn[]={(0b11111111),(0b11111111),(0b00000011)}; // All 17 segments ON

//
void pt6311_init(void){
  delayMicroseconds(200); //power_up delay
  // Note: Allways the first byte in the input data after the STB go to LOW is interpret as command!!!

  // Configure VFD display (grids)
  cmd_with_stb(numberOfGrids);//  (0b01000000)    cmd1 12 Grids and 16 segments??? (IC da TOPRO ref: TP6311Q)
  delayMicroseconds(1);
  cmd_with_stb((0b10001000) | 7); // IMPORTANT:  After define the size GRIDs is necessary do a cmd "ON" display.
  delayMicroseconds(1);
  // turn vfd on, stop key scannig
   cmd_with_stb(0b10001000);//(BIN(01100110)); 
  delayMicroseconds(1);
  // Write to memory display, increment address, normal operation
  cmd_with_stb(0b01000000);//(BIN(01000000));
  delayMicroseconds(1);
  // Address 00H - 15H ( total of 11*2Bytes=176 Bits)
  cmd_with_stb(0b11000000);//(BIN(01100110)); 
  delayMicroseconds(1);
  // set DIMM/PWM to value
  cmd_with_stb((0b10001000) | 7);//0 min - 7 max  )(0b01010000)
  delayMicroseconds(1);
}
void send_char(unsigned char grid, unsigned char byte0, unsigned char byte1,unsigned char byte2){
        cmd_with_stb(numberOfGrids); //       cmd 1 // 10 Grids & 17 Segments
        delayMicroseconds(2);
        cmd_with_stb((0b10001000) | 7); // IMPORTANT:  After define the size GRIDs is necessary do a cmd "ON" display.
        delayMicroseconds(2);
        cmd_with_stb(0b01000000); //       cmd 2 //Normal operation; Auto increment
        digitalWrite(VFD_ce, LOW);
        delayMicroseconds(2);
            cmd_without_stb((0b11000000) | grid); // cmd 3 //wich define the start address (00H to 15H)
            cmd_without_stb(byte0); // Data to fill table of 5 grids * 16 segm = 80 bits on the table
            cmd_without_stb(byte1); // Data to fill table of 5 grids * 16 segm = 80 bits on the table
            cmd_4bitsWithout_stb(byte2); // only half byte of third byte.
            //
            digitalWrite(VFD_ce, HIGH);
            delayMicroseconds(2);
}
void cmd_4bitsWithout_stb(unsigned char a){
  // send without stb
  unsigned char data = 0x00; //value to transmit
  unsigned char mask = 1; //our bitmask
  
  data=a;
  //This don't send the strobe signal, to be used in burst data send
   for (mask = 0b00000001; mask>0; mask <<= 1) { //iterate through bit mask
     digitalWrite(VFD_clk, LOW);
     if (data & mask){ // if bitwise AND resolves to true
        digitalWrite(VFD_data, HIGH);
     }
     else{ //if bitwise and resolves to false
       digitalWrite(VFD_data, LOW);
     }
    delayMicroseconds(2);
    digitalWrite(VFD_clk, HIGH);
    delayMicroseconds(2);
   }
}
void cmd_without_stb(unsigned char a){
  // send without stb
  unsigned char data = 0x00; //value to transmit
  unsigned char mask = 1; //our bitmask
  
  data=a;
  //This don't send the strobe signal, to be used in burst data send
   for (mask = 0b00000001; mask>0; mask <<= 1) { //iterate through bit mask
     digitalWrite(VFD_clk, LOW);
     if (data & mask){ // if bitwise AND resolves to true
        digitalWrite(VFD_data, HIGH);
     }
     else{ //if bitwise and resolves to false
       digitalWrite(VFD_data, LOW);
     }
    delayMicroseconds(2);
    digitalWrite(VFD_clk, HIGH);
    delayMicroseconds(2);
   }
}
void cmd_4bitsWith_stb(unsigned char a){
  // send with stb
  unsigned char data = 0x00; //value to transmit
  unsigned char mask = 1; //our bitmask
  
  data=a; 
  //This send the strobe signal
  //Note: The first byte input at in after the STB go LOW is interpreted as a command!!!
  digitalWrite(VFD_ce, LOW);
  delayMicroseconds(1);
   for (mask = 0b00000001; mask>0; mask <<= 1) { //iterate through bit mask
     digitalWrite(VFD_clk, LOW);
     delayMicroseconds(1);
     if (data & mask){ // if bitwise AND resolves to true
        digitalWrite(VFD_data, HIGH);
     }
     else{ //if bitwise and resolves to false
       digitalWrite(VFD_data, LOW);
     }
    digitalWrite(VFD_clk, HIGH);
    delayMicroseconds(1);
   }
   digitalWrite(VFD_ce, HIGH);
   delayMicroseconds(1);
}
void cmd_with_stb(unsigned char a){
  // send with stb
  unsigned char data = 0x00; //value to transmit
  unsigned char mask = 1; //our bitmask
  
  data=a; 
  //This send the strobe signal
  //Note: The first byte input at in after the STB go LOW is interpreted as a command!!!
  digitalWrite(VFD_ce, LOW);
  delayMicroseconds(1);
   for (mask = 0b00000001; mask>0; mask <<= 1) { //iterate through bit mask
     digitalWrite(VFD_clk, LOW);
     delayMicroseconds(1);
     if (data & mask){ // if bitwise AND resolves to true
        digitalWrite(VFD_data, HIGH);
     }
     else{ //if bitwise and resolves to false
       digitalWrite(VFD_data, LOW);
     }
    digitalWrite(VFD_clk, HIGH);
    delayMicroseconds(1);
   }
   digitalWrite(VFD_ce, HIGH);
   delayMicroseconds(1);
}
void test_VFD(void){
      clear_VFD();
      
      digitalWrite(VFD_ce, LOW);
      delayMicroseconds(1);
      cmd_with_stb(numberOfGrids); // cmd 1 // 8 Grids & 20 Segments
      cmd_with_stb((0b10001000) | 7); // IMPORTANT:  After define the size GRIDs is necessary do a cmd "ON" display.
      cmd_with_stb(0b01000000); // cmd 2 //Write VFD, Normal operation; Set pulse as 1/16, Auto increment
      cmd_with_stb(0b10001000 | 0x07); // cmd 2 //set on, dimmer to max
        digitalWrite(VFD_ce, LOW);
        delayMicroseconds(1);
        cmd_without_stb((0b11000000)); //cmd 3 wich define the start address (00H to 15H)
        // Only here must change the bit to test, first 2 bytes and 1/2 byte of third.
         for (int i = 0; i < 8 ; i++){ // test base to 16 segm and 5 grids
          // Zone of test, if write 1 on any position of 3 bytes below position, will bright segment corresponding it.
         cmd_without_stb(0b00000000); // Data to fill table 5*16 = 80 bits
         cmd_without_stb(0b00000000); // Data to fill table 5*16 = 80 bits
         cmd_4bitsWithout_stb(0b00000000); // Data to fill table 5*16 = 80 bits
         }
      digitalWrite(VFD_ce, HIGH);
      delay(1);
      delay(3000);  
}
void setup() {
    // initialize digital pin LED_BUILTIN as an output.
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.begin(115200);
    cli();
    pinMode(VFD_clk, OUTPUT);
    pinMode(VFD_data, OUTPUT);
    pinMode(VFD_ce, OUTPUT);

    pinMode(BUTTON_PIN, INPUT_PULLUP);

    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN),
                  buttonReleasedInterrupt,
                  FALLING);
    pt6311_init();

    test_VFD();

    clear_VFD();

    //only here I active the enable of interrupts to allow run the test of VFD
    //interrupts();             // enable all interrupts
    sei();
}
void clear_VFD(void){
  int sum=0;
  /*
  Here I clean all registers 
  Could be done only on the number of grid
  to be more fast. The 12 * 3 bytes = 36 registers
  */
      for (int n=0; n < 16; n++){  //
        cmd_with_stb(numberOfGrids); //       cmd 1 // 8 Grids & 20 Segments
        delayMicroseconds(1);
        cmd_with_stb((0b10001000) | 7); // IMPORTANT:  After define the size GRIDs is necessary do a cmd "ON" display.
        delayMicroseconds(1);
        cmd_with_stb(0b01000000); //       cmd 2 //Normal operation; Set pulse as 1/16
        digitalWrite(VFD_ce, LOW);
        delayMicroseconds(1);
            cmd_without_stb((0b11000000) | sum); // cmd 3 //wich define the start address (00H to 15H)
            cmd_without_stb(0b00000000); // Data to fill table of 5 grids * 16 segm = 80 bits on the table
            cmd_without_stb(0b00000000); // Data to fill table of 5 grids * 16 segm = 80 bits on the table
            cmd_4bitsWithout_stb(0b00000000); // only half byte of third byte.
            //
            digitalWrite(VFD_ce, HIGH);
            delayMicroseconds(2);
            sum=(sum +3);
     }
}
void led_on(void){
  byte myByte;
    digitalWrite(VFD_ce, LOW);
    delayMicroseconds(20);
    cmd_without_stb(0b01000001); //Command to write a LED's pins!
    delayMicroseconds(2);
    //For example, Bit 1 (as designated by b0) has the value of “0”, then this means that LED1 is ON. 
    //It must be noted that when power is turned ON, bit 5 to bit 1 (b4 to b0) are given the value of “1”. Please refer to the 
    //diagrams below: 
    myByte =(0b00011101);  // Here is only to invert bit of led 2 connected to the pin 18 of HT48R05A.
    cmd_without_stb(myByte);
    delayMicroseconds(2);
    digitalWrite(VFD_ce, HIGH);
    delayMicroseconds(2);
}
void led_off(void){
  byte myByte;
    digitalWrite(VFD_ce, LOW);
    delayMicroseconds(20);
    cmd_without_stb(0b01000001);
    delayMicroseconds(2);
    myByte =(0b00011111);  // Here is only to invert bit of 5 LED's.
    cmd_without_stb(myByte);
    delayMicroseconds(2);
    digitalWrite(VFD_ce, HIGH);
    delayMicroseconds(2);
}
void all_ON(void){
  int sum=0;
  /*
  Here I test all registers 
  Could be done only on the number of grid
  to be more fast. The 12 * 3 bytes = 36 registers
  */
      for (int n=0; n < 16; n++){  //
        cmd_with_stb(numberOfGrids); //       cmd 1 // 14 Grids & 14 Segments
        delayMicroseconds(1);
        cmd_with_stb((0b10001010) | 7); // IMPORTANT:  After define the size GRIDs is necessary do a cmd "ON" display.
        delayMicroseconds(1);
        cmd_with_stb(0b01000000); //       cmd 2 //Normal operation; Set pulse as 1/16
        digitalWrite(VFD_ce, LOW);
        delayMicroseconds(1);
            cmd_without_stb((0b11000000) | sum); // cmd 3 //wich define the start address (00H to 15H)
            cmd_without_stb(0b11111111); // Data to fill table of 5 grids * 16 segm = 80 bits on the table
            cmd_without_stb(0b11111111); // Data to fill table of 5 grids * 16 segm = 80 bits on the table
            cmd_4bitsWithout_stb(0b11111111); // only half byte of third byte.
            //
            digitalWrite(VFD_ce, HIGH);
            delayMicroseconds(3000);
            Serial.println(((0b11000000) | sum), BIN);
            sum=(sum +3);
     }
}
void gridTestSegments(uint8_t grid){
  int sum= grid; //nee receive 0x00 to test grid 0, 0x03 to test grid 1, 0x06 to test grid 2 and so on...
  /*
  Here I test all registers 
  Could be done only on the number of grid
  to be more fast. The 12 * 3 bytes = 36 registers
  */
      //for (int n=0; n < 16; n++){  //
        cmd_with_stb(numberOfGrids); //       cmd 1 // 14 Grids & 14 Segments
        delayMicroseconds(1);
        cmd_with_stb((0b10001010) | 7); // IMPORTANT:  After define the size GRIDs is necessary do a cmd "ON" display.
        delayMicroseconds(1);
        cmd_with_stb(0b01000000); //       cmd 2 //Normal operation; Set pulse as 1/16
        digitalWrite(VFD_ce, LOW);
        delayMicroseconds(1);
            cmd_without_stb((0b11000000) | sum); // cmd 3 //wich define the start address (00H to 15H)
            cmd_without_stb(0b11111111); // Data to fill table of 5 grids * 16 segm = 80 bits on the table
            cmd_without_stb(0b11111111); // Data to fill table of 5 grids * 16 segm = 80 bits on the table
            cmd_4bitsWithout_stb(0b00001111); // only half byte of third byte.
            //
            digitalWrite(VFD_ce, HIGH);
            delayMicroseconds(3000);
            Serial.println(((0b11000000) | sum), BIN);
            sum=(sum +3);
     //}
}
void segBySeg(void){
  int u=0;
  int sum=0;
  unsigned char word1, word2,word3,word4;
  unsigned int mask0=0b0000000000000001;
  unsigned int mask1=0b0000000000000001;
 
      for ( u =0; u < 32; u++){
        cmd_with_stb(numberOfGrids); //       cmd 1 // 8 Grids & 20 Segments
        delayMicroseconds(1);
        cmd_with_stb((0b10001000) | 7); // IMPORTANT:  After define the size GRIDs is necessary do a cmd "ON" display.
        delayMicroseconds(1);
        cmd_with_stb(0b01000000); //       cmd 2 //Normal operation; Set pulse as 1/16
        digitalWrite(VFD_ce, LOW);
        delayMicroseconds(1);
        cmd_without_stb((0b11000000) | sum); // cmd 3 //wich define the start address (00H to 15H)
    //         
        if (u<=15){
       word4 = (mask0 & 0xff000000) >>  24; // remaning segments 
       word3 = (mask0 & 0x00ff0000) >>  16; // remaning segments
       word2 = (mask0 & 0x0000ff00) >>  8; // grid 9 & 10 and segments
       word1 = (mask0 & 0x000000ff) >>  0; // grids
        mask0=(mask0 <<= 1);
        }
        if (u>15){
       word4 = (mask1 & 0xff000000) >>  24; // remaning segments 
       word3 = (mask1 & 0x00ff0000) >>  16; // remaning segments
       word2 = (mask1 & 0x0000ff00) >>  8; // grid 9 & 10 and segments
       word1 = (mask1 & 0x000000ff) >>  0; // grids
        mask1=(mask1 <<= 1);
        }
       //
       //Serial.println(mask0, BIN);
       //Serial.println(mask1, BIN);
            cmd_without_stb(word1); // Data to fill table of 5 grids * 16 segm = 80 bits on the table
            cmd_without_stb(word2); // Data to fill table of 5 grids * 16 segm = 80 bits on the table
            cmd_4bitsWithout_stb(word3); // only half byte of third byte.
            //
            digitalWrite(VFD_ce, HIGH);
            delay(500);
            //Serial.println(((0b11000000) | sum), BIN);
            sum=(sum +3);
    }
}
void msgHiFolks(){ 
    //Please pay attention to the value of memory, the 0x1E is address of digit at left!
    send_char(0x21,letterSpace[0],letterSpace[1],letterSpace[2]); //Space
    send_char(0x1E,letterH[0],letterH[1],letterH[2]); //H
    send_char(0x1B,letterI[0],letterI[1],letterI[2]); //I
    send_char(0x18,letterSpace[0],letterSpace[1],letterSpace[2]); //Space
    send_char(0x15,letterF[0],letterF[1],letterF[2]); //F
    send_char(0x12,letterO[0],letterO[1],letterO[2]); //O
    send_char(0x0F,letterL[0],letterL[1],letterL[2]); //L
    send_char(0x0C,letterK[0],letterK[1],letterK[2]); //K
    send_char(0x09,letterS[0],letterS[1],letterS[2]); //S
    send_char(0x06,letterSpace[0],letterSpace[1],letterSpace[2]); //Space
    send_char(0x03,letterSpace[0],letterSpace[1],letterSpace[2]); //Space
    send_char(0x00,letterSpace[0],letterSpace[1],letterSpace[2]); //Space
}
void charactersA(){
    clear_VFD();
    send_char(0x00,letterA[0], letterA[1], letterA[2]);
    send_char(0x03,letterB[0], letterB[1], letterB[2]);
    send_char(0x06,letterC[0], letterC[1], letterC[2]);
    send_char(0x09,letterD[0], letterD[1], letterD[2]);
    send_char(0x0C,letterE[0], letterE[1], letterE[2]);
    send_char(0x0F,letterF[0], letterF[1], letterF[2]);
    send_char(0x12,letterG[0], letterG[1], letterG[2]);
    send_char(0x15,letterH[0], letterH[1], letterH[2]);
}
void charactersB(){
    send_char(0x00,letterI[0], letterI[1], letterI[2]);
    send_char(0x03,letterJ[0], letterJ[1], letterJ[2]);
    send_char(0x06,letterK[0], letterK[1], letterK[2]);
    send_char(0x09,letterL[0], letterL[1], letterL[2]);
    send_char(0x0C,letterM[0], letterM[1], letterM[2]);
    send_char(0x0F,letterN[0], letterN[1], letterN[2]);
    send_char(0x12,letterO[0], letterO[1], letterO[2]);
    send_char(0x15,letterP[0], letterP[1], letterP[2]);
}
void charactersC(){
    send_char(0x00,letterQ[0], letterQ[1], letterQ[2]);
    send_char(0x03,letterR[0], letterR[1], letterR[2]);
    send_char(0x06,letterS[0], letterS[1], letterS[2]);
    send_char(0x09,letterT[0], letterT[1], letterT[2]);
    send_char(0x0C,letterU[0], letterU[1], letterU[2]);
    send_char(0x0F,letterV[0], letterV[1], letterV[2]);
    send_char(0x12,letterX[0], letterX[1], letterX[2]);
    send_char(0x15,letterZ[0], letterZ[1], letterZ[2]);
}
void charactersD(){
    send_char(0x00,letterW[0], letterW[1], letterW[2]);
    send_char(0x03,letterY[0], letterY[1], letterY[2]);
    send_char(0x06,letterSpace[0], letterSpace[1], letterSpace[2]);
    send_char(0x09,letterSpace[0], letterSpace[1], letterSpace[2]);
    send_char(0x0C,letterSpace[0], letterSpace[1], letterSpace[2]);
    send_char(0x0F,letterSpace[0], letterSpace[1], letterSpace[2]);
    send_char(0x12,letterSpace[0], letterSpace[1], letterSpace[2]);
    send_char(0x15,letterSpace[0], letterSpace[1], letterSpace[2]);
}
void numbers1(){
    clear_VFD();  
    send_char(0x03,number0[0], number0[1], number0[2]);
    send_char(0x06,number1[0], number1[1], number1[2]);
    send_char(0x09,number2[0], number2[1], number2[2]);
    send_char(0x0C,number3[0], number3[1], number3[2]);
    send_char(0x0F,number4[0], number4[1], number4[2]);
    send_char(0x12,number5[0], number5[1], number5[2]);
    send_char(0x15,number6[0], number6[1], number6[2]);
    send_char(0x18,number7[0], number7[1], number7[2]);
    send_char(0x1B,number8[0], number8[1], number8[2]);
    send_char(0x1E,number9[0], number9[1], number9[2]);
}
void readButtons(){
    //Take special attention to the initialize digital pin LED_BUILTIN as an output.
    //
    int ledPin = 13;   // LED connected to digital pin 13
    int inPin = VFD_data;     // PIN will be used as input
    int val = 0;       // variable to store the read value
    int dataIn=0;

    byte array[8] = {0,0,0,0,0,0,0,0};
    byte together = 0;

    unsigned char receive = 8; //define our transmit pin
    unsigned char data = 0; //value to transmit, binary 10101010
    unsigned char mask = 1; //our bitmask

    array[0] = 1;

    unsigned char btn1 = 0x41;

      digitalWrite(VFD_ce, LOW);
        delayMicroseconds(2);
      cmd_without_stb(0b01000010); // cmd 2 //Read Keys;Normal operation; Set pulse as 1/16
      //10: Read Key Data Is related to K1:K4 (Pins 1 until to 4)
      //11: Read SW Data Is related to switch SW1:SW4 (Pins 10 until 13)
       // cmd_without_stb((0b11000000)); //cmd 3 wich define the start address (00H to 15H)
     // send without stb
  
  pinMode(VFD_data, INPUT);  // Important this point! Here I'm changing the direction of the pin to INPUT data.
  delayMicroseconds(2);
  //PORTD != B01010100; // this will set only the pins you want and leave the rest alone at
  //their current value (0 or 1), be careful setting an input pin though as you may turn 
  //on or off the pull up resistor  
  //This don't send the strobe signal, to be used in burst data send
  digitalWrite(VFD_clk, HIGH);
         for (int z = 0; z < 6; z++){
             //for (mask=00000001; mask > 0; mask <<= 1) { //iterate through bit mask
                   for (int h =8; h > 0; h--) {
                      digitalWrite(VFD_clk, LOW);  // Data Output Pin (N-Channel, Open-Drain) This pin outputs serial data at the falling edge of the shift clock (starting from the lower bit). 
                      delayMicroseconds(2);
                     val = digitalRead(inPin);
                      //digitalWrite(ledPin, val);    // sets the LED to the button's value
                           if (val & mask){ // if bitwise AND resolves to true
                             //Serial.print(val);
                            //data =data | (1 << mask);
                            array[h] = 1;
                           }
                           else{ //if bitwise and resolves to false
                            //Serial.print(val);
                           // data = data | (1 << mask);
                           array[h] = 0;
                           }
                    digitalWrite(VFD_clk, HIGH);
                    delayMicroseconds(2);
                    
                   } 
             
              Serial.print(z);
              Serial.print(" - " );
                        
                                  for (int bits = 8 ; bits > 0; bits--) {
                                      Serial.print(array[bits]);
                                   }
                       
                        if (z==0){
                          if(array[0] == 1){
                           nFunction = 0;
                          }
                          if(array[1] == 1){
                          nFunction = 1;
                          }
                          if(array[2] == 1){
                           nFunction = 2;
                          }
                          if(array[3] == 1){
                           nFunction = 3;
                          }
                          if(array[4] == 1){
                          nFunction = 4;
                          }
                          if(array[5] == 1){
                           nFunction = 5;
                          }
                          if(array[6] == 1){
                           nFunction = 6;
                          }
                          if(array[7] == 1){
                            nFunction = 7;
                          }
                        }
                        if (z==1){
                          if(array[7] == 1){
                             if(flagSet == false)
                             flagSet=true;
                             else
                             flagSet=false;
                          }
                        }
                          if (z==1){
                            if(array[5] == 1){
                              //Your action here!
                            }
                          }

                          if (z==1){
                            if(array[4] == 1){
                             //Your action here!
                              flagReached = false;
                            }
                          }                     
                  Serial.println();
          }  // End of "for" of "z"
      Serial.println();

 digitalWrite(VFD_ce, HIGH);
 delayMicroseconds(2);
 cmd_with_stb((0b10001000) | 7); //cmd 4
 delayMicroseconds(2);
 pinMode(VFD_data, OUTPUT);  // Important this point!  // Important this point! Here I'm changing the direction of the pin to OUTPUT data.
 delay(5); 
}
void findSegments (void){
  uint8_t m = 0x00;
  uint8_t grid = 0x00;
  //       
  for(uint8_t g = 0x00; g < numberOfDigits; g=g+2){
    m=0x00;
                for (int s = 0x00; s < numberOfSegments; s++){
                    // This cycle while is used to controle button advance segments test!
                   while(1){
                            if(!buttonReleased){
                              delay(200);
                              break; //Comment this line to respect the action of button!
                            }
                            else{
                              delay(15);
                               buttonReleased = false;
                               break;
                               }
                         }
              //cmd1 Configure VFD display (grids) 
              cmd_with_stb(numberOfGrids); //       cmd 1 // 14 Grids & 14 Segments
              delayMicroseconds(1);
              cmd_with_stb((0b10001010) | 7); // IMPORTANT:  After define the size GRIDs is necessary do a cmd "ON" display.
              delayMicroseconds(1);
              cmd_with_stb(0b01000000); //       cmd 2 //Normal operation; Set pulse as 1/16
              delayMicroseconds(1);
                  digitalWrite(VFD_ce, LOW);
                  delayMicroseconds(1);
                  //
                  cmd_without_stb((0b11000000 | g )); //cmd 3 wich define the start address (00H to 2DH)
                  delayMicroseconds(1);
                      if(s < 8){
                        cmd_without_stb(0b00000001 << s); // Data to fill table 5*16 = 80 bits
                        cmd_without_stb(0b00000000); // Data to fill table 5*16 = 80 bits
                        cmd_4bitsWithout_stb(0b00000000); // Data to fill
                         Serial.print("s: "); Serial.println(s, HEX);
                      }
                      else{
                        cmd_without_stb(0b00000000); // Data to fill table 5*16 = 80 bits
                        cmd_without_stb(0b00000001 << m); // Data to fill table 5*16 = 80 bits
                        cmd_4bitsWithout_stb(0b00000000); // Data to fill
                        m++;
                        Serial.print("m: "); Serial.println(m, HEX);
                      }
                  delayMicroseconds(1);
                  digitalWrite(VFD_ce, HIGH);
                  //
                  Serial.print("grid:  "); Serial.print(grid, DEC);
                  Serial.print(";   Segment:  "); Serial.println(s, DEC);
                  delay(50);
                }
                grid++;           
  }           
}
void loop() {
    clear_VFD();
        for(uint8_t i=0; i < 4; i++){
          all_ON(); 
          delay(500);
          clear_VFD();
          delay(500);
        }
    //
    gridTestSegments(0x00);
    delay(2000);
    clear_VFD();
    gridTestSegments(0x21);
    delay(2000);
    clear_VFD();

    msgHiFolks();
    delay(2000);
    
    // findSegments();
    //segBySeg();//Teste
    
      led_on(); // This function switch the LED's you define on command to On
      delay(500);
      led_off(); // This function switch the LED's you define on command to Off
      delay(100);
    //
        while(1){ 
          readButtons();
          switch (nFunction){
            case 7: msgHiFolks(); break;
            case 6: charactersC(); break;
            case 5: charactersB(); break;
            case 4: charactersA(); break;
            case 3: charactersD(); break;
            case 2: all_ON();  break;
            case 1: numbers1(); break;
            case 0: break; //Here I don't do nothing...
            default: msgHiFolks();break;
            delay(50);
          nFunction=0;
          }  
        }
}
void buttonReleasedInterrupt() {
  buttonReleased = true; // This is the line of interrupt button to advance one step on the search of segments!
}
