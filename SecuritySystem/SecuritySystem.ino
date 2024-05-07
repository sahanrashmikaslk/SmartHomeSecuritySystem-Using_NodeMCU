#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>


LiquidCrystal_I2C lcd(0x27, 16, 2);
const char* ssid = "iPhone1234";
const char* password = "123sahan";

#define BOTtoken "7051557108:AAHocrT0kjve7kXoDzublyNarECLt7bzVaU"
#define CHAT_ID "1238463583"

#define Sensor D0
#define Buzzer D5


X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

const unsigned long BOT_MTBS = 1000;
const int ledPin = LED_BUILTIN;
int ledStatus = 0;
int sensorStatus = 0;
unsigned long bot_lasttime;

void handleNewMessages(int numNewMessages) {
  Serial.print("handleNewMessages ");
  Serial.println(numNewMessages);

  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;

    String from_name = bot.messages[i].from_name;
    if (from_name == "")
      from_name = "Guest";

    if (text == "/lightOn") {
      digitalWrite(ledPin, HIGH);  // turn the LED on (HIGH is the voltage level)
      ledStatus = 1;
      bot.sendMessage(chat_id, "Turning on the light", "");
    }

    if (text == "/lightOff") {
      ledStatus = 0;
      digitalWrite(ledPin, LOW);  // turn the LED off (LOW is the voltage level)
      bot.sendMessage(chat_id, "Turning off the light", "");
    }

    if (text == "/lightStatus") {
      if (ledStatus) {
        bot.sendMessage(chat_id, "Light is ON", "");
      } else {
        bot.sendMessage(chat_id, "Light is OFF", "");
      }
    }


    if (text == "/sensorOn") {
      sensorStatus = 1;
      bot.sendMessage(chat_id, "Turning on your Security System", "");
    }

    if (text == "/sensorOff") {
      sensorStatus = 0;
      bot.sendMessage(chat_id, "Turning off your Security System ", "");
    }

    if (text == "/sensorStatus") {
      if (sensorStatus) {
        bot.sendMessage(chat_id, "Security System is ON", "");
      } else {
        bot.sendMessage(chat_id, "Security System is OFF", "");
      }
    }

    if (text == "/start") {
      String welcome = "Welcome to MINT Sparks IOT solutions 🌐, " + from_name + "!.\n";
      welcome += "This is MINT Spark Bot 🤖\n\n";
      welcome += "/sensorOn : to switch ON your Security System\n";
      welcome += "/sensorOff : to switch OFF your Security System\n";
      welcome += "/sensorStatus : Returns current status of the Light\n\n";
      welcome += "/lightOn : to switch the Light ON\n";
      welcome += "/lightOff : to switch the Light OFF\n";
      welcome += "/lightstatus : Returns current status of the Light\n";
      bot.sendMessage(chat_id, welcome, "Markdown");
    }
  }
}

void welcomeText() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("MINT Sparks :)");
  delay(1500);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("MINT Sparks ;)");
  delay(1500);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("MINT Sparks :)");
  delay(1000);
}
void responseScreen() {

  String welcome = "Should I turn on your Security system?🚨.\n\n";
  welcome += "/sensorOn : to switch ON\n";
  welcome += "/sensorOff : to switch OFF\n";
  bot.sendMessage(CHAT_ID, welcome, "Markdown");
}


void replyScreen() {

  String welcome = "Should I turn on the light?💡.\n\n";
  welcome += "/lightOn : to switch the Led ON\n";
  welcome += "/lightOff : to switch the Led OFF\n";

  welcome += "/lightStatus : Returns current status of LED\n";
  bot.sendMessage(CHAT_ID, welcome, "Markdown");
}

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  Wire.begin(D2, D1);
  configTime(0, 0, "pool.ntp.org");  // get UTC time via NTP
  client.setTrustAnchors(&cert);     // Add root certificate for api.telegram.org

  pinMode(Sensor, INPUT);
//  pinMode(LEDR, OUTPUT);
//pinMode(LEDG, OUTPUT);
  pinMode(Buzzer, OUTPUT);

 // digitalWrite(LEDR, LOW);
  //digitalWrite(LEDG, LOW);
  //digitalWrite(LEDWIFI, LOW); 

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  //welcome text
  welcomeText();


  int a = 16;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    lcd.setCursor(a, 0);
    lcd.print(".");
    delay(1000);
    a++;
  }

  Serial.println("");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi connected");
  delay(1500);
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());
  Serial.println(WiFi.localIP());
  delay(1500);

  bot.sendMessage(CHAT_ID, "Welcome to MINT Sparks IOT solutions,  \n\nWiFi Connected! 🌐", "");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("System started");
  delay(1000);
  responseScreen();
}

void loop() {
  while (!sensorStatus) {
    digitalWrite(Buzzer, LOW);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Standby Mode");
    if (millis() - bot_lasttime > BOT_MTBS) {
      int numNewMessages = bot.getUpdates(bot.last_message_received + 1);


      while (numNewMessages) {
        Serial.println("got response");
        handleNewMessages(numNewMessages);
        numNewMessages = bot.getUpdates(bot.last_message_received + 1);
      }

      bot_lasttime = millis();
    }
  }
  bool readValue = digitalRead(Sensor);
  bool value = !readValue;
  Serial.println(value);
  if (value == 1) {
    Serial.println("Motion Detected");
    digitalWrite(Buzzer, HIGH);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Motion Detected");
    bot.sendMessage(CHAT_ID, "Motion detected!!", "");
    replyScreen();
   // delay(1500);

  } else if (value == 0) {
    digitalWrite(Buzzer, LOW);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("No Motion    ");
  }
  if (millis() - bot_lasttime > BOT_MTBS) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    bot_lasttime = millis();
  }
}