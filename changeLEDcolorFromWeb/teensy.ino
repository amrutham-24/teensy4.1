#define PIN_R 2
#define PIN_G 3
#define PIN_B 4
String rxLine = "";

void setColor(int r, int g, int b) {
  analogWrite(PIN_R, r);
  analogWrite(PIN_G, g);
  analogWrite(PIN_B, b);
}

void parseLine(String line) {
  int r, g, b, br;
  int matched = sscanf(line.c_str(), "C,%d,%d,%d,%d", &r, &g, &b, &br);

  Serial.print("RX raw: [");
  Serial.print(line);
  Serial.println("]");

  if (matched != 4) {
    Serial.println("Parse FAILED, ignoring line");
    return;
  }

  float scale = br / 255.0;
  int rr = r * scale, gg = g * scale, bb = b * scale;

  Serial.printf("Parsed r=%d g=%d b=%d br=%d -> out r=%d g=%d b=%d\n",
                r, g, b, br, rr, gg, bb);

  setColor(rr, gg, bb);
}

void setup() {
  Serial.begin(115200);      // USB debug console
  Serial1.begin(115200);     // RX1 = pin 0, from ESP32 TX
  pinMode(PIN_R, OUTPUT);
  pinMode(PIN_G, OUTPUT);
  pinMode(PIN_B, OUTPUT);
  delay(1000);
  Serial.println("Teensy ready, waiting on Serial1...");
}

void loop() {
  while (Serial1.available()) {
    char c = Serial1.read();
    if (c == '\n') {
      parseLine(rxLine);
      rxLine = "";
    } else if (c != '\r') {
      rxLine += c;
    }
  }
}
