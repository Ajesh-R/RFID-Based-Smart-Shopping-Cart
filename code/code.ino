#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>
#include <RtcDS1307.h>
#include <ESP8266WiFi.h>

RtcDS1307<TwoWire> rtc(Wire);
int Hor;
int Min;
int Sec;

WiFiServer server(80);

String header;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

// Replace with your network credentials
const char* ssid = "Galaxy";
const char* password = "ajesh@123";

// set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 2;

int pro = 0;
int total = 0; // Initialize total

// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

// Rfid 
#define RST_PIN D3
#define SS_PIN D4

byte readCard[4];
String tagID = "";
MFRC522 mfrc522(SS_PIN, RST_PIN);
String addedp[10];
int addedprice[10];

boolean getID();
int PRODUCTprice(String ckword); // Correct function signature
String PRODUCTname(String ckword); // Correct function signature
void checkPDinlist();

void setup() {
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  lcd.print(WiFi.localIP());
  delay(1000);
  lcd.clear();
  server.begin();

  pinMode(D8, OUTPUT); 
  Serial.begin(115200);  //Starts serial connection
  rtc.Begin();     //Starts I2C

  SPI.begin();
  mfrc522.PCD_Init();

  lcd.init();
  lcd.backlight();
  lcd.setCursor(2, 0);
  lcd.print("WELCOME");
  lcd.setCursor(0, 1);
  lcd.print("HAPPY SHOPPING");
  delay(3000);
  lcd.clear();
  Serial.begin(9600);
  lcd.clear();
}

void loop() {
  RtcDateTime now = rtc.GetDateTime();
  Hor = now.Hour();
  Min = now.Minute();
  Sec = now.Second();
  WiFiClient client = server.available();
  lcd.setCursor(0, 0);
  lcd.print("ADD PRODUCTS");
  lcd.setCursor(0,1);
  lcd.print("total=");
  lcd.print(total);
  
  if (getID()) {
    lcd.clear();
    checkPDinlist();
    digitalWrite(D8,HIGH);
    delay(1000);
    digitalWrite(D8,LOW);
  }
  
  if (client) { 
    Serial.println("New Client."); // print a message out in the serial port
    String currentLine = ""; // make a String to hold incoming data from the client
    currentTime = millis();
    previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime) {
      currentTime = millis();
      if (client.available()) {
        char c = client.read(); // read a byte, then
        Serial.write(c); // print it out the serial monitor
        header += c;
        if (c == '\n') { // if the byte is a newline character
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #77878A;}</style></head>");
            client.println("<body><h1>ESP8266 Web Server</h1>");
            client.println("<table>");
            client.println("<tr>");
            client.println("<th> ITEM </th><th>PRICE</th>");
            client.println("</tr>");
            for(int i=0;i<10;i++){
              if (addedp[i] != "") { // Check if product name is not empty
                client.println("<tr><th>"+addedp[i]+"</th><th>"+String(addedprice[i])+"</th></tr>");
              }
            }
            client.println("</table>");
            client.println("Total: " + String(total));
            client.println("</body></html>");
            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    header = "";
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println(""); 
  }
}

boolean getID(){
  if(!mfrc522.PICC_IsNewCardPresent()) {
    return false;
  }
  if(!mfrc522.PICC_ReadCardSerial()) {
    return false;
  }
  tagID = "";
  for (uint8_t i = 0; i < 4; i++) {
    tagID.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  tagID.toUpperCase();
  mfrc522.PICC_HaltA();
  return true;
}

int PRODUCTprice(String ckword) {
  if(ckword=="234B5996") {
    return 50;
  }
  if(ckword=="432D2F96") {
    return 45;
  }
  if(ckword=="13DF1115") {
    return 40;
  }
  return 0;
}

String PRODUCTname(String ckword) {
  if(ckword=="234B5996") {
    return "AVT TEA 50RS";
  }
  if(ckword=="13DF1115") {
    return "RICE 40RS";
  }
  if(ckword=="432D2F96") {
    return "SUGAR 45RS";
  }
  if(ckword=="2C46174A") {
    lcd.setCursor(0,1);
    lcd.print("time "+String(Hor)+":"+String(Min)+":"+String(Sec));
    lcd.setCursor(0,0);
    lcd.print("total="+String(total));
     delay(60000);
      
      
    }
    return "invalid tag";
  }
  int add(int price,int total){
    total=total+price;
    return total;
    }
  int subtract(int price,int total){
      if(total<0){
        return 0;}
      total=total-price;
      return total;
      }
   void checkPDinlist(){
      for(int i=0;i<10;i++){
      if(addedp[i]==PRODUCTname(tagID))
      {addedp[i]="";
      addedprice[i]=0;
      
      total=subtract(PRODUCTprice(tagID),total);
      lcd.setCursor(0, 0);
      lcd.print(PRODUCTname(tagID));
      lcd.setCursor(0, 1);
      lcd.print("REMOVED");
      delay(1500);
    break;
      }

      else if(addedp[i]=="")
      {addedp[i]=PRODUCTname(tagID);
      addedprice[i]=PRODUCTprice(tagID);
      total=add(PRODUCTprice(tagID),total);
      lcd.setCursor(0, 0);
      lcd.print(PRODUCTname(tagID));
      lcd.setCursor(0, 1);
      lcd.print("ADDED");
      delay(1500);
      break;
        }
}
}
