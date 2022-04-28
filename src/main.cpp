#include <Arduino.h>
#include "WiFi.h"
#include "WebServer.h"
#include "FastLED.h"
#include "secrets.h"

const char* ssid = SECRET_SSID;
const char* password = SECRET_PASS;

const uint8_t baseRed = 0;
const uint8_t baseOrange = 18;
const uint8_t baseGreen = 92;

WebServer server(80);
enum Status {
    RESOLVED,
    WARNING,
    CRITICAL,
};

Status currentStatus = RESOLVED;
uint8_t currentColor = baseGreen;

#define NUM_LEDS 5
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
#define DATA_PIN 18

CRGB leds[NUM_LEDS];

void onRoot() {
    Serial.println("Root");
    server.send(200, "text/plain", "Hi there!");
}

void onWarning() {
    currentStatus = WARNING;
    Serial.println("Warning!");
    server.send(200, "text/plain", "Warning");
}

void onCritical() {
    currentStatus = CRITICAL;
    Serial.println("Critical!");
    server.send(200, "text/plain", "Critical");
}

void onResolved() {
    currentStatus = RESOLVED;
    Serial.println("Resolved");
    server.send(200, "text/plain", "Resolved");
}

void setup() {
    Serial.begin(115200);

    Serial.println();
    Serial.print("Connecting to: ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    Serial.println();
    Serial.println("WiFi connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    server.on("/", onRoot);
    server.on("/warning", onWarning);
    server.on("/critical", onCritical);
    server.on("/resolved", onResolved);
    server.begin();

    FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    FastLED.setBrightness(100);

}



void loop() {
    server.handleClient();
    delay(2);
    EVERY_N_MILLISECONDS(200) {
        Serial.println(currentStatus);
        switch (currentStatus) {
            case RESOLVED:
                currentColor = baseGreen;
                break;
            case WARNING:
                currentColor = baseOrange;
                break;
            case CRITICAL:
                currentColor = baseRed;
                break;
        }
        for (auto & led : leds) {
            led = CHSV(
                    random8(currentColor - 2, currentColor + 2),
                    random8(200, 255),
                    random8(125, 255)
                    );
        }
        FastLED.show();
    }
}

