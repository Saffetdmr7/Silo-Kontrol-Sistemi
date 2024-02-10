#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// DHT11/DHT22 Sensörü
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// MQ-2 Gaz Sensörü
#define MQ2PIN A0

// Piezo Buzzer
#define BUZZERPIN 5

// LED'ler
#define REDLEDPIN 6
#define GREENLEDPIN 7

// Buton
#define BUTTONPIN 12

// I2C LCD adresini ve boyutunu ayarla
LiquidCrystal_I2C lcd(0x27, 20, 4);

bool riskliDurum = false;
bool sistemAktif = true;
bool oncekiButonDurumu = HIGH;

void setup() {
  pinMode(REDLEDPIN, OUTPUT);
  pinMode(GREENLEDPIN, OUTPUT);
  pinMode(BUZZERPIN, OUTPUT);
  pinMode(BUTTONPIN, INPUT_PULLUP);

  lcd.init();                      // LCD'yi başlat
  lcd.backlight();                 // Arka ışığı aç
  dht.begin();
}

void loop() {
  float nem = 0;        // Değişkenleri loop'un başında tanımla
  float sicaklik = 0;
  int gazSeviyesi = 0;

  if (sistemAktif) {
    nem = dht.readHumidity();
    sicaklik = dht.readTemperature();
    gazSeviyesi = analogRead(MQ2PIN);
  
    // Risk değerlendirmesi
    riskliDurum = (nem > 70 || sicaklik > 25 || gazSeviyesi > 170);
  
    if (riskliDurum) {
      digitalWrite(REDLEDPIN, HIGH);
      digitalWrite(GREENLEDPIN, LOW);
      tone(BUZZERPIN, 3000);  // Buzzer'ı çalıştır
    } else {
      digitalWrite(REDLEDPIN, LOW);
      digitalWrite(GREENLEDPIN, HIGH);
      noTone(BUZZERPIN);  // Buzzer'ı durdur
    }
  }

  // Buton kontrolü
  bool guncelButonDurumu = digitalRead(BUTTONPIN);
  if (guncelButonDurumu == LOW && oncekiButonDurumu == HIGH) {
    sistemAktif = !sistemAktif;
    if (!sistemAktif) {
      noTone(BUZZERPIN);
      digitalWrite(REDLEDPIN, LOW);
      digitalWrite(GREENLEDPIN, LOW);
    }
    delay(200); // Buton gürültüsünü engellemek için bekle
  }
  oncekiButonDurumu = guncelButonDurumu;

  // LCD ekranda verileri göster
  if (sistemAktif) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(riskliDurum ? "Durum: Riskli" : "Durum: Guvenli");

    lcd.setCursor(0, 1);
    lcd.print("Nem: ");
    lcd.print(nem);
    lcd.print("%");

    lcd.setCursor(0, 2);
    lcd.print("Temp: ");
    lcd.print(sicaklik);
    lcd.print("C");

    lcd.setCursor(0, 3);
    lcd.print("Gaz: ");
    lcd.print(gazSeviyesi);
  }

  delay(500); // 0,5 saniye bekle
}
