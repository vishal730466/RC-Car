#include <WiFi.h>
#include <WebServer.h>

const char *ssid = "ESP32-Network";
const char *password = "123456789";

WebServer server(80);

const int bleft = 4;
const int bright = 5;
const int fleft = 13;
const int fright = 32;
const int ena1   = 23;
const int ena2   = 22;
const int ena3   = 12;
const int ena4   = 14;

int enaval = 100;

// Use a raw string literal for the HTML and JavaScript code
const char htmlPage[] PROGMEM = R"rawliteral(
  <!DOCTYPE html>
  <html>
    <head>
      <style>
        html, body {
          margin: 0;
          padding: 0;
          height: 100%;
          width: 100%;
          overflow: hidden;
        }
        #circle {
          width: 100px;
          height: 100px;
          background-color: red;
          position: absolute;
          top: 200px;
          left: 200px;
          transition: top 0.5s ease;
        }
        #con {
          width: 500px;
          height: 500px;
  
          background-color: blue;
          position: absolute;
          top: 700px;
          left: 300px;
        }
      .btn{
                height: 100px;
                width: 100px;
                position: relative;
                top: 100px;
                left: 100px;
                font-size: larger;
            }
      </style>
    </head>
    <body> 
    <button class="btn" onclick="fetch('/ena')">50</button>
      <button class="btn" onclick="fetch('/ena/100')">100</button>
      <button class="btn" onclick="fetch('/ena/max')">Max</button>

      <div id="con">
        <div id="circle"></div>
      </div>
      

      <script>
        let circle = document.getElementById('circle');
        let startY, startX;

        document.body.addEventListener('touchmove', function(e) { e.preventDefault(); }, { passive: false });

        circle.addEventListener('touchstart', function(e) {
          startY = e.touches[0].clientY;
          startX = e.touches[0].clientX;
          circle.style.transition = 'none';
        });

        circle.addEventListener('touchmove', function(e) {
          let currentY = e.touches[0].clientY;
          let currentX = e.touches[0].clientX;

          let moveY = currentY - startY;
          let moveX = currentX - startX;

          let newTop = circle.offsetTop + moveY;
          let newLeft = circle.offsetLeft + moveX;

          if (newLeft >= 50 && newTop <= 400 && newLeft <= 400 && newTop >= 50) {
            circle.style.top = newTop + 'px';
            circle.style.left = newLeft + 'px';

            if (newTop <= 160 && newLeft >= 0 && newLeft < 500) {
              fetch('/forward/on');
            } else if (newTop >= 350 && newLeft > 150 && newLeft < 400) {
              fetch('/back/on');
            } else if (newLeft >= 0 && newLeft < 200 && newTop > 160 && newTop < 400) {
              fetch('/left/on');
            } else if (newLeft > 300 && newTop > 160 && newTop < 400) {
              fetch('/right/on');
            } else {
              fetch('/led/off');
            }
          }
          startY = currentY;
          startX = currentX;
        });

        circle.addEventListener('touchend', function(e) {
          circle.style.top = '200px';
          circle.style.left = '200px';
          setTimeout(function() {
            fetch('/led/off');
          }, 500);
        });
      </script>
    </body>
  </html>
)rawliteral";

// Function to handle root URL "/"
void handleRoot()
{
  server.send(200, "text/html", htmlPage);
}

void forward() {
  digitalWrite(bleft, HIGH);
  digitalWrite(fleft, HIGH);
  digitalWrite(fright, HIGH);
    digitalWrite(bright, HIGH);
  analogWrite(ena1,enaval);
  analogWrite(ena2,enaval);
   analogWrite(ena3,enaval);
  analogWrite(ena4,enaval);
  
  Serial.println("forward");
  server.send(200, "text/plain", "Forward On");
}

void handleLedOff() {
  digitalWrite(bleft, LOW);
  digitalWrite(fleft, LOW);
  digitalWrite(bright, LOW);
  digitalWrite(fright, LOW);
  Serial.println("LED is OFF");
  server.send(200, "text/plain", "LED Off");
}

void back() {
  digitalWrite(bleft, LOW);
  digitalWrite(bright, LOW);
  digitalWrite(fleft, LOW);
  digitalWrite(fright, LOW);
//  analogWrite(ena1,enaval);
//   analogWrite(ena2,enaval);
  Serial.println("Back On");
  server.send(200, "text/plain", "Back On");
}

void left() {
  digitalWrite(fleft, LOW);
  digitalWrite(fright, HIGH);
  digitalWrite(bleft, LOW);
  digitalWrite(bright, HIGH);
  Serial.println("Left On");
  server.send(200, "text/plain", "Left On");
}

void right() {
  digitalWrite(fleft, HIGH);
  digitalWrite(fright, LOW);
  digitalWrite(bleft, HIGH);
  digitalWrite(bright, LOW);
  Serial.println("Right On");
  server.send(200, "text/plain", "Right On");
}

void ena(){
  enaval=50;
} 
void ena_100(){
  enaval= 100;
}
void ena_max(){
  enaval= 250;
}

void setup()
{
  Serial.begin(115200);
  pinMode(fleft, OUTPUT);
  pinMode(fright, OUTPUT);
  pinMode(bleft, OUTPUT);
  pinMode(bright, OUTPUT);
  pinMode(ena1, OUTPUT);
  pinMode(ena2, OUTPUT);

  digitalWrite(fleft, LOW);
  digitalWrite(fright, LOW);
  digitalWrite(bleft, LOW);
  digitalWrite(bright, LOW);
  digitalWrite(ena1, 0);// Initially off
 digitalWrite(ena2, 0);
 digitalWrite(ena3, 0);
 digitalWrite(ena4, 0);

  WiFi.softAP(ssid, password);
  server.on("/", handleRoot);
  server.on("/forward/on", forward);
  server.on("/left/on", left);
  server.on("/back/on", back);
  server.on("/right/on", right);
  server.on("/led/off", handleLedOff);
    server.on("/ena", ena);
     server.on("/ena/100", ena_100);
     server.on("/ena/max", ena_100);

  server.begin();
  Serial.println("HTTP server started");
}

void loop()
{
  server.handleClient();
}
