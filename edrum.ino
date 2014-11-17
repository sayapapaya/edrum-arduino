// Saya Date,Eddie Farias & Stephen Suen
// 2.009
// Tech Review
// eDrum
// November 17, 2014

// intializing drum LED output
const int Crash_R = 22;
const int Crash_G = 24;
const int Crash_B = 26;

const int HiHat_R = 28;
const int HiHat_G = 30;
const int HiHat_B = 32;

const int Ride_R = 34;
const int Ride_G = 36;
const int Ride_B = 38;

const int LeftTom_R = 40;
const int LeftTom_G = 42;
const int LeftTom_B = 44;

const int RightTom_R = 46;
const int RightTom_G = 48;
const int RightTom_B = 50;

const int Snare_R = 31;
const int Snare_G = 33;
const int Snare_B = 35;

const int FloorTom_R = 37;
const int FloorTom_G = 39;
const int FloorTom_B = 41;

const int Bass_R = 43;
const int Bass_G = 45;
const int Bass_B = 47;


// intializing piezos 
const int Crash_P    = A0;
const int HiHat_P    = A1;
const int Ride_P     = A2;
const int LeftTom_P  = A3;
const int RightTom_P = A4;
const int Snare_P    = A5;
const int FloorTom_P = A6;
const int Bass_P     = A7;

int sensorReading0 = 0;
int sensorReading1 = 0;
int sensorReading2 = 0;
int sensorReading3 = 0;
int sensorReading4 = 0;
int sensorReading5 = 0;
int sensorReading6 = 0;
int sensorReading7 = 0;

//setting Thresholds (these need to be adjusted when setting up on drums)
int Crash_T    = 400;
int HiHat_T    = 400;
int Ride_T     = 400;
int LeftTom_T  = 400;
int RightTom_T = 400;
int Snare_T    = 400;
int FloorTom_T = 400;
int Bass_T     = 400;

//MakingArray of outputs
int sensors[8]   = {Crash_P, HiHat_P, Ride_P, LeftTom_P, RightTom_P, Snare_P, FloorTom_P, Bass_P};
int threshold[8] = {Crash_T, HiHat_T, Ride_T, LeftTom_T, RightTom_T, Snare_T, FloorTom_T, Bass_T};
int LED_R[8]     = {Crash_R, HiHat_R, Ride_R, LeftTom_R, RightTom_R, Snare_R, FloorTom_R, Bass_R};
int LED_G[8]     = {Crash_G, HiHat_G, Ride_G, LeftTom_G, RightTom_G, Snare_G, FloorTom_G, Bass_G};
int LED_B[8]     = {Crash_B, HiHat_B, Ride_B, LeftTom_B, RightTom_B, Snare_B, FloorTom_B, Bass_B};

//stuff from parser
boolean sequenceReceived = false;

const char startDelimiter = '[';
const char endDelimiter   = ']';

int mode = 0;
int action = 0;
int bpm = 0;
int length = 0;

int tracksProcessed = 0;
int metadata[4];
int* track1 = 0;
int* track2 = 0;
int* track3 = 0;
float* lengths = 0;

// start time of current mode
int time = 0;

// setting up inputs and outputs
void setup()
{  
  pinMode(Crash_R, OUTPUT);
  pinMode(Crash_G, OUTPUT);
  pinMode(Crash_B, OUTPUT);
  
  pinMode(HiHat_R, OUTPUT);
  pinMode(HiHat_G, OUTPUT);
  pinMode(HiHat_B, OUTPUT);
  
  pinMode(Ride_R, OUTPUT);
  pinMode(Ride_G, OUTPUT);
  pinMode(Ride_B, OUTPUT);
  
  pinMode(LeftTom_R, OUTPUT);
  pinMode(LeftTom_G, OUTPUT);
  pinMode(LeftTom_B, OUTPUT);
  
  pinMode(RightTom_R, OUTPUT);
  pinMode(RightTom_G, OUTPUT);
  pinMode(RightTom_B, OUTPUT);
  
  pinMode(Snare_R, OUTPUT);
  pinMode(Snare_G, OUTPUT);
  pinMode(Snare_B, OUTPUT);
  
  pinMode(FloorTom_R, OUTPUT);
  pinMode(FloorTom_G, OUTPUT);
  pinMode(FloorTom_B, OUTPUT);
  
  pinMode(Bass_R, OUTPUT);
  pinMode(Bass_G, OUTPUT);
  pinMode(Bass_B, OUTPUT);
   
  Serial.begin(115200); 
}
///////
//Setting up the parser functions
// builds sequence string for parsing
void processInput() {
  static String sequence = "";
  char input = Serial.read();
  switch (input) {
    case startDelimiter:
      sequence = "";
      break;
    case endDelimiter:
      parseSequence(sequence);
      break;
    default:
      sequence += input;
      break;
   }
}

// turns 'x,y,z' into pdata = [x, y, z] of type int
void splitIntArray(String array, char delimiter, int length, int pdata[]) {
  String currentString = array;
  int delimiterIndex = 0;
  int i = 0;
  while (i < length) {
    delimiterIndex = currentString.indexOf(delimiter);
    pdata[i] = currentString.substring(0, delimiterIndex).toInt();
    currentString = currentString.substring(delimiterIndex + 1);
    i++;
  }
}

// turns 'x,y,z' into pdata = [x, y, z] of type float
void splitFloatArray(String array, char delimiter, int length, float pdata[]) {
  String currentString = array;
  int delimiterIndex = 0;
  int i = 0;
  while (i < length) {
    delimiterIndex = currentString.indexOf(delimiter);
    pdata[i] = currentString.substring(0, delimiterIndex).toFloat();
    currentString = currentString.substring(delimiterIndex + 1);
    i++;
  }
}

// parses strings and assigns to variables
void parseSequence(String sequence) {
  char arrayType = sequence[0];
  if (arrayType == 'm') { // metadata
    splitIntArray(sequence.substring(2), ',', 4, metadata);
    mode = metadata[0];
    action = metadata[1];
    bpm = metadata[2];
    length = metadata[3];
    delete[] track1;
    delete[] track2;
    delete[] track3;
    delete[] lengths;
  }
  else if (arrayType == 't') { // track
   if (tracksProcessed == 0) {
      track1 = new int[length];
      splitIntArray(sequence.substring(2), ',', length, track1);
    }
    else if (tracksProcessed == 1) {
      track2 = new int[length];
      splitIntArray(sequence.substring(2), ',', length, track2);
    }
    else if (tracksProcessed == 2) { // lengths
      track3 = new int[length];
      splitIntArray(sequence.substring(2), ',', length, track3);
      tracksProcessed = -1; // reset track count
    }
    tracksProcessed++;
  }
  else if (arrayType == 'l') { // lengths
    lengths = new float[length];
    splitFloatArray(sequence.substring(2), ',', length, lengths);
    sequenceReceived = true;
  }
}
////////////////////////////////////////////
///Processing Input from Serial
void listening() {
  if (!sequenceReceived) {
    if (Serial.available() > 0) {
      processInput();
    }
  }
//  if (sequenceReceived) {
//    Serial.println("Sequence detected!");
//    Serial.print("mode: ");
//    Serial.println(mode);
//    Serial.print("action: ");
//    Serial.println(action);
//    Serial.print("bpm: ");
//    Serial.println(bpm);
//    Serial.print("length: ");
//    Serial.println(length);
//    
//    Serial.print("track 1:");
//    for (int i = 0; i < length; i++) {
//      Serial.print(" ");
//      Serial.print(track1[i]);
//    }
//    Serial.println("");
//    
//    Serial.print("track 2:");
//    for (int i = 0; i < length; i++) {
//      Serial.print(" ");
//      Serial.print(track2[i]);
//    }
//    Serial.println("");
//    
//    Serial.print("track 3:");
//    for (int i = 0; i < length; i++) {
//      Serial.print(" ");
//      Serial.print(track3[i]);
//    }
//    Serial.println("");
//    
//    Serial.print("lengths:");
//    for (int i = 0; i < length; i++) {
//      Serial.print(" ");
//      Serial.print(lengths[i]);
//    }
//    Serial.println("");
//    Serial.println("...done.");
//  }  
}



////////////////////////////////////////////
/// Set Color //


////////////////////////////////////////////
//Implementing feedback//
/////////////////////////////////////////////////
void feedback(){
  for(int i=0;i<8;i++){
    if (analogRead(sensors[i])>threshold[i]){
      int id = i;
      int start = millis()-time;
      String output = "[h:" + String(track1[i]) + "," + String(start) + "]";
      Serial.println(output);
    }
  }
}



////////////////////////////////////////////
///// MODE 1: 

void Learn_LR()
{
//  Serial.println("LEARN MODE");
  for(int i = 0; i < length; i++)
  {
    // Start with no drums to be played
    // using binary: 0 for do not play, 1 for play  
    int hit_0 = 0;
    int hit_1 = 0;
    int hit_2 = 0;
    
//    Serial.print("  Drums to hit:");
      
    // light up correct drums to be played  
    if(track1[i] > -1)
    {
      digitalWrite(LED_R[track1[i]], HIGH);
      hit_0 = 1;
//      Serial.print(" ");
//      Serial.print(track1[i]);
    }
    
    if(track2[i] > -1)
    {
      digitalWrite(LED_G[track2[i]], HIGH);
      hit_1 = 1;
//      Serial.print(" ");
//      Serial.print(track2[i]);
    }
    
    if(track3[i] > -1)
    {
      digitalWrite(LED_B[track3[i]], HIGH);
      hit_2 = 1;
//      Serial.print(" ");
//      Serial.println(track3[i]);
    }
    
    Serial.println("");
    
    // wait until all drums in sequence have been hit 
    while(hit_0 == 1 || hit_1 == 1 || hit_2 == 1)
    {
      int start_time = millis() - time;
      if(hit_0 == 1 && analogRead(sensors[track1[i]]) > threshold[track1[i]])
      {
        hit_0 = 0;
        String output = "[h:" + String(track1[i]) + "," + String(start_time) + "]";
//        Serial.print("    ");
        Serial.println(output);
      }
        
      if(hit_1 == 1 && analogRead(sensors[track2[i]]) > threshold[track2[i]])
      {
        hit_1 = 0;
        String output = "[h:" + String(track2[i]) + "," + String(start_time) + "]";
//        Serial.print("    ");
        Serial.println(output);
      }
        
      if(hit_2 == 1 && analogRead(sensors[track3[i]]) > threshold[track3[i]])
      {
        hit_2 = 0;
        String output = "[h:" + String(track2[i]) + "," + String(start_time) + "]";
//        Serial.print("    ");
        Serial.println(output);
      }   
    }
    
    // turn off all drums 
    digitalWrite(LED_G[0], LOW);
    digitalWrite(LED_G[1], LOW);
    digitalWrite(LED_G[2], LOW);
    digitalWrite(LED_G[3], LOW);
    digitalWrite(LED_G[4], LOW);
    digitalWrite(LED_G[5], LOW);
    digitalWrite(LED_G[6], LOW);
    digitalWrite(LED_G[7], LOW);
    
    digitalWrite(LED_R[0], LOW);
    digitalWrite(LED_R[1], LOW);
    digitalWrite(LED_R[2], LOW);
    digitalWrite(LED_R[3], LOW);
    digitalWrite(LED_R[4], LOW);
    digitalWrite(LED_R[5], LOW);
    digitalWrite(LED_R[6], LOW);
    digitalWrite(LED_R[7], LOW);
    
    digitalWrite(LED_B[0], LOW);
    digitalWrite(LED_B[1], LOW);
    digitalWrite(LED_B[2], LOW);
    digitalWrite(LED_B[3], LOW);
    digitalWrite(LED_B[4], LOW);
    digitalWrite(LED_B[5], LOW);
    digitalWrite(LED_B[6], LOW);
    digitalWrite(LED_B[7], LOW);
    
    delay(500);
  }  
  
  Serial.println("[e]");
}
////
//Without right and left input
void Learn()
{
//  Serial.println("LEARN MODE");
  for(int i = 0; i < length; i++)
  {
    // Start with no drums to be played
    // using binary: 0 for do not play, 1 for play  
    int hit_0 = 0;
    int hit_1 = 0;
    int hit_2 = 0;
    
//    Serial.print("  Drums to hit:");
      
    // light up correct drums to be played  
    if(track1[i] > -1)
    {
      digitalWrite(LED_G[track1[i]], HIGH);
      hit_0 = 1;
//      Serial.print(" ");
//      Serial.print(track1[i]);
    }
    
    if(track2[i] > -1)
    {
      digitalWrite(LED_G[track2[i]], HIGH);
      hit_1 = 1;
//      Serial.print(" ");
//      Serial.print(track2[i]);
    }
    
    if(track3[i] > -1)
    {
      digitalWrite(LED_G[track3[i]], HIGH);
      hit_2 = 1;
//      Serial.print(" ");
//      Serial.println(track3[i]);
    }
    
    Serial.println("");
    
    // wait until all drums in sequence have been hit 
    while(hit_0 == 1 || hit_1 == 1 || hit_2 == 1)
    {
      int start_time = millis() - time;
      if(hit_0 == 1 && analogRead(sensors[track1[i]]) > threshold[track1[i]])
      {
        hit_0 = 0;
        String output = "[h:" + String(track1[i]) + "," + String(start_time) + "]";
//        Serial.print("    ");
        Serial.println(output);
      }
        
      if(hit_1 == 1 && analogRead(sensors[track2[i]]) > threshold[track2[i]])
      {
        hit_1 = 0;
        String output = "[h:" + String(track2[i]) + "," + String(start_time) + "]";
//        Serial.print("    ");
        Serial.println(output);
      }
        
      if(hit_2 == 1 && analogRead(sensors[track3[i]]) > threshold[track3[i]])
      {
        hit_2 = 0;
        String output = "[h:" + String(track2[i]) + "," + String(start_time) + "]";
//        Serial.print("    ");
        Serial.println(output);
      }   
    }
    
    // turn off all drums 
    digitalWrite(LED_G[0], LOW);
    digitalWrite(LED_G[1], LOW);
    digitalWrite(LED_G[2], LOW);
    digitalWrite(LED_G[3], LOW);
    digitalWrite(LED_G[4], LOW);
    digitalWrite(LED_G[5], LOW);
    digitalWrite(LED_G[6], LOW);
    digitalWrite(LED_G[7], LOW);
    
    digitalWrite(LED_R[0], LOW);
    digitalWrite(LED_R[1], LOW);
    digitalWrite(LED_R[2], LOW);
    digitalWrite(LED_R[3], LOW);
    digitalWrite(LED_R[4], LOW);
    digitalWrite(LED_R[5], LOW);
    digitalWrite(LED_R[6], LOW);
    digitalWrite(LED_R[7], LOW);
    
    digitalWrite(LED_B[0], LOW);
    digitalWrite(LED_B[1], LOW);
    digitalWrite(LED_B[2], LOW);
    digitalWrite(LED_B[3], LOW);
    digitalWrite(LED_B[4], LOW);
    digitalWrite(LED_B[5], LOW);
    digitalWrite(LED_B[6], LOW);
    digitalWrite(LED_B[7], LOW);
    
    delay(500);
  }  
  
  Serial.println("[e]");
}
////////////////////////////////////////////
///// DEMO with right left:
void Demo_LR()
{
  boolean songEnd = false;
  for(int i=0 ; i < length; i++ ){
      int led_time = (lengths[i]/bpm)*60000;
 //Turning on LEDs of corresponding drums
      if (track1[i] > -1)
      {
        digitalWrite(LED_G[track1[i]], HIGH);
      }
      if(track2[i] > -1)
      {
        digitalWrite(LED_G[track2[i]], HIGH);
      }
      if(track3[i] > -1)
      {
        digitalWrite(LED_G[track3[i]], HIGH);
      }
  

//    
//    //How long LED Stays on
      delay(led_time/2); //This delay would change based on size of note later intialized in an array
//    
//    //turning off LEDs
      if (track1[i] > -1)
      {
        digitalWrite(LED_G[track1[i]], LOW);
      }
      if(track2[i] > -1)
      {
       digitalWrite(LED_G[track2[i]], LOW);
      }
      if(track3[i] > -1)
     {
       digitalWrite(LED_G[track3[i]], LOW);
     }
     delay(led_time/2); // this delay is part of the shared delay; delay between notes
     if(i==length-1){
       delay(3000);
       songEnd == true;
     }
    }
    //keep getting feedback till the song ends
  }
  
  
  /////////////////////////////////////////////////////
  ////////////////// Demo without right left
  void Demo()
{
  boolean songEnd = false;
  for(int i=0 ; i < length; i++ ){
      int led_time = (lengths[i]/bpm)*60000;
 //Turning on LEDs of corresponding drums
      if (track1[i] > -1)
      {
        digitalWrite(LED_G[track1[i]], HIGH);
      }
      if(track2[i] > -1)
      {
        digitalWrite(LED_G[track2[i]], HIGH);
      }
      if(track3[i] > -1)
      {
        digitalWrite(LED_G[track3[i]], HIGH);
      }
  

//    
//    //How long LED Stays on
      delay(led_time/2); //This delay would change based on size of note later intialized in an array
//    
//    //turning off LEDs
      if (track1[i] > -1)
      {
        digitalWrite(LED_G[track1[i]], LOW);
      }
      if(track2[i] > -1)
      {
       digitalWrite(LED_G[track2[i]], LOW);
      }
      if(track3[i] > -1)
     {
       digitalWrite(LED_G[track3[i]], LOW);
     }
     delay(led_time/2); // this delay is part of the shared delay; delay between notes
     if(i==length-1){
       delay(3000);
       songEnd == true;
     }
    }
  }

////////////////////////////////////////////
///// MODE 2: FOLLOW 
///// this mode currently does not allow for accuracy readings, can be implemented later on now
///// that we know how to do simulateous hit detection (refer to mode 1)

// Values to change, discuss with andrew for appropriate rhythem
//const float led_time_actual = 400; // how long led stays on
//const float note_delay_actual = 200; // timing between notes (ie time between different leds turning on)
//const float speed_multiplier = 2; // speed you want to play at relative to "actual" playing speed
//
//float led_time = led_time_actual / speed_multiplier;
//float note_delay = note_delay_actual / speed_multiplier;
//
void Play()
{
  boolean songEnd = false;
  for(int i=0 ; i < length; i++ ){
      int led_time = (lengths[i]/bpm)*60000;
 //Turning on LEDs of corresponding drums
      if (track1[i] > -1)
      {
        digitalWrite(LED_G[track1[i]], HIGH);
      }
      if(track2[i] > -1)
      {
        digitalWrite(LED_G[track2[i]], HIGH);
      }
      if(track3[i] > -1)
      {
        digitalWrite(LED_G[track3[i]], HIGH);
      }
  

//    
//    //How long LED Stays on
      delay(led_time/2); //This delay would change based on size of note later intialized in an array
//    
//    //turning off LEDs
      if (track1[i] > -1)
      {
        digitalWrite(LED_G[track1[i]], LOW);
      }
      if(track2[i] > -1)
      {
       digitalWrite(LED_G[track2[i]], LOW);
      }
      if(track3[i] > -1)
     {
       digitalWrite(LED_G[track3[i]], LOW);
     }
     delay(led_time/2); // this delay is part of the shared delay; delay between notes
     if(i==length-1){
       delay(3000);
       songEnd == true;
     }
    }
    //keep getting feedback till the song ends
   while(songEnd == false){
      feedback();
    }
  }

  

////////////////////////////////////////////
////////////////////////////////////////////
///// Begin Program   

// how long to wait before program loops  
//const int delay_loop = 1000000000;

void loop(){
  // Setup
  listening();
  
  if (sequenceReceived) {
    time = millis();
    
    // Choose Mode
    if (mode == 0){
      if(action ==0){
        Learn();
      }
      if(action ==1){
        Learn_LR();
      }
      if(action ==2){
        Demo();
      }
      if(action ==3){
        Demo_LR();
      }    
      // Wait to hit
      
    }
    if (mode == 1){
      // Play through
      Play();
    }
    if (mode == 2){
      // Record Mode
      //Record();
    }
    sequenceReceived = false;
  }
  
  //Piezo_Testing();
  //delay(delay_loop);
  
}
