#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "RGB_Control";
const char* password = "rgb12345";
WebServer server(80);
HardwareSerial mySerial(1);

String page = R"rawliteral(
<!DOCTYPE html>
<html>
<body style="text-align:center; font-family:sans-serif;">
<h2>RGB LED Control</h2>
<input type="color" id="picker" value="#ff0000">
<br><br>
<button onclick="sendColor()">Set Color</button>
<p id="status"></p>
<script>
function sendColor() {
  let color = document.getElementById("picker").value;
  let r = parseInt(color.substr(1,2),16);
  let g = parseInt(color.substr(3,2),16);
  let b = parseInt(color.substr(5,2),16);
  fetch(`/set?r=${r}&g=${g}&b=${b}`)
    .then(res => res.text())
    .then(t => document.getElementById("status").innerText = t)
    .catch(e => document.getElementById("status").innerText = "Error: " + e);
}
</script>
</body>
</html>
)rawliteral";

void handleRoot() {
  server.send(200, "text/html", page);
}

void handleSet() {
  int r = server.arg("r").toInt();
  int g = server.arg("g").toInt();
  int b = server.arg("b").toInt();

  String msg = "C," + String(r) + "," + String(g) + "," + String(b) + ",255\n";

  Serial.print("Sending to Teensy: ");
  Serial.print(msg);

  mySerial.print(msg);   // printf on ESP32 can be finicky, plain print is more predictable
  mySerial.flush();      // make sure it's actually pushed out before responding

  server.send(200, "text/plain", "OK: " + msg);
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting AP...");
  WiFi.softAP(ssid, password);
  Serial.print("IP: ");
  Serial.println(WiFi.softAPIP());

  mySerial.begin(115200, SERIAL_8N1, 4, 5); // RX=4 (unused), TX=5 -> Teensy RX1
  Serial.println("mySerial started on TX=5");

  server.on("/", handleRoot);
  server.on("/set", handleSet);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
