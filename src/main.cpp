#include <Arduino.h>

#include <WiFi.h>
#include <WebServer.h>
#include <DFMiniMp3.h>
#include "PageIndex.h"

#define PB_Repeat     1
#define PB_NextTrack  2

//---------------------------------------- Button Back, Next, Play/Stop
#define back_button 18
#define play_button 4
#define next_button 10

//---------------------------------------- Cấu hình chân RX/TX, giao tiếp UART2
#define RX_PIN 6 
#define TX_PIN 7

//---------------------------------------- Biến điều chỉnh âm lượng
uint16_t mp3_Volume = 20;

byte mp3_Status;
bool isPlaying = false; 

//---------------------------------------- Chọn chế độ phát
int PB_Mode = PB_NextTrack;

//---------------------------------------- Khai báo và cấu hình Access Point 
const char* ssid = "NoName"; 
const char* password = "groupNoName";

IPAddress local_ip(192,168,10,1);
IPAddress gateway(192,168,10,1);
IPAddress subnet(255,255,255,0);

//---------------------------------------- Định nghĩa dựa trên thư viện
class Mp3Notify;
typedef DFMiniMp3<HardwareSerial, Mp3Notify> DfMp3; 

DfMp3 dfmp3(Serial1); // Giao tiếp qua UART2

WebServer server(80); // Server trên cổng 80.

//________________________________________________________________________________
// Xử lý khi mở địa chỉ IP trên trình duyệt
void handleRoot() {
  server.send(200, "text/html", MAIN_page); //Gửi trang web
}

void server_Send(String msg) {
  server.send(200, "text/plane", msg); // Gửi phản hồi đến client
}

//________________________________________________________________________________
// Xử lý các lệnh đến từ clients
void handleCommands() {
  String incoming_Command;
  
  incoming_Command = server.arg("PLAYERCMD");

  Serial.println();
  Serial.println("------------PLAYERCMD");
  Serial.print("Incoming command : ");
  Serial.println(incoming_Command);

  if (incoming_Command == "PY") {
    dfmp3.start();  
    Serial.println("Start playing mp3."); 
    delay(500);
    
    DfMp3_Status status = dfmp3.getStatus();
    mp3_Status = status.state;
    server_Send("Sta," + String(mp3_Status));
    delay(500);
  
  } else if (incoming_Command == "PE") {
    dfmp3.pause();
    Serial.println("Pause playing mp3.");
    delay(500);
    
    DfMp3_Status status = dfmp3.getStatus();
    mp3_Status = status.state;
    server_Send("Sta," + String(mp3_Status));
    delay(500);

  } else if (incoming_Command == "PV") {
    dfmp3.prevTrack(); 
    Serial.println("Play previous mp3."); 
    delay(500);

  } else if (incoming_Command == "NT") {
    dfmp3.nextTrack();
    Serial.println("Play next mp3.");
    delay(500);

  } else if (incoming_Command == "VD") {
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

  } else if (incoming_Command == "VU") {
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

  } else if (incoming_Command == "RP") {
    PB_Mode = PB_Repeat;
    Serial.println("Repeat Song.");
    delay(500);

  } else if (incoming_Command == "NRP") {
    PB_Mode = PB_NextTrack;
    Serial.println("Turn off Repeat.");
    delay(500);
  } 

  else if (incoming_Command == "8"){dfmp3.playGlobalTrack(1); delay(500);}
  else if (incoming_Command == "1"){dfmp3.playGlobalTrack(2); delay(500);}
  else if (incoming_Command == "2"){dfmp3.playGlobalTrack(3); delay(500);}
  else if (incoming_Command == "3"){dfmp3.playGlobalTrack(4); delay(500);}
  else if (incoming_Command == "4"){dfmp3.playGlobalTrack(5); delay(500);}
  else if (incoming_Command == "5"){dfmp3.playGlobalTrack(6); delay(500);}
  else if (incoming_Command == "6"){dfmp3.playGlobalTrack(7); delay(500);}
  else if (incoming_Command == "7"){dfmp3.playGlobalTrack(8); delay(500);}

  else if (incoming_Command == "Get") {
    server_Send("Data," + String(mp3_Status) + "," + String(mp3_Volume));
    delay(500);
  }

  Serial.println("------------");
  
  server_Send("+OK");
  delay(500);
}
 
//________________________________________________________________________________
// Kiểm tra trạng thái của mô-đun DFPlayer để in ra thông báo ở Serial Monitor
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
// Xử lý khi có vòng lặp với delays, nó cho phép thông báo được xử lý mà không cần ngắt
void waitMilliseconds(uint16_t msWait) {
  uint32_t start = millis();
  
  while ((millis() - start) < msWait)
  {
    dfmp3.loop(); 
    delay(1);
  }
}

//________________________________________________________________________________
// Thông tin ban đầu
void get_Information() {
  Serial.println();
  Serial.println("-----------Information from the DFPlayer module.");
  
  DfMp3_Status status = dfmp3.getStatus();
  mp3_Status = status.state;
  Serial.print("Status : ");
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

  Serial.println("Initializing DFPlayer Mini...");
  
  dfmp3.begin();
  dfmp3.reset();
  dfmp3.setVolume(mp3_Volume);

  Serial.println("Initialization is complete.");

  get_Information();

//---------------------------------------- Đặt Wifi ở chế độ Access Point
  Serial.println();
  Serial.println("-------------");
  Serial.println("WIFI mode : AP");
  WiFi.mode(WIFI_AP);
  Serial.println("-------------");
  delay(1500);

//----------------------------------------Setting ESP32 trở thành một Access Point.
  Serial.println();
  Serial.println("-------------");
  Serial.println("Setting up ESP32 to be an Access Point.");
  WiFi.softAP(ssid, password); // Tạo Access Point
  delay(1500);
  Serial.println("Setting up ESP32 softAPConfig.");
  WiFi.softAPConfig(local_ip, gateway, subnet);
  Serial.println("-------------");
  delay(1500);

//---------------------------------------- Setting server
  Serial.println();
  Serial.println("-------------");
  Serial.println("Setting the server.");
  Serial.println("-------------");
  server.on("/", handleRoot); 
  server.on("/setPLAYER", handleCommands);
  delay(1500);

//---------------------------------------- Bắt đầu server
  server.begin(); 
  Serial.println();
  Serial.println("HTTP server started");
  delay(1500);
  
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
  delay(1000);
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
    if (isPlaying) {
      Serial.println("Pause");
      dfmp3.pause();
      isPlaying = false;  
    } 
    else {
      Serial.println("Play");
      dfmp3.start();
      isPlaying = true; 
    }
  }

  // Xử lí yêu cầu Client
  server.handleClient();
  
  waitMilliseconds(100);
}