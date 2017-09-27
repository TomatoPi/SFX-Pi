#include <LedControl.h>

#define latch 3
#define data 4
#define clck 5

#define A1 B00000100
#define A2 B01110111

#define B1 B00000100
#define B2 B01111100

#define C1 B00000000
#define C2 B00111001

#define D1 B00000100
#define D2 B01011110

#define E1 B00000100
#define E2 B01111001

#define F1 B00000000
#define F2 B01110001

#define G1 B00000100
#define G2 B00111101

#define H1 B00000100
#define H2 B01110110

#define I1 B00010001
#define I2 B00000000

#define J1 B00000000
#define J2 B00011110

#define K1 B00001010
#define K2 B01110000

#define L1 B00000000
#define L2 B00111000

#define M1 B00000010
#define M2 B10110110

#define N1 B00001000
#define N2 B10110110

#define O1 B00000000
#define O2 B00111111

#define P1 B00000100
#define P2 B01110011

#define Q1 B00001000
#define Q2 B00111111

#define R1 B00001100
#define R2 B01110011

#define S1 B00000100
#define S2 B01101101

#define T1 B00010001
#define T2 B00000001

#define U1 B00000000
#define U2 B00111110

#define V1 B00100010
#define V2 B00110000

#define W1 B00101000
#define W2 B00110110

#define X1 B00101010
#define X2 B10000000

#define Y1 B00010010
#define Y2 B10000000

#define Z1 B00100010
#define Z2 B00001001

#define i1 B00000000
#define o1 B00000110

#define i2 B00000100
#define o2 B01011011

#define i3 B00000100
#define o3 B01001111

#define i4 B00000100
#define o4 B01100110

#define i5 B00000100
#define o5 B01101101

#define i6 B00000100
#define o6 B01111101

#define i7 B00000000
#define o7 B00000111

#define i8 B00000100
#define o8 B01111111

#define i9 B00000100
#define o9 B01101111

#define i0 B00000000
#define o0 B00111111

#define tiret1 B00000100
#define tiret2 B01000000

#define slash1 B00100010
#define slash2 B00000000

#define under1 B00000000
#define under2 B00001000

#define SCREEN_SIZE 12
#define BUFFER_SIZE 100
#define SCROLL_TIME 200

#define MAX_MESSAGES_COUNT 5

const byte ALPHANUM[][2] = {{A1, A2}, {B1, B2}, {C1, C2}, {D1, D2}, {E1, E2}, {F1, F2}, {G1, G2}, {H1, H2}, {I1, I2}, {J1, J2}, {K1, K2}, {L1, L2}, {M1, M2}, {N1, N2}, {O1, O2}, {P1, P2}, {Q1, Q2}, {R1, R2}, {S1, S2}, {T1, T2}, {U1, U2}, {V1, V2}, {W1, W2}, {X1, X2}, {Y1, Y2}, {Z1, Z2}, 
                     {i0, o0}, {i1, o1}, {i2, o2}, {i3, o3}, {i4, o4}, {i5, o5}, {i6, o6}, {i7, o7}, {i8, o8}, {i9, o9}};

typedef enum{

  N_FLAG, //Not parsing a message

  P_FLAG, //Parsing flag

  W_FOR,  //Wait forced index
  P_FOR, //Parse forced index

  W_DUR,  //Wait parse duration flag
  P_DUR,  //Parse duration

  W_MEG,  //Wait parse message
  P_MEG,   //Parse message

  W_SEG, //Wait 7segment datas
  P_SEG  //Pase 7Segment datas
}PARSE_FLAG;

/*
 * Input Parsing stuff
 */
PARSE_FLAG FLAG = N_FLAG;

String inBuff = "";
int inDur = 0;
int inIdx = 0;

int inSeg = 0;

bool isTemp = false;

bool has_changed = true;

/*
 * Messages Buffer stuff
 */

struct MESSAGE{

  String m_;
  int d_;
};

// Store all avaiable messages for display
MESSAGE mBuff[MAX_MESSAGES_COUNT];

bool overrideBuff = true;
int currentIndex = 0;

bool forceIndex = false;

unsigned long timeB = millis();

/*
 * Temp Message stuff
 */
MESSAGE tempM;

bool hasTempM = false;

/*
 * Screen managing stuff
 */
byte screen[2][BUFFER_SIZE] = {};
unsigned long time_l = millis();
int idx = -1;
boolean newText = false;

boolean expm = false;

/*
 * 7 Segments display stuff 
 */
 /*
  * pin 12 is connected to the MAX7219 pin 1
  * pin 11 is connected to the CLK pin 13
  * pin 10 is connected to LOAD pin 12
  * 1 as we are only using 1 MAX7219
  * true since this is a common anode display setup
  */
#define SEG7_COUNT 4

LedControl lc = LedControl(12,11,10,1,true); // lc is our object
int idx7 = 0;

boolean newSeg7 = false;

// 7Segment display buffer
char seg7[SEG7_COUNT] = {};
boolean dot7[SEG7_COUNT] = {};

char seg7Temp[SEG7_COUNT] = {};
boolean dot7Temp[SEG7_COUNT] = {};

unsigned long time7 = millis();
boolean hasTemp7 = false;
int temp7dur = 0;

boolean exp7 = false;

#define SEG7_TIME 500

void setup() {
  //set pins to output because they are addressed in the main loop
  pinMode(latch, OUTPUT);
  pinMode(clck, OUTPUT);
  pinMode(data, OUTPUT);
  Serial.begin(57600);

  for(int i = 0; i < 4; i++){
    shiftOut(data, clck, MSBFIRST, B00000000);
  }

  lc.shutdown(0,false); // turn off power saving, enables display
  lc.setIntensity(0,5); // sets brightness (0~15 possible values)
  lc.clearDisplay(0); // clear screen
}

void loop() {

  // New Seg14 message received
  if (newText && !forceIndex ){

    if ( has_changed || expm ){
      
      Serial.println( "Loop : New Text" );
  
      String n = (hasTempM)? tempM.m_ : mBuff[currentIndex].m_ ;
      
      preRenderScreen( n );

      expm = !hasTempM;
    }
    
    newText = false;
    timeB = millis();
  }
  // New Seg7 message received
  else if ( newSeg7 ){

    if ( has_changed || exp7 ){
        
      Serial.println( "Loop : New Seg7" );
  
      lc.clearDisplay( 0 );
      
      for ( int i = 0; i < SEG7_COUNT ; i ++ ){
        
        if ( hasTemp7 ){
          
          lc.setChar( 0, i, seg7Temp[i], dot7Temp[i] );
          exp7 = false;
        }
        else{
          
          lc.setChar( 0, i, seg7[i], dot7[i] );
        }
      }
    }

    newSeg7 = false;
    
    time7 = millis();
  }
  else if ( forceIndex ){

    Serial.println( "Loop : ForceIndex" );

    preRenderScreen( mBuff[currentIndex].m_ );

    forceIndex = false;
    newText = false;
    
    timeB = millis();
    
  }
  else if ( hasTempM && millis() - timeB > tempM.d_ ){

    Serial.println( "Loop : Temp Message Expired" );

    newText = true;
    hasTempM = false;

    expm = true;
    
    timeB = millis();
  }
  else if ( hasTemp7 && millis() - time7 > temp7dur ){

    Serial.println( "Loop : Temp Seg7 Expired" );

    newSeg7 = true;
    hasTemp7 = false;

    exp7 = true;
    
    time7 = millis();
  }
  else if ( ( mBuff[currentIndex].d_ != 0 ) && ( millis() - timeB > mBuff[currentIndex].d_ ) ){

    Serial.println( "Loop : Current Message Expired" );

    bufferNext();
    preRenderScreen( mBuff[currentIndex].m_ );

    timeB = millis();
  }
  
  printScreen(idx);
  
}

void preRenderScreen(String& msg){

  msg.toUpperCase();
  for(int i = 0; i < BUFFER_SIZE; i++){
      byte char1 = 0;
      byte char2 = 0;
    char c = msg.charAt(i);
    if(c > 47 && c < 58){
      char1 = ALPHANUM[c-22][0];
      char2 = ALPHANUM[c-22][1];
    }
    else if(c > 64 && c < 91){
      char1 = ALPHANUM[c-65][0];
      char2 = ALPHANUM[c-65][1];
    }
    else if(c > 96 && c < 123){
      char1 = ALPHANUM[c-97][0];
      char2 = ALPHANUM[c-97][1];
    }else if(c == 45){
      char1 = tiret1;
      char2 = tiret2;
    }else if(c == 47){
      char1 = slash1;
      char2 = slash2;
    }else if(c == 95){
      char1 = under1;
      char2 = under2;
    }
    screen[0][i] = char1;
    screen[1][i] = char2;
  }
}

void printScreen(int& index){

    for(int i = 1; i < SCREEN_SIZE; i++){
      digitalWrite(latch, HIGH);
      shiftOut(data, clck, MSBFIRST, (i>7)?(1<<i-8):B00000000);
      shiftOut(data, clck, MSBFIRST, (i<8)?(1<<i):B00000000);
      if((i+index) >= BUFFER_SIZE || (i+index) < 0){
        shiftOut(data, clck, MSBFIRST, 0);
        shiftOut(data, clck, MSBFIRST, 0);
      }else{
        shiftOut(data, clck, MSBFIRST, screen[0][i+index]);
        shiftOut(data, clck, MSBFIRST, screen[1][i+index]);
      }
      /*
      if(index >= textL && i == 1){
        index = -SCREEN_SIZE;
      }
      */
      digitalWrite(latch, LOW);
      delay(1);
    }
    /*
    if(scroll && millis() - time_l  > SCROLL_TIME){
      index++;
      time_l = millis();
    }
    */
}

void bufferNext(){

  currentIndex = ( currentIndex +1 ) % MAX_MESSAGES_COUNT;
}

void bufferPrev(){

  currentIndex = ( currentIndex - 1  < 0)? MAX_MESSAGES_COUNT - 1 : currentIndex - 1 ;
}

void bufferClear(){

  for ( int i = 0; i < MAX_MESSAGES_COUNT; i++ ){

    mBuff[i].m_ = "";
    mBuff[i].d_ = 0;
  }

  currentIndex = 0;
  
}

MESSAGE newMessage( String m, int d ){

  MESSAGE tmp = { m, d };

  Serial.print( " , Duration : ");
  Serial.print( d );
  
  Serial.print( " , Message : ");
  Serial.print( m );
  Serial.println( " --" );
        
  return tmp;
}

void serialEvent() {
  
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    
    //Serial.write(inChar);

    // If not parsing message, wait for '[' symbol and begin parsing
    if ( FLAG == N_FLAG ){
      
      if (inChar == '[') {
        
        FLAG = P_FLAG;
        
        overrideBuff = true;
        inDur = 0;

        isTemp = false;

        has_changed = false;
        
        forceIndex = false;
      }
    } 
    // If parse message flag
    else if ( FLAG == P_FLAG ){
      
      if ( inChar == 'm' ){

        FLAG = W_MEG;
      }
      else if ( inChar == '7' ){

        FLAG = W_SEG;
      }
      else if ( inChar == 's' ){

        overrideBuff = false;
        inIdx = (inIdx + 1) % MAX_MESSAGES_COUNT;
      }
      else if ( inChar == 'o' ){

        inIdx = currentIndex;
        has_changed = true;
      }
      else if ( inChar == 'c' ){

        bufferClear();
        inIdx = currentIndex;
      }
      else if ( inChar == 'd' ){

        FLAG = W_DUR;
      }
      else if ( inChar == 'f' ){

        FLAG = W_FOR;
        forceIndex = true;
        has_changed = true;
      }
      else if ( inChar == 't' ){

        isTemp = true;
        inDur = 1000;
      }
      else if ( inChar == ']' ){

        FLAG = N_FLAG;
        newText = !newSeg7;
      }
    }
    // Forced index parsing
    else if ( FLAG == W_FOR ){

      if ( inChar == '[' ){

        FLAG = P_FOR;
        inBuff = "";
      }
      else if ( inChar == 'n' ){

        bufferNext();
        FLAG = P_FLAG;
      }
      else if ( inChar == 'p' ){

        bufferPrev();
        FLAG = P_FLAG;
      }
      else if ( inChar == 't' ){

        hasTempM = true;
        hasTemp7 = true;
        forceIndex = false;
        FLAG = P_FLAG;
      }
    }
    else if ( FLAG == P_FOR ){
      
      if ( inChar == ']' ){

        currentIndex = inBuff.toInt();
        
        if ( currentIndex >= MAX_MESSAGES_COUNT ) currentIndex = MAX_MESSAGES_COUNT - 1 ;
        if ( currentIndex < 0 ) currentIndex = 0 ;
        
        FLAG = P_FLAG;
      }
      else {

        inBuff += inChar;
      }
    }
    // Wait for Duration's begin flag
    else if ( FLAG == W_DUR ){

      if ( inChar == '[' ){
        
        FLAG = P_DUR;
        inBuff = "";
      }
    }
    // Parse duration
    else if ( FLAG == P_DUR ){

      if ( inChar == ']' ){

        inDur = inBuff.toInt();
        FLAG = P_FLAG;
      }
      else {

        inBuff += inChar;
      }
    }
    // Wait for message's begin flag
    else if ( FLAG == W_MEG ){
      
      if ( inChar == '[' ){

        FLAG = P_MEG;
        inBuff = "";
      }
    }
    // Parse message
    else if ( FLAG == P_MEG ){

      // If reached message's end, add it to buffer and change flag
      if ( inChar ==  ']' ){

        hasTempM = isTemp;

        if ( isTemp ){
  
          Serial.print( "New Temp Message parsed, ");
          
          inBuff.toUpperCase();
          has_changed |= !inBuff.equals( tempM.m_ );
  
          tempM = newMessage( inBuff, inDur );
          FLAG = P_FLAG;
        }
        else{
  
          Serial.print( "New Message parsed, Index : ");
          Serial.print( inIdx );
          
          inBuff.toUpperCase();
          has_changed |= !inBuff.equals( mBuff[inIdx].m_ );
  
          mBuff[inIdx] = newMessage( inBuff, inDur );
          FLAG = P_FLAG;
        }
      }
      // Else Add new char to the message
      else {

        inBuff += inChar;
      }
    }
    else if ( FLAG == W_SEG ){

      FLAG = P_SEG;
      inBuff = "";
      inSeg = 0;
      
    }
    else if ( FLAG == P_SEG ){

      if ( inChar == ']' ){

        hasTemp7 = isTemp;

        FLAG = P_FLAG;

        if ( isTemp ){

          temp7dur = inDur;
          
          Serial.print( "New Temp 7Segments Message Parsed, " );
          for ( int i = 0; i < SEG7_COUNT; i++ ){
  
            Serial.print( seg7Temp[i] );
            Serial.print( "." );
            Serial.print( dot7Temp[i] );
            Serial.print( " " );
          }
          Serial.println( "--" );
        }
        else {
          
          Serial.print( "New 7Segments Message Parsed, " );
          for ( int i = 0; i < SEG7_COUNT; i++ ){
  
            Serial.print( seg7[i] );
            Serial.print( "." );
            Serial.print( dot7[i] );
            Serial.print( " " );
          }
          Serial.println( "--" );
        }

        newSeg7 = true;
      }
      else if ( inChar == 'f' && inSeg < SEG7_COUNT ){

        if ( isTemp ){

          has_changed |= dot7Temp[inSeg] != inChar;
          dot7Temp[inSeg] = true;
        }
        else{

          has_changed |= dot7[inSeg] != inChar;
          dot7[inSeg] = true;
        }
      }
      else if ( inSeg < SEG7_COUNT ){

        if ( isTemp ){

          has_changed |= seg7Temp[inSeg] != inChar;
          seg7Temp[inSeg] = inChar;
        }
        else{

          has_changed |= seg7[inSeg] != inChar;
          seg7[inSeg] = inChar;
        }
          
        inSeg++;
        
      }
    }
  }
}
//[od[2000]m[test1]]
//[sd[1500]m[poum2]]
//[sd[3000]m[patata3]]
//[sd[9000]m[avant]][sm[dernier]]
//[tm[temp]]
//[7[1234]]

