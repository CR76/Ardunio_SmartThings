int x;
int lengths;

char api_frame_RE[8]    = {0x7E, 0x00, 0x04, 0x08, 0x01, 0x52, 0x45, 0x5F};                             // 0 = Restore Defaults        
char api_frame_EE[9]    = {0x7E, 0x00, 0x05, 0x08, 0x01, 0x45, 0x45, 0x01, 0x6B};                       // 1 = Encryption Enabled
char api_frame_KY[24]   = {0x7E, 0x00, 0x14, 0x08, 0x01, 0x4B, 0x59, 0x5A, 0x69, 0x67, 0x42, 0x65, 0x65, 0x41, 0x6C, 0x6C, 0x69, 0x61, 0x6E, 0x63, 0x65, 0x30, 0x39, 0x9A}; // HA Encryption key  0x5A,69,67,42,65,65,41,6C,6C,69,61,6E,63,65,30,39
char api_frame_NJ[9]    = {0x7E, 0x00, 0x05, 0x08, 0x01, 0x4E, 0x4A, 0x5A, 0x04};                       // 5A = 90 seconds Node Join Time
char api_frame_ZS[9]    = {0x7E, 0x00, 0x05, 0x08, 0x01, 0x5A, 0x53, 0x02, 0x47};                       // 2 = Zigbee PRO stack.
char api_frame_EO[9]    = {0x7E, 0x00, 0x05, 0x08, 0x01, 0x45, 0x4F, 0x01, 0x61};                       // 1 = Encryption Options
char api_frame_AO[9]    = {0x7E, 0x00, 0x05, 0x08, 0x01, 0x41, 0x4F, 0x03, 0x63};                       // API Output frame = 3 ZDO Pass through.
char api_frame_WR[8]    = {0x7E, 0x00, 0x04, 0x08, 0x01, 0x57, 0x52, 0x4D};                             // Write. Write parameter values to non-volatile memory
char api_frame_CB[8]    = {0x7E,0x00,0x04,0x08,0x21,0x43,0x42,0x51};                                    // Create AT command to leave network.


void setup() {
  Serial.begin(9600);
  
  Serial.print("Configuring Xbee");
  
  Reset();

  Serial.print("Configuration complete");
}

void loop() {
  // put your main code here, to run repeatedly:

}


void APIsend(char API_packet[]){
  
    lengths = (API_packet[2] + 4);                                       //Calaculate packet length using packet length identifier and add the start byte, 2 byte length and checksum.
    x = 0;
    while (x < lengths){                                                 //Send the command
        Serial.print(API_packet[x]);
        x ++;
    }
}

void ATsend(char AT_packet[], int Length){
  
    x = 0;
    while (x < Length){                                                 //Send the command
        Serial.print(AT_packet[x]);
        x ++;
    }
}

void Reset(){
      
    APIsend(api_frame_RE);          //Restore defaults
    delay(1500);                      //IMPORTANT- GUARD TIME must be > 1 second
    ATsend("+++", 3);
    delay(1500);                      //IMPORTANT- GUARD TIME must be > 1 second
    ATsend("ATAP1\r", 6);           //Set XBee to API mode.
    delay(250);
    ATsend("ATWR\r", 5);            //Write command.
    delay(250);
    ATsend("ATCN\r", 5);            //Exit command mode.
    delay(250);
    APIsend(api_frame_EE);          //Encryption enabled.
    delay(100);
    APIsend(api_frame_KY);          //HA Encryption key.
    delay(100);
    APIsend(api_frame_EO);          //Encryption Options.    
    delay(100);
    APIsend(api_frame_NJ);          //Node join time.
    delay(100);
    APIsend(api_frame_ZS);          //Zigbee Stack.
    delay(100);
    APIsend(api_frame_AO);          //API output frame.
    delay(100);
    APIsend(api_frame_WR);          //Write.
    delay(100);
    APIsend(api_frame_CB);          //Leave network.
}
