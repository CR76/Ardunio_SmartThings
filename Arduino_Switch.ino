#define Start_Byte 0x7E
#define AppV 0x00
#define HarV 0x00



char AI[8] = {0x7E,0x00,0x04,0x08,0x21,0x41,0x49,0x4C};                 //create AT command to querey network joined?
char MY[8] = {0x7E,0x00,0x04,0x08,0x21,0x4D,0x59,0x30};                 //create AT command to get my network ID.
char SL[8] = {0x7E,0x00,0x04,0x08,0x21,0x53,0x4C,0x37};                 //create AT command to get 64bit LSB.
char SH[8] = {0x7E,0x00,0x04,0x08,0x21,0x53,0x48,0x3B};                 //create AT command to get 64bit MSB.
char CB[8] = {0x7E,0x00,0x04,0x08,0x21,0x43,0x42,0x51};                 //Create AT command to leave network.
//char Packet[64];
int Packet[64];
char Address[10];
int check = 0;
boolean Network_Joined = false;

void setup() {
pinMode(13, OUTPUT); 
Serial.begin(9600); 
 

while (Network_Joined == false){
  Packet_Send(AI);
  delay(500);
  while(Serial.available() == 0);
  Event();
}

Packet_Send(MY);
delay(50);
while(Serial.available() == 0);
Event();

Packet_Send(SH);
delay(50);
while(Serial.available() == 0);
Event();

Packet_Send(SL);
delay(50);
while(Serial.available() == 0);
Event();

Device_Announce(); 

}

void loop() {
  delay(50);
  while(Serial.available() == 0);
  Event();
}

void Event() {

//char checksum = 0;
int checksum = 0;
int Current_Byte;
int x = 0;
int Length = 0;
  
  //while (Serial.available() == 0);
  Current_Byte = Serial.read();
  if (Current_Byte == Start_Byte){
    memset(Packet, 0, sizeof(Packet));
    Packet[0] = Current_Byte;
    while (Serial.available() == 0);
    Packet[1] = Serial.read();
    //Serial.print("Length: ");
    while (Serial.available() == 0);
    //Serial.print( Packet[1]);
    Packet[2] = Serial.read();
    Length = Packet[2];
    //Serial.print("Length: ");
    //Serial.print(Length);
    for ( x = 0; x < (Length); x++){
      delay(10);
      Packet[(x+3)] = Serial.read();
      //Serial.print(Packet[(x+3)], HEX);
      checksum = (checksum + Packet[(x+3)]);
    }
    Packet[(x+3)] = Serial.read();
    //Serial.print(Packet[(x+3)], HEX);
    checksum = (0xFF - (checksum & 0xFF));
    //Serial.print(checksum, HEX);
    if (Packet[x+3] == checksum){
      Serial.print("Good");
      Packet_Type();
    }
    else{
      //Packet Bad
      Serial.print("BAD");
    }
  }
}

void Packet_Type() {
  if (Packet[3] == 0x91){                                     //If packet ID = 0x91
        if (Packet[19] == 0x00){                                        //If packet profile = 16.
          if (Packet[17] == 0x04){                                      //If packet cluster = 14.
            Serial.print("Sim Desc");
            Simple_Description();                                               //Process Simple Desciption request.
          }
          else if (Packet[17] ==0x05){                                    //If packet cluster = 15.
            Serial.print("active EP");
            Active_EP_Request();                                              //Process active EP Request.
          }
          else{
                                                                         //Unknown packet
          }
        }
    else{                                                               //Else if packet profile is not = 00.
        if (Packet[17] == 0x00){                                        //If packet Cluster = 00.
            Serial.print("Clu Basic");
            Cluster_Basic();                                             //Process Cluster Basic request.
        }
        else if (Packet[17] == 0x06){                                    //Else if packet profile is not = 06.
            On_Off();                                                    //Process On/Off.
        }
        else{
          
        }
    }
  }
  
  else if(Packet[3] == 0x88){                                  //Else if packet ID = 0x88.
    if(Packet[6] == 0x49){                                     //Connected to Network
      if(Packet[8] == 0x00){
        Network_Joined = true;
      }
    }
    else if(Packet[6] == 0x59){
      Address[0] = Packet[8];
      Address[1] = Packet[9];
    }
    else if(Packet[6] == 0x48){
      Address[2] = Packet[8];
      Address[3] = Packet[9];
      Address[4] = Packet[10];
      Address[5] = Packet[11];
    }
    else if(Packet[6] == 0x4C){
      Address[6] = Packet[8];
      Address[7] = Packet[9];
      Address[8] = Packet[10];
      Address[9] = Packet[11];
    }
  }
  else{
    memset(Packet, 0, sizeof(Packet));                        //Unknown Packet
  }
}

void PBuild(char FrameAddr[], char sEP, char dEP, char Profile[], char Cluster[], char Payload[],int Pay_Size){
    
    int z = 0;                                                          //Set counter to 0.
    char Output_Packet[64];
    char Checksum = 0;
    memset(Output_Packet, 0, sizeof(Output_Packet));                    //Clear output buffer.

    Pay_Size = 20 + (Pay_Size);                                               //Find out the size of the out going packet.

    Output_Packet[0] = 0x7E;                                                  //Packet start bit.
    Output_Packet[1] = 0x00;                                                  //Packet length MSB.
    Output_Packet[2] = Pay_Size;                                                  //Packet length LSB.
    Output_Packet[3] = 0x11;                                                  //Packet Type
    Output_Packet[4] = 0x00;                                                  //No response needed.
    Output_Packet[5] = FrameAddr[0];                                          //64 bit address High
    Output_Packet[6] = FrameAddr[1];  
    Output_Packet[7] = FrameAddr[2];
    Output_Packet[8] = FrameAddr[3];
    Output_Packet[9] = FrameAddr[4];                                          //64 bit address Low
    Output_Packet[10] = FrameAddr[5];
    Output_Packet[11] = FrameAddr[6];
    Output_Packet[12] = FrameAddr[7];
    Output_Packet[13] = FrameAddr[8];                                         //16 bit PAN address
    Output_Packet[14] = FrameAddr[9];
    Output_Packet[15] = sEP;                                                  //Source Endpoint
    Output_Packet[16] = dEP;                                                  //Destinaton Endpoint
    Output_Packet[17] = Cluster[0];                                           //Cluster ID
    Output_Packet[18] = Cluster[1];
    Output_Packet[19] = Profile[0];                                           //Profile ID
    Output_Packet[20] = Profile[1];
    Output_Packet[21] = 0x00;                                                 //Broadcast radius
    Output_Packet[22] = 0x00;                                                 //Trasmit option bitfield
    while (z < Pay_Size){
        Output_Packet[(23 + z)] = Payload[z];                                 //Copy packet payload
        z ++;
    }

    z = 0;                                                              //Set Alternatie counter to 0
    Checksum = 0;
    while (z < (Output_Packet[2])){                                           //Create the checksum.
        Checksum = (Checksum + Output_Packet[z + 3]);                           
        z++;
    }
    Output_Packet[z+3] = (0xFF - ( Checksum & 0xFF));
                                                        
    Packet_Send(Output_Packet);                                                     //Send the packet.
}

void Device_Announce(){
    char Anncounce_Frame[10] = {0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFE};
    char Anncounce_Payload[12];                                               //Create char array for payload.
    char Cluster[2] = {0x00, 0x13};                                      //Create char array for Cluster ID.
    char Profile[2] = {0x00, 0x00};                                      //Create char array for Profile ID.
    Anncounce_Payload[0] = 0x22;                                              //Set seqence ID to 0x22.
    Anncounce_Payload[1] = Address[1];                                            //Copy 16bit PAN address little endian.
    Anncounce_Payload[2] = Address[0];
    Anncounce_Payload[3] = Address[9];                                           //Copy 64bit address little endian.
    Anncounce_Payload[4] = Address[8];
    Anncounce_Payload[5] = Address[7];
    Anncounce_Payload[6] = Address[6];
    Anncounce_Payload[7] = Address[5];
    Anncounce_Payload[8] = Address[4];
    Anncounce_Payload[9] = Address[3];
    Anncounce_Payload[10] = Address[2]; 
    Anncounce_Payload[11] = 0x8C;                                             //Set device parameters.

    PBuild(Anncounce_Frame,0x00,0x00,Profile,Cluster, Anncounce_Payload, 12);         //Send packet to builder.
}

void Active_EP_Request(){
    char Cluster[2] = {0x80, 0x05};                                    //Create char array for Cluster.
    char Profile[2] = {0x00, 0x00};                                    //Create char array for Profile.
    char Buffer[64];
    char ST_Address[10];

    for(int x = 0; x < 10 ; x ++){
      ST_Address[x] =  Packet[x+4];
    }
    memset(Buffer, 0, sizeof(Buffer));                                  //Clear array.
                                                                           
    Buffer[0] = Packet[21];                                      //Set Transcation Seq number to match inbound packets seq number.
    Buffer[1] = 0x00;                                                   //Status $00 = success Table 2.93 on page 159 of ZBSpec.
    Buffer[2] = Packet[22];                                      //Set Network address little endian order.
    Buffer[3] = Packet[23];
    Buffer[4] = 0x01;                                                   //Active end point count in this case 1.
    Buffer[5] = 0x76;                                                   //Endpoint 38.
    PBuild(ST_Address,0x00,0x00,Profile,Cluster, Buffer, 0x06); 
  
}

void Simple_Description(){
      
    char End_Point = Packet[24];                                             //Get endpoint from payload.
    char Cluster[2] = {0x80,0x04};                                      //Create char array for cluster ID.
    char Profile[2] = {0x00,0x00};                                      //Create char array for profile ID.
    char Buffer[64];
    char ST_Address[10];

    for(int x = 0; x < 10 ; x ++){
      ST_Address[x] =  Packet[x+4];
    }
    memset(Buffer, 0, sizeof(Buffer));

    if( End_Point == 0x76){                                                    //If Endpoint = 38.                                                                            
        Buffer[0] = Packet[21];                                  //Set Transcation Seq number to match inbound packets seq number
        Buffer[1] = 0x00;                                               //Status $00 = success Table 2.93 on page 159 of ZBSpec 
        Buffer[2] = Packet[22];                                  //Set Network address little endian order
        Buffer[3] = Packet[23]; 
        Buffer[4] = 0x0E;                                               //Length in bytes of the Simple Descriptor to Follow
        Buffer[5] = 0x76;                                               //Endpoint of the simple descriptor Table 2.38 on page 88 of ZBSpec
        Buffer[6] = 0x04;                                               //Application Profile ID 2 Bytes Little endian. $0104 = Home Automation Profile
        Buffer[7] = 0x01;
        Buffer[8] = 0x02;                                               //Device type 2 Bytes Little endian, $0002 = On/Off Output see page 42 of ZigBee Home Automation Profile
        Buffer[9] = 0x00;                                                             
        Buffer[10] = 0x00;                                              //App Dev Version 4bits + reserved 4bits  
        Buffer[11] = 0x02;                                              //Input cluster count in this case we only have $02 input clusters
        Buffer[12] = 0x00;                                              //Input cluster list 2 bytes each little endian. $0000 = Basic Cluster 
        Buffer[13] = 0x00;
        Buffer[14] = 0x06;                                              //Output cluster 2 bytes each little endian. $0006 = On / Off Cluster
        Buffer[15] = 0x00;
        Buffer[16] = 0x00;                                              //Output cluster list. No output cluster.
        PBuild(ST_Address,0x00,0x00,Profile,Cluster, Buffer, 0x11);
    }
    
    else{                                                                              
        Buffer[0] = Packet[21];                                  //Set Transcation Seq number to match inbound packets seq number.
        Buffer[1] = 0x82;                                               //Status $82 = Invalid_EP page 212 of ZigBee Specification.  
        Buffer[2] = Packet[22];                                  //Set Network address little endian order.
        Buffer[3] = Packet[23];
        Buffer[4] = 0x00;                                               //Length in bytes of simple descriptor to follow.
        PBuild(ST_Address,0x00,0x00,Profile,Cluster, Buffer, 0x05); 
    }
}

void Cluster_Basic(){
      
    char Command_ID = Packet[23];                                                          //Set command ID to 0.
    char Sequence_Number = Packet[22];                                                     //Set seqence number to 0.
    char Attribute_ID[2] = {Packet[24],Packet[25]};
    char Cluster[2] = {Packet[16],Packet[17]};
    char Profile[2] = {Packet[18],Packet[19]};
    char Buffer[64];
    char ST_Address[10];
    char Manufacturer[7] = {'A','R','D','U','I','N','O'};                                            //Product manufacturer
    char Device[3] = {'U','N','O'};    //Device Name

    for(int x = 0; x < 10 ; x ++){
      ST_Address[x] =  Packet[x+4];
    }
    memset(Buffer, 0, sizeof(Buffer));                                  //Clear array.
    
    
    if ((Command_ID == 0x00) && (Attribute_ID[0] == 0x01) && (Attribute_ID[1] == 0x00)){     //If Application version is requested.
        Buffer[0] = 0x18;                                               //Frame control direction is server to client.
        Buffer[1] = Sequence_Number;                                             //Reply with seqence number from request.
        Buffer[2] = 0x01;                                               //Command indetifier = 1, Read attribute response.
        Buffer[3] = 0x01;                                               //Attribute Identfier (2 bytes) field being reported.
        Buffer[4] = 0x00;       
        Buffer[5] = 0x00;                                               //status 00 success.
        Buffer[6] = 0x20;                                               //Attribute data type 0x20 = unsigned 8 bit integer.
        Buffer[7] = AppV;                                               //Application version.
        PBuild(ST_Address,Packet[15],Packet[14],Profile,Cluster, Buffer, 0x08);               //Send data to packet builder.
        } 
        
    if ((Command_ID == 0x00) && (Attribute_ID[0] == 0x03) && (Attribute_ID[1] == 0x00)){     //If Hardware version is requeted.
        Buffer[0] = 0x18;                                               //Frame control direction is server to client.
        Buffer[1] = Sequence_Number;                                             //Reply with seqence number from request.
        Buffer[2] = 0x01;                                               //Command indetifier = 1, Read attribute response.
        Buffer[3] = 0x03;                                               //Attribute Identfier (2 bytes) field being reported.
        Buffer[4] = 0x00;
        Buffer[5] = 0x00;                                               //status 00 success.
        Buffer[6] = 0x20;                                               //Attribute data type 0x20 = unsigned 8 bit integer.
        Buffer[7] = HarV;                                               //Hardware version.
        PBuild(ST_Address,Packet[15],Packet[14],Profile,Cluster, Buffer, 0x08);               //Send data to packet builder.
        }

    if ((Command_ID == 0x00) && (Attribute_ID[0] == 0x04) && (Attribute_ID[1] == 0x00)){     //If device Manufacturer is requested
        Buffer[0] = 0x18;                                               //Frame control direction is server to client.
        Buffer[1] = Sequence_Number;                                             //Reply with seqence number from request.
        Buffer[2] = 0x01;                                               //Command indetifier = 1, Read attribute response.
        Buffer[3] = 0x04;                                               //Attribute Identfier (2 bytes) field being reported.
        Buffer[4] = 0x00;
        Buffer[5] = 0x00;                                               //status 00 success.
        Buffer[6] = 0x42;                                               //Attribute data type 0x42 = character string.
        Buffer[7] = sizeof(Manufacturer);                                        //Size of string to follow.
        int x = 0;
        while( x < (sizeof(Manufacturer))){                                      //Send sting byte by byte.
            Buffer[(x+8)] = Manufacturer[x];
            x++;
            }
        PBuild(ST_Address,Packet[15],Packet[14],Profile,Cluster, Buffer, (0x08 + Buffer[7])); //Send data to packet builder.
        }
        
    if ((Command_ID == 0x00) && (Attribute_ID[0] == 0x05) && (Attribute_ID[1] == 0x00)){     //If Device devoloper is requested.
        Buffer[0] = 0x18;                                               //Frame control direction is server to client.
        Buffer[1] = Sequence_Number;                                             //Reply with seqence number from request.
        Buffer[2] = 0x01;                                               //Command indetifier = 1, Read attribute response.
        Buffer[3] = 0x05;                                               //Attribute Identfier (2 bytes) field being reported.
        Buffer[4] = 0x00;
        Buffer[5] = 0x00;                                               //status 00 success.
        Buffer[6] = 0x42;                                               //Attribute data type 0x42 = character string.
        Buffer[7] = sizeof(Device) ;                                       //Size of string to follow.
        int x = 0;
        while( x < (sizeof(Device))){                                      //Send sting byte by byte.
            Buffer[(x+8)] = Device[x];
            x++;
            }
        PBuild(ST_Address,Packet[15],Packet[14],Profile,Cluster, Buffer, (0x08 + Buffer[7])); //Send data to packet builder.
        }   
}

void On_Off(){

    Serial.print("on/off");
    char Command_ID = Packet[23];                                                          //Set command ID to 0.
    char Sequence_Number = Packet[22];                                                     //Set seqence number to 0.
    char Frame_Type = Packet[21];                   //Get frame type from payload.
    Frame_Type = Frame_Type & 0x03;                                     //Bitwise & with 0x03 to make sure you are looking at the first 2 bits.
    char Attribute_ID[2] = {Packet[24],Packet[25]};
    char Cluster[2] = {Packet[16],Packet[17]};
    char Profile[2] = {Packet[18],Packet[19]};
    char Buffer[64];
    char ST_Address[10];

    for(int x = 0; x < 10 ; x ++){
      ST_Address[x] =  Packet[x+4];
    }
    
    memset(Buffer, 0, sizeof(Buffer));                                  //Clear array.

    if ((Frame_Type == 0x00) && (Command_ID == 0x00) && (Attribute_ID[1] == 0x00)){     //If Command to check status is sent.
      Serial.print("read");
      Serial.print(digitalRead(13));
      Buffer[0] = 0x18;                                                   //Frame control direction is server to client.
      Buffer[1] = Sequence_Number;                                                 //Reply with seqence number from request.
      Buffer[2] = 0x01;                                                   //Command indetifier = 1, Read attribute response.
      Buffer[3] = 0x00;                                                   //Attribute Identfier (2 bytes) field being reported.
      Buffer[4] = 0x00;
      Buffer[5] = 0x00;                                                   //Status 00 = success.
      Buffer[6] = 0x10;                                                   //Attribute data type 10 = boolean.
      Buffer[7] = digitalRead(13);                                                    //Read LED state.
      PBuild(ST_Address,Packet[15],Packet[14],Profile,Cluster, Buffer, 0x08);
    }
    
    if ((Frame_Type == 0x01) && (Command_ID == 0x00) && (Attribute_ID[1] == 0x00)){     //command to turn off LED.
      Serial.print("LED OFF");
      digitalWrite(13, LOW);                                                            //Turn LED off.
      Buffer[0] = 0x18;                                                   //Frame control direction is server to client.
      Buffer[1] = Sequence_Number;                                                 //Reply with seqence number from request.
      Buffer[2] = 0x0B;                                                   //Command identifier 0x08 = default response.
      Buffer[3] = Command_ID;                                                  //reply with the command ID sne to you.
      Buffer[4] = 0x00;                                                   //Send sucess 00
      PBuild(ST_Address,Packet[15],Packet[14],Profile,Cluster, Buffer, 0x05);                   //Send to packet builder.
    }
    
    if ((Frame_Type == 0x01) && (Command_ID == 0x01) && (Attribute_ID[1] == 0x00)){     //Command to turn on LED.
      Serial.print("LED ON");
      digitalWrite(13, HIGH);                                                       //Turn LED on.
      Buffer[0] = 0x18;                                                   //Frame control direction is server to client.
      Buffer[1] = Sequence_Number;                                                 //Reply with seqence number from request.
      Buffer[2] = 0x0B;                                                   //Command identifier 0x08 = default response.
      Buffer[3] = Command_ID;                                                  //reply with the command ID sne to you.
      Buffer[4] = 0x00;                                                   //Send sucess 00
      PBuild(ST_Address,Packet[15],Packet[14],Profile,Cluster, Buffer, 0x05);                   //Send to packet builder.
    }
    
    if ((Frame_Type == 0x01) && (Command_ID == 0x02) && (Attribute_ID[1] == 0x00)){     //Command to toggle LED.
      Serial.print("LED Toggle");
      digitalWrite(13, HIGH);                                                     //Toggle LED
      Buffer[0] = 0x18;                                                   //Frame control direction is server to client.
      Buffer[1] = Sequence_Number;                                                 //Reply with seqence number from request.
      Buffer[2] = 0x0B;                                                   //Command identifier 0x08 = default response.
      Buffer[3] = Command_ID;                                                  //reply with the command ID sne to you.
      Buffer[4] = 0x00;                                                   //Send sucess 00
      PBuild(ST_Address,Packet[15],Packet[14],Profile,Cluster, Buffer, 0x05);                   //Send to packet builder.
    }
}

void Packet_Send(char API_Packet[]){
  int y = 0;
  int Packet_Length;
    
  Packet_Length = (API_Packet[2] + 4);                                       //Calaculate packet length using packet length identifier and add the start byte, 2 byte length and checksum.
  while (y < Packet_Length){                                                 //Send the command
    Serial.print(API_Packet[y]);
    delay(10);
    y ++;
  }
  /*y = 0;
  Packet_Length = (API_Packet[2] + 4);                                       //Calaculate packet length using packet length identifier and add the start byte, 2 byte length and checksum.
  while (y < Packet_Length){                                                 //Send the command
    Serial.print(API_Packet[y],HEX);
    y ++;
  }*/
}

