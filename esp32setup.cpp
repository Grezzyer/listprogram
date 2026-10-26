#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>

const char* ssid = "Oppo A6X";
const char* password = "11261707";

const char* serverURL =
  "http://192.168.123.92/iot-monitoring/api/insert_data.php";

#define DHTPIN 4
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);

  dht.begin();

  Serial.println();
  Serial.println("IoT Monitoring Suhu & Kelembapan");
  Serial.println("Menghubungkan ke Wi-Fi...");

  WiFi.begin(ssid, password);

  int percobaan = 0;

  while (WiFi.status() != WL_CONNECTED && percobaan < 30) {
    delay(500);
    Serial.print(".");
    percobaan++;
  }

  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Wi-Fi berhasil terhubung!");
    Serial.print("IP ESP32: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Wi-Fi gagal terhubung.");
    Serial.println("Periksa nama dan password Wi-Fi.");
  }
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi terputus, mencoba menyambung...");
    WiFi.reconnect();

    delay(5000);
    return;
  }

  float suhu = dht.readTemperature();
  float kelembapan = dht.readHumidity();

  if (isnan(suhu) || isnan(kelembapan)) {
    Serial.println("Gagal membaca sensor DHT22.");
    delay(5000);
    return;
  }

  Serial.print("Suhu: ");
  Serial.print(suhu);
  Serial.print(" °C | Kelembapan: ");
  Serial.print(kelembapan);
  Serial.println(" %");

  HTTPClient http;

  http.begin(serverURL);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  String data =
    "suhu=" + String(suhu, 2) +
    "&kelembapan=" + String(kelembapan, 2);

  int httpCode = http.POST(data);

  if (httpCode > 0) {
    Serial.print("Kode HTTP: ");
    Serial.println(httpCode);

    Serial.print("Respons server: ");
    Serial.println(http.getString());

    if (httpCode == 200) {
      Serial.println("Data sensor berhasil dikirim.");
    }
  } else {
    Serial.print("Pengiriman gagal: ");
    Serial.println(HTTPClient::errorToString(httpCode));
  }

  http.end();

  Serial.println("-----------------------------");

  delay(10000);
}
