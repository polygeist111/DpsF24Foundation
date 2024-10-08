#include <WiFi.h>
#include <WiFiUdp.h>

char ssid[] = "meshmeshmesh";    // Set your Wi-Fi SSID
char password[] = "sparkyasu";    // Set your Wi-Fi password
int status = WL_IDLE_STATUS;        // Indicator of Wi-Fi status
WiFiUDP udp;

const int udpPort = 4211;
//const int buttonPin = 21;
//const int potPin = A0;

//const int ledPin = 4;
//int ledBrightness;

const byte unityLightInputPin = 10;
const byte unityLEDPin = 11;
const byte potentiometer1Pin = A6;

const byte redPin = 5;
const byte greenPin = 26; //should be on 6, but having hardware issues
const byte bluePin = 7;

const byte value = 20;

//variables for color adjustments on potentiometer
byte hue = 0;
byte red = 0;
byte blue = 0;
byte green = 0;

//light feedback
bool poweredOn = false;

//timer
int packetBaselineMillis = 0;
int packetDelayMillis = 40;

void setup() {
    Serial.begin(115200);
    pinMode(unityLEDPin, OUTPUT);
    pinMode(unityLightInputPin, INPUT);
    pinMode(potentiometer1Pin, INPUT);

    pinMode(redPin, OUTPUT);
    pinMode(bluePin, OUTPUT);
    pinMode(greenPin, OUTPUT);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("\nWi-Fi connected.");
    udp.begin(udpPort);
}

void loop() {
  //process on/off, prototype unity feedback
  byte unityLightInput = digitalRead(unityLightInputPin);
  Serial.print(unityLightInput);
  Serial.print(" ");
  if (unityLightInput == 1 && poweredOn == false) {
    poweredOn = true;
    digitalWrite(unityLEDPin, HIGH);
  } else if (unityLightInput == 0 && poweredOn == true) {
    poweredOn = false;
    digitalWrite(unityLEDPin, LOW);
  }

  //process color
  int potIn = analogRead(potentiometer1Pin);
  hue = map(potIn, 10, 1036, 0, 255);
  Serial.print(potIn - 10);
  Serial.print(" ");
  Serial.println(hue);
  calculateColors(hue);
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue);

}

void packetHandler() {
  //S/R every 40ms
  int time = millis();
  if (time >= packetBaselineMillis + packetDelayMillis) {
    packetBaselineMillis = time;

    // 1. Send button state and potentiometer value to Unity
    int buttonState = digitalRead(unityLightInputPin);
    int potValue = analogRead(potentiometer1Pin);

    char packetBuffer[50];
    sprintf(packetBuffer, "Button: %d, Pot: %d", buttonState, potValue);

    udp.beginPacket("10.156.57.65", 4211); // Replace with Unity IP address
    udp.write(packetBuffer);
    udp.endPacket();

    // 2. Receive position data from Unity
    int packetSize = udp.parsePacket();
    if (packetSize) {
        char packetBuffer[255];
        int len = udp.read(packetBuffer, 255);
        if (len > 0) {
            packetBuffer[len] = 0;
        }

        Serial.println(packetBuffer); // Display received position data

        // Parse position data
        float posX, posY, posZ;
        sscanf(packetBuffer, "X:%f,Y:%f,Z:%f", &posX, &posY, &posZ);

        // Example of using the position values
        Serial.print("Position - X: ");
        Serial.print(posX);
        Serial.print(" Y: ");
        Serial.print(posY);
        Serial.print(" Z: ");
        Serial.println(posZ);

        // You can use posX, posY, and posZ to control servos, motors, etc.
        //ledBrightness = abs((int)posY %255);
        //analogWrite(ledPin, ledBrightness);
    }  
  }
}

//Takes target hue value and processes into which format to use
void calculateColors(int hueIn) {
  //hue = hueIn;
  //HSV to RGB conversion formula found here: https://www.rapidtables.com/convert/color/hsv-to-rgb.html
  //All code is original

  //because value and saturation are both assumed to be 100%, the math involves A LOT of 1s
  double s = 1.0;
  double v = value / 100.0;
  double C = v * s;
  double X = C * (1.0 - abs(fmod(((double)hueIn / 60.0), 2.0) - 1.0));
  //Serial.println(fmod(((double) hueIn / 60.0), 2.0));
  int m = v - C;

  //assigns arrangement of coefficients C, X, and 0 based on input hue
  //then calls writeColors with that arrangement, which will convert that to RGB values
  if (hueIn >= 0 && hueIn < 60) {
    writeColors(C, X, 0, m);
  } else if (hueIn >= 60 && hueIn < 120) {
    writeColors(X, C, 0, m);
  } else if (hueIn >= 120 && hueIn < 180) {
    writeColors(0, C, X, m);
  } else if (hueIn >= 180 && hueIn < 240) {
    writeColors(0, X, C, m);
  } else if (hueIn >= 240 && hueIn < 300) {
    writeColors(X, 0, C, m);
  } else if (hueIn >= 300 && hueIn < 360) {
    writeColors(C, 0, X, m);
  }
}

//Convert input arrangement of rgb (on a float scale of 0.0 to 1.0) and zone to int values 0-255 for that zone
void writeColors(double thisRed, double thisGreen, double thisBlue, double m) {
  //map 0.0-1.0 to 0-255
  //(thisRed + m) *= 255;
  //(thisBlue + m) *= 255;
  //(thisGreen + m) *= 255;
  thisRed = (thisRed + m) * 255;
  thisGreen = (thisGreen + m) * 255;
  thisBlue = (thisBlue + m) * 255;

  red = thisRed;
  blue = thisBlue;
  green = thisGreen;
  /*
  Serial.print(red);
  Serial.print(" ");
  Serial.print(green);
  Serial.print(" ");
  Serial.println(rgbTimer);
  */
}