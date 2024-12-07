#include <Arduino.h>

#include <WiFi.h>
#include <WebServer.h>
#include <DFMiniMp3.h>
#include "PageIndex.h"

//---------------------------------------- Defines the playback mode.
#define PB_Normal     0
#define PB_Repeat     1
#define PB_NextTrack  2

//---------------------------------------- Button Back, Next, Play/Stop
#define back_button 18
#define play_button 4
#define next_button 10

//---------------------------------------- Cấu hình chân RX/TX, giao tiếp  UART2
#define RX_PIN 6 // Chân RX kết nối với TX của DFPlayer Mini
#define TX_PIN 7 // Chân TX kết nối với RX của DFPlayer Mini

//---------------------------------------- Biến điều chỉnh âm lượng
uint16_t mp3_Volume = 20;

byte mp3_Status;
bool isPlaying = false; // Biến lưu trạng thái của nhạc, mặc định là không phát nhạc

//----------------------------------------  Select playback mode.
// The playback mode here is not set to the DFPlayer module.
// Playback mode here is just a command that is executed after the mp3 has finished playing (see "static void OnPlayFinished").
// PB_Normal : after finishing playing the mp3, the module stops playing.
// PB_Repeat: after finishing playing the mp3, the module will repeat playing the same or last mp3 file.
// PB_NextTrack : after finishing playing the mp3, the module will play the next mp3.

//int PB_Mode = PB_Normal;
// int PB_Mode = PB_Repeat;
int PB_Mode = PB_NextTrack;

// If you want to set the playback mode to the DFPlayer module, use the command:
// dfmp3.setPlaybackMode(mode).
// mode (choose one): DfMp3_PlaybackMode_Repeat, DfMp3_PlaybackMode_FolderRepeat, DfMp3_PlaybackMode_SingleRepeat, DfMp3_PlaybackMode_Random.
// Example : dfmp3.setPlaybackMode(DfMp3_PlaybackMode_SingleRepeat).

//---------------------------------------- Khai báo và cấu hình Access Point 
const char* ssid = "NoName"; 
const char* password = "groupNoName";

IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

//---------------------------------------- forward declare the notify class, just the name.
class Mp3Notify;
// define a handy type using serial and our notify class.
typedef DFMiniMp3<HardwareSerial, Mp3Notify> DfMp3; 

// instance a DfMp3 object.
DfMp3 dfmp3(Serial1);

// Server on port 80.
WebServer server(80);

//________________________________________________________________________________
// This routine is executed when you open ESP32 IP Address in browser.
void handleRoot() {
  // server.sendHeader("Content-Type", "text/html; charset=UTF-8");
  server.send(200, "text/html", MAIN_page); //Send web page
}

//________________________________________________________________________________
void server_Send(String msg) {
  // server.sendHeader("Content-Type", "text/html; charset=UTF-8");
  server.send(200, "text/plane", msg); // Send replies to the client.
}

//________________________________________________________________________________
// Subroutines for handling incoming commands from clients.
void handleCommands() {
  String incoming_Command;
  
  incoming_Command = server.arg("PLAYERCMD");

  Serial.println();
  Serial.println("------------PLAYERCMD");
  Serial.print("Incoming command : ");
  Serial.println(incoming_Command);

  if (incoming_Command == "PE") {
    // Pause playing mp3.
    dfmp3.pause();
    Serial.println("Pause playing mp3.");
    delay(500);
    
    DfMp3_Status status = dfmp3.getStatus();
    mp3_Status = status.state;
    server_Send("Sta," + String(mp3_Status));
    delay(500);

  } else if (incoming_Command == "VD") {
    // Decrease the volume.
    mp3_Volume = mp3_Volume - 2;
    if (mp3_Volume < 0) mp3_Volume = 0;
    dfmp3.setVolume(mp3_Volume);
    Serial.println("Decrease the volume.");
    Serial.print("Volume = ");
    Serial.println(mp3_Volume);
    delay(500);
    
    uint16_t volume = dfmp3.getVolume();
    server_Send("Vlm," + String(volume));
    delay(500);

  } else if (incoming_Command == "PV") {
    // Play previous mp3.
    dfmp3.prevTrack(); 
    Serial.println("Play previous mp3."); 
    delay(1000);

  } else if (incoming_Command == "PY") {
    // Start playing mp3.
    dfmp3.start();  
    Serial.println("Start playing mp3."); 
    delay(500);
    
    DfMp3_Status status = dfmp3.getStatus();
    mp3_Status = status.state;
    server_Send("Sta," + String(mp3_Status));
    delay(500);

  } else if (incoming_Command == "NT") {
    // Play next mp3.
    dfmp3.nextTrack();
    Serial.println("Play next mp3.");
    delay(1000);

  } else if (incoming_Command == "VU") {
    // Increase the volume.
    mp3_Volume = mp3_Volume + 2;
    if (mp3_Volume > 30) mp3_Volume = 30;
    dfmp3.setVolume(mp3_Volume);
    Serial.println("Increase the volume.");
    Serial.print("Volume = ");
    Serial.println(mp3_Volume);
    delay(500);
    
    uint16_t volume = dfmp3.getVolume();
    server_Send("Vlm," + String(volume));
    delay(500);

  } else if (incoming_Command == "SP") {
    // Stop playing mp3.
    dfmp3.stop();  
    Serial.println("Stop playing mp3.");
    delay(500);
    
    DfMp3_Status status = dfmp3.getStatus();
    mp3_Status = status.state;
    server_Send("Sta," + String(mp3_Status));
    delay(500);

  } else if (incoming_Command == "RP") {
    PB_Mode = PB_Repeat;
    // dfmp3.setPlaybackMode(DfMp3_PlaybackMode_SingleRepeat);
    Serial.println("Repeat Song.");
    delay(1000);

  } else if (incoming_Command == "NRP") {
    PB_Mode = PB_NextTrack;
    Serial.println("Turn off Repeat.");
    delay(1000);
  } 

  else if (incoming_Command == "8"){dfmp3.playGlobalTrack(1); delay(1000);}
  else if (incoming_Command == "1"){dfmp3.playGlobalTrack(2); delay(1000);}
  else if (incoming_Command == "2"){dfmp3.playGlobalTrack(3); delay(1000);}
  else if (incoming_Command == "3"){dfmp3.playGlobalTrack(4); delay(1000);}
  else if (incoming_Command == "4"){dfmp3.playGlobalTrack(5); delay(1000);}
  else if (incoming_Command == "5"){dfmp3.playGlobalTrack(6); delay(1000);}
  else if (incoming_Command == "6"){dfmp3.playGlobalTrack(7); delay(1000);}
  else if (incoming_Command == "7"){dfmp3.playGlobalTrack(8); delay(1000);}

  else if (incoming_Command == "Get") {
    server_Send("Data," + String(mp3_Status) + "," + String(mp3_Volume));
    delay(500);
  }

  Serial.println("------------");
  
  server_Send("+OK");
  delay(500);
}
 
//________________________________________________________________________________
class Mp3Notify {
public:
  static void PrintlnSourceAction(DfMp3_PlaySources source, const char* action) {
    Serial.println();
    if (source & DfMp3_PlaySources_Sd) {
        Serial.print("SD Card, ");
    }
    if (source & DfMp3_PlaySources_Usb) {
        Serial.print("USB Disk, ");
    }
    if (source & DfMp3_PlaySources_Flash) {
        Serial.print("Flash, ");
    }
    Serial.println(action);
  }

  static void OnError([[maybe_unused]] DfMp3& mp3, uint16_t errorCode) {
    // see DfMp3_Error for code meaning.
    Serial.println();
    Serial.print("Com Error ");
    Serial.println(errorCode);
  }

  static void OnPlayFinished([[maybe_unused]] DfMp3& mp3, [[maybe_unused]] DfMp3_PlaySources source, uint16_t track) {
    Serial.println();
    Serial.print("Play finished for #");
    Serial.println(track);
    if (PB_Mode == PB_Repeat) {
      dfmp3.playGlobalTrack(track);
      dfmp3.setPlaybackMode(DfMp3_PlaybackMode_SingleRepeat);
      dfmp3.playGlobalTrack(track);
    }
    if (PB_Mode == PB_NextTrack) dfmp3.nextTrack();
  }

  static void OnPlaySourceOnline([[maybe_unused]] DfMp3& mp3, DfMp3_PlaySources source)
  { PrintlnSourceAction(source, "online"); }

  static void OnPlaySourceInserted([[maybe_unused]] DfMp3& mp3, DfMp3_PlaySources source)
  { PrintlnSourceAction(source, "inserted"); }
  
  static void OnPlaySourceRemoved([[maybe_unused]] DfMp3& mp3, DfMp3_PlaySources source)
  { PrintlnSourceAction(source, "removed"); }
};

//________________________________________________________________________________
// Subroutine to check the status of the DFPlayer module so that after it finishes playing the mp3,
// a notification will be printed in the serial communication (Serial monitor).
void waitMilliseconds(uint16_t msWait) {
  uint32_t start = millis();
  
  while ((millis() - start) < msWait)
  {
    // if you have loops with delays, its important to 
    // call dfmp3.loop() periodically so it allows for notifications 
    // to be handled without interrupts
    dfmp3.loop(); 
    delay(1);
  }
}

//________________________________________________________________________________
void get_Information() {
  Serial.println();
  Serial.println("-----------Information from the DFPlayer module.");
  
  DfMp3_Status status = dfmp3.getStatus();
  mp3_Status = status.state;
  Serial.print("status : ");
  Serial.print(mp3_Status);
  Serial.println(" (0:Stop,1:Playing,2:Pause.)");

  uint16_t volume = dfmp3.getVolume();
  mp3_Volume = volume;
  Serial.print("volume : ");
  Serial.println(mp3_Volume);
  
  uint16_t count = dfmp3.getTotalTrackCount(DfMp3_PlaySource_Sd);
  Serial.print("files : ");
  Serial.println(count);
  Serial.println("-----------");
}

//________________________________________________________________________________ SETUP
void setup() {  
  pinMode(back_button, INPUT_PULLUP);
  pinMode(next_button, INPUT_PULLUP);
  pinMode(play_button, INPUT_PULLUP);

  Serial.begin(115200);                            // Giao tiếp với Serial Monitor
  Serial1.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN); // Giao tiếp với DFPlayer Mini

  //Serial.println();
  Serial.println("Initializing DFPlayer Mini...");
  
  dfmp3.begin();
  dfmp3.reset();
  dfmp3.setVolume(mp3_Volume);

  // Serial.println();
  Serial.println("Initialization is complete.");

  get_Information();

//---------------------------------------- Set Wifi to Access Point mode
  Serial.println();
  Serial.println("-------------");
  Serial.println("WIFI mode : AP");
  WiFi.mode(WIFI_AP);
  Serial.println("-------------");
  delay(1000);

//----------------------------------------Setting up ESP32 to be an Access Point.
  Serial.println();
  Serial.println("-------------");
  Serial.println("Setting up ESP32 to be an Access Point.");
  WiFi.softAP(ssid, password); //--> Creating Access Points
  delay(1000);
  Serial.println("Setting up ESP32 softAPConfig.");
  WiFi.softAPConfig(local_ip, gateway, subnet);
  Serial.println("-------------");
  delay(1000);

//---------------------------------------- Setting the server.
  Serial.println();
  Serial.println("-------------");
  Serial.println("Setting the server.");
  Serial.println("-------------");
  server.on("/", handleRoot); 
  server.on("/setPLAYER", handleCommands);
  delay(1000);

//---------------------------------------- Start server.
  server.begin(); 
  Serial.println();
  Serial.println("HTTP server started");
  delay(1000);
  
//---------------------------------------- In một số thông tin ra monitor
  Serial.println();
  Serial.println("------------");
  Serial.print("SSID name : ");
  Serial.println(ssid);
  Serial.print("IP address : ");
  Serial.println(WiFi.softAPIP());
  Serial.println();
  Serial.println("Connect your computer or mobile Wifi to the SSID above.");
  Serial.println("Visit the IP Address above in your browser to open the main page.");
  Serial.println("------------");
  Serial.println();
  delay(500);

  // Nếu muốn bật nhạc lập tức (you want the DFPlayer Mini module to immediately play mp3s when the module is turned on)
  //Serial.println();
  //Serial.println("Start playing mp3.");
  //dfmp3.playGlobalTrack(1);  //--> sd:0001.mp3, sd:0002.mp3 and so on.
}

//________________________________________________________________________________ LOOP
void loop() {
  if(!digitalRead(back_button)){
    while(!digitalRead(back_button));
    Serial.println("Back");
    dfmp3.prevTrack();
  }

  if(!digitalRead(next_button)){
    while(!digitalRead(next_button));
    Serial.println("Next");
    dfmp3.nextTrack();
  }

  if(!digitalRead(play_button)){
    while(!digitalRead(play_button));
     // Nếu đang phát nhạc, thì tạm dừng
    if (isPlaying) {
      Serial.println("Pause");
      dfmp3.pause();
      isPlaying = false;  // Cập nhật trạng thái là đang tạm dừng
    } 
    // Nếu không đang phát nhạc, thì bắt đầu phát
    else {
      Serial.println("Play");
      dfmp3.start();
      isPlaying = true;  // Cập nhật trạng thái là đang phát nhạc
    }
  }

  // Handle client requests.
  server.handleClient();
  
  // Calls the waitMilliseconds() subroutine.
  waitMilliseconds(100);
}