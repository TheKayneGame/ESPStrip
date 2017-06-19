#include <ESP8266mDNS.h>
#define FASTLED_ALLOW_INTERRUPTS 0
#include <FastLED.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>


#define LED_PIN     2
#define NUM_LEDS    60
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];


int brightness = 255;
unsigned int port = 53686;

char packetBuffer[255];

WiFiUDP Udp;

int Datapin = 3;

int RED;
int GREEN;
int BLUE;

CRGB ledsBuff[NUM_LEDS];

MDNSResponder mdns;

void setup()
{
	// set pin modes
	pinMode(Datapin, OUTPUT);
	pinMode(LED_BUILTIN, OUTPUT);
	pinMode(14, INPUT_PULLUP);
	pinMode(0, OUTPUT);
	pinMode(2, OUTPUT);

	digitalWrite(LED_BUILTIN, LOW);
	digitalWrite(Datapin, LOW);


	// begin serial and connect to WiFi
	Serial.begin(115200);
	Serial.setDebugOutput(0);
	delay(100);
	Serial.println();
	Serial.println();

	WiFi.mode(WIFI_STA);
	WiFi.setAutoConnect(true);

	WiFi.hostname("ESPStrip");

	while (WiFi.status() != WL_CONNECTED) {
		digitalWrite(2, LOW);
		delay(250);
		Serial.print(".");
		digitalWrite(2, HIGH);
		delay(250);
	}

	analogWrite(2, 127);

	Serial.println("");
	Serial.println("WiFi connected");
	Serial.println("IP address: ");
	IPAddress IP = WiFi.localIP();
	Serial.println(IP);
	
	if (mdns.begin("ESPStrip", WiFi.localIP()))
	{
		Serial.println("RESPONDER STARTED");
	}

	FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
	FastLED.setBrightness(brightness);
	//FastLED.setMaxPowerInVoltsAndMilliamps(5, 2000);

	for (int i = 0; i <= NUM_LEDS; i++) {
		FastLED.clear();
		leds[i] = CRGB::White;
		delay(10);
		FastLED.show();
	}

	for (int i = NUM_LEDS; i >= 0; i--) {
		FastLED.clear();
		leds[i] = CRGB::White;
		delay(10);
		FastLED.show();
	}

	FastLED.clear();
	FastLED.show();
	Udp.begin(port);
}

unsigned long previousMillis = 0;
unsigned long currentMillis;
int value = 0;

void loop()
{
	
	if (digitalRead(14) == 0) {
		wifiSetup();
		Serial.println("444");
	}
	currentMillis = millis();
	
	int packetSize = Udp.parsePacket();
	if (packetSize) {
		
		//Receives Packet and sends response

		

		//interrupts();

		Serial.print("Received packet of size ");
		Serial.println(packetSize);
		Serial.print("From ");
		IPAddress remoteIp = Udp.remoteIP();
		Serial.print(remoteIp);
		Serial.print(", port ");
		Serial.println(Udp.remotePort());

		//noInterrupts();

		//Reads Packet to string
		int len = Udp.read(packetBuffer, 255);
		if (len > 0) {
			Serial.println(len);
			packetBuffer[len] = 0;
			Udp.flush();

		}

		Serial.println("Contents:");
		Serial.println(packetBuffer);
		Serial.println();

		char * temp;
		Serial.print("Splitting string \"");
		Serial.print(packetBuffer);
		Serial.println("\"");

		int r = 0;
		int b = 0;
		int g = 0;
		int begin = 0;
		int end = 0;
		int brightness = 0;
		int action = 0;
		int arg = 0;
		
		//Splitting String to Intergers
		if (temp == NULL) {
			temp = strtok(packetBuffer, ":");
			Serial.println(temp);
			r = atoi(temp);
		}

		if (temp != NULL) {
			temp = strtok(NULL, ":");
			Serial.println(temp);
			g = atoi(temp);
		}

		if (temp != NULL) {
			temp = strtok(NULL, ":");
			Serial.println(temp);
			b = atoi(temp);
		}

		if (temp != NULL) {
			temp = strtok(NULL, ":");
			Serial.println(temp);
			begin = atoi(temp);
		}

		if (temp != NULL) {
			temp = strtok(NULL, ":");
			Serial.println(temp);
			end = atoi(temp);
		}

		if (temp != NULL) {
			temp = strtok(NULL, ":");
			Serial.println(temp);
			brightness = atoi(temp);
		}

		if (temp != NULL) {
			temp = strtok(NULL, ":");
			Serial.println(temp);
			action = atoi(temp);
		}

		if (temp != NULL) {
			temp = strtok(NULL, ":");
			Serial.println(temp);
			arg = atoi(temp);
		}

		

		switch (action)
		{
		case 0:
			/*Set LED*/
			editLED(r, g, b, begin, end, brightness);
			break;
		case 1:
			/*Plays Notif Animation*/
			notifLED(r, g, b, brightness, arg);
			break;
		case 2:
			/*Supposed Broadcast Response*/
			UDPresponse("1");
			break;
		case 255:
			/*Supposed Broadcast Response*/
			UDPresponse("255");
			break;
		}
		
	}

	while (Serial.available() > 0) { Serial.read(); }
	//delayMicroseconds(200);

}

void editLED(int r, int g, int b, int begin, int end, int brightness) {
	FastLED.setBrightness(brightness);
	fill_solid(leds, NUM_LEDS, CRGB(r, g, b));
	fill_solid(ledsBuff, NUM_LEDS, CRGB(r, g, b));
	FastLED.show();

	UDPresponse("0:1");

}

void notifLED(int r, int g, int b, int brightness,int times) {
	//UDPresponse();

	int j = NUM_LEDS / 2;
	int d = 10 / times;

	int e = 0;
	while (e < times) {
		currentMillis = millis();
		
		if (currentMillis - previousMillis >= 10)
		{
			//Serial.println("e:" + e);
			previousMillis = currentMillis;
			
			/*Set To Color*/
			int j = NUM_LEDS / 2;
			int e1 = 0;
			int e1a = 0;
			while (e1 < j) {
				
				currentMillis = millis();
				if (currentMillis - previousMillis >= 10)
				{
					/*Serial.print("e1 ");
					Serial.println(e1);*/
					previousMillis = currentMillis;

					leds[e1].red = r;
					leds[e1a].red = r;

					leds[e1].green = g;
					leds[e1a].green = g;

					leds[e1].blue = b;
					leds[e1a].blue = b;

					FastLED.show();

					e1a = NUM_LEDS - e1;

					e1++;
				}


			}
			if (e1 >= j) { e1 = 0; e1a = 0; }

			

			/*Set To pColor*/
			while (e1 < j) {
				
				currentMillis = millis();
				if (currentMillis - previousMillis >= 10)
				{
					/*Serial.print("e2 ");
					Serial.println(e1);*/

					previousMillis = currentMillis;

					leds[e1].red = ledsBuff[e1].red;
					leds[e1a].red = ledsBuff[e1a].red;

					leds[e1].green = ledsBuff[e1].green;
					leds[e1a].green = ledsBuff[e1a].green;

					leds[e1].blue = ledsBuff[e1].blue;
					leds[e1a].blue = ledsBuff[e1a].blue;

					FastLED.show();

					e1a = NUM_LEDS - e1;

					e1++;
					
				}

			}
			if (e1 >= j) { e1 = 0; e1a = 0; }
			e++;
			
		}
		
	}
	if (e >= times) { e = 0; UDPresponse("0:1"); }
}

void UDPresponse(char ReplyBuffer[]) {
	Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
	Udp.write(ReplyBuffer);
	Udp.endPacket();
	
	//delayMicroseconds(200);
}

void wifiSetup() {
	
	delay(500);
	digitalWrite(0, HIGH);
	WiFi.beginSmartConfig();

	while (WiFi.status() != WL_CONNECTED) {
		digitalWrite(0, LOW);
		delay(250);
		Serial.print(".");
		digitalWrite(0, HIGH);
		delay(250);
	}
	digitalWrite(0, LOW);
}