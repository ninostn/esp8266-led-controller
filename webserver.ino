#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// ===== WiFi =====
const char* ssid = "LABKOM1";     
const char* password = "labkom@PPLG";

// ===== LED =====
const int leds[] = {5, 4, 14, 12, 13};

bool statusLED[5] = {false, false, false, false, false};

// ===== Server =====
ESP8266WebServer server(80);

// ===== HTML =====
const char html[] PROGMEM = R"(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <title>ESP8266 Dashboard</title>
  <style>
    :root {
      --primary: #000000;
      --secondary: #FFFFFF;
      --accent: #494949;
      --shadow: 8px 8px 0px var(--primary);
      --green: #28a745;
      --red: #dc3545;
      --off: #ccc;
    }
    * {
      margin: 0;
      padding: 0;
      box-sizing: border-box;
      font-family: 'Courier New', monospace;
    }
    body {
      background-color: var(--secondary);
      display: flex;
      flex-direction: column;
      justify-content: center;
      align-items: center;
      min-height: 100vh;
      padding: 20px;
    }
    .group-info {
      width: 100%;
      max-width: 400px;
      padding: 20px 25px;
      background-color: var(--secondary);
      border: 2px solid var(--primary);
      border-radius: 12px;
      box-shadow: var(--shadow);
      text-align: center;
      margin-bottom: 20px;
      position: relative;
    }
    .group-info h3 {
      color: var(--primary);
      font-size: 18px;
      margin-bottom: 10px;
      font-weight: 700;
    }
    .group-info p {
      color: var(--primary);
      font-size: 14px;
      line-height: 1.6;
    }
    .dashboard-card {
      width: 100%;
      max-width: 400px;
      padding: 30px 25px;
      background-color: var(--secondary);
      border: 2px solid var(--primary);
      border-radius: 12px;
      box-shadow: var(--shadow);
      text-align: center;
      position: relative;
    }
    .dashboard-card::before {
      content: '';
      position: absolute;
      top: 6px;
      left: 6px;
      right: -6px;
      bottom: -6px;
      border: 2px solid var(--primary);
      z-index: -1;
      border-radius: 12px;
    }
    h2 {
      color: var(--primary);
      margin-bottom: 25px;
      font-size: 24px;
      font-weight: 700;
    }
    .lamp-control {
      display: flex;
      justify-content: space-between;
      align-items: center;
      margin: 20px 0;
    }
    .lamp-label {
      font-weight: bold;
      display: flex;
      align-items: center;
      gap: 12px;
      color: var(--primary);
    }
    .lamp-symbol {
      width: 24px;
      height: 24px;
      border-radius: 50%;
      background-color: var(--off);
      display: inline-block;
      transition: 0.3s all;
    }
    .toggle-btn {
      padding: 10px 20px;
      border: 2px solid var(--primary);
      border-radius: 8px;
      font-weight: bold;
      cursor: pointer;
      color: var(--primary);
      background-color: #ffffff;
      transition: all 0.3s;
    }
    .toggle-btn:hover:not(:disabled) {
      transform: translate(-2px, -2px);
      box-shadow: 4px 4px 0px var(--primary);
    }
    .toggle-btn:disabled {
      opacity: 0.6;
      cursor: not-allowed;
    }
    .toggle-btn.active {
      background-color: #f0f0f0;
      border-color: #555;
    }
    .lamp-symbol.on {
      transform: scale(1.2);
    }
    .lamp-symbol.on.hijau {
      background-color: #28a745;
      box-shadow: 0 0 15px 4px #28a745;
    }
    .lamp-symbol.on.kuning {
      background-color: #ffc107;
      box-shadow: 0 0 15px 4px #ffc107;
    }
    .lamp-symbol.on.merah {
      background-color: #dc3545;
      box-shadow: 0 0 15px 4px #dc3545;
    }
    .lamp-symbol.on.biru {
      background-color: #007bff;
      box-shadow: 0 0 15px 4px #007bff;
    }
    .lamp-symbol.on.putih {
      background-color: #ffffff;
      border: 2px solid var(--primary);
      box-shadow: 0 0 15px 4px rgba(255, 255, 255, 0.8);
    }
  </style>
</head>
<body>
  <div class="group-info">
    <h3>Kelompok 5</h3>
    <p>Nino (Ngoding + Rakit), Raihan (Rakit), Naysila (Rakit), Mila (Jaringan), Nira (Rakit), Resti L (Ngoding), Restiana (Co Barang)</p>
  </div>

  <div class="dashboard-card">
    <h2>ESP8266 Dashboard</h2>

    <div class="lamp-control">
      <div class="lamp-label">
        <span class="lamp-symbol" id="lampHijau"></span> Hijau
      </div>
      <button class="toggle-btn" id="btnHijau">ON</button>
    </div>

    <div class="lamp-control">
      <div class="lamp-label">
        <span class="lamp-symbol" id="lampKuning"></span> Kuning
      </div>
      <button class="toggle-btn" id="btnKuning">ON</button>
    </div>

    <div class="lamp-control">
      <div class="lamp-label">
        <span class="lamp-symbol" id="lampMerah"></span> Merah
      </div>
      <button class="toggle-btn" id="btnMerah">ON</button>
    </div>

    <div class="lamp-control">
      <div class="lamp-label">
        <span class="lamp-symbol" id="lampBiru"></span> Biru
      </div>
      <button class="toggle-btn" id="btnBiru">ON</button>
    </div>

    <div class="lamp-control">
      <div class="lamp-label">
        <span class="lamp-symbol" id="lampPutih"></span> Putih
      </div>
      <button class="toggle-btn" id="btnPutih">ON</button>
    </div>
  </div>

  <script>
    const esp8266_ip = window.location.hostname;

    const btn = {
      hijau: document.getElementById("btnHijau"),
      kuning: document.getElementById("btnKuning"),
      merah: document.getElementById("btnMerah"),
      biru: document.getElementById("btnBiru"),
      putih: document.getElementById("btnPutih")
    };

    const lamp = {
      hijau: document.getElementById("lampHijau"),
      kuning: document.getElementById("lampKuning"),
      merah: document.getElementById("lampMerah"),
      biru: document.getElementById("lampBiru"),
      putih: document.getElementById("lampPutih")
    };

    Object.keys(lamp).forEach(color => {
      lamp[color].classList.add(color);
    });

    let status = {
      hijau: false,
      kuning: false,
      merah: false,
      biru: false,
      putih: false
    };

    Object.keys(btn).forEach(color => {
      btn[color].addEventListener("click", () => toggleLamp(color));
    });

    function toggleLamp(color) {
      btn[color].disabled = true;
      fetch(`http://${esp8266_ip}/api/${color}/toggle`)
        .then(res => res.json())
        .then(data => {
          status = data;
          updateLampUI();
        })
        .catch(err => {
          console.error("Error:", err);
        })
        .finally(() => {
          btn[color].disabled = false;
        });
    }

    function updateLampUI() {
      Object.keys(status).forEach(color => {
        if (status[color]) {
          lamp[color].classList.add("on");
          btn[color].textContent = "OFF";
          btn[color].classList.add("active");
        } else {
          lamp[color].classList.remove("on");
          btn[color].textContent = "ON";
          btn[color].classList.remove("active");
        }
      });
    }

    function fetchLampStatus() {
      fetch(`http://${esp8266_ip}/api/status`)
        .then(res => res.json())
        .then(data => {
          status = data;
          updateLampUI();
        })
        .catch(err => {
          console.error("Error:", err);
        });
    }

    setInterval(fetchLampStatus, 500);
    fetchLampStatus();
  </script>
</body>
</html>
)";

// ===== JSON Status =====
String getStatusJSON() {
  return "{"
         "\"hijau\":" + String(statusLED[0] ? "true" : "false") + ","
         "\"kuning\":" + String(statusLED[1] ? "true" : "false") + ","
         "\"merah\":" + String(statusLED[2] ? "true" : "false") + ","
         "\"biru\":" + String(statusLED[3] ? "true" : "false") + ","
         "\"putih\":" + String(statusLED[4] ? "true" : "false") +
         "}";
}

// ===== API =====

// Serve HTML
void handleRoot() {
  server.send_P(200, "text/html", html);
}

void handleStatus() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", getStatusJSON());
}

void handleHijau() {
  statusLED[0] = !statusLED[0];
  digitalWrite(leds[0], statusLED[0] ? HIGH : LOW);
  server.send(200, "application/json", getStatusJSON());
}

void handleKuning() {
  statusLED[1] = !statusLED[1];
  digitalWrite(leds[1], statusLED[1] ? HIGH : LOW);
  server.send(200, "application/json", getStatusJSON());
}

void handleMerah() {
  statusLED[2] = !statusLED[2];
  digitalWrite(leds[2], statusLED[2] ? HIGH : LOW);
  server.send(200, "application/json", getStatusJSON());
}

void handleBiru() {
  statusLED[3] = !statusLED[3];
  digitalWrite(leds[3], statusLED[3] ? HIGH : LOW);
  server.send(200, "application/json", getStatusJSON());
}

void handlePutih() {
  statusLED[4] = !statusLED[4];
  digitalWrite(leds[4], statusLED[4] ? HIGH : LOW);
  server.send(200, "application/json", getStatusJSON());
}

void setup() {
  Serial.begin(115200);

  for (int i = 0; i < 5; i++) {
    pinMode(leds[i], OUTPUT);
    digitalWrite(leds[i], LOW);
  }

  WiFi.begin(ssid, password);
  Serial.print("\nConnecting...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected!");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/api/status", handleStatus);
  server.on("/api/hijau/toggle", handleHijau);
  server.on("/api/kuning/toggle", handleKuning);
  server.on("/api/merah/toggle", handleMerah);
  server.on("/api/biru/toggle", handleBiru);
  server.on("/api/putih/toggle", handlePutih);

  server.begin();
  Serial.println("Server started!");
}

void loop() {
  server.handleClient();
}