#include <Wire.h> //Untuk komunikasi I2C
#include <LiquidCrystal_I2C.h> //Untuk mengendalikan LCD berbasis I2C
#include <WiFi.h> //Untuk mengaktifkan koneksi WiFi pada Arduino
#include <HTTPClient.h> //Untuk membuat permintaan HTTP dari Arduino

LiquidCrystal_I2C lcd(0x27, 16, 2); // LCD I2C 16x2


// untuk konfig wifi
const char* ssid = "LAPTOP";
const char* password = "12345678";

//untuk api url
const char* serverUrl = "http://192.168.137.1/insert_hujan.php";


const float mmPerPulse = 0.70;  // Masukkan nilai hujan dalam mm untuk setiap gerakan bucket

float mmTotali = 0.00;  // Initialize to 0.00 //total curah hujan
int sensore = 0; //menyimpan nilai dari sensor atau kondisi tertentu
int lastState = 0; //untuk menyimpan status atau keadaan sebelumnya 
int A = 0; //sisa dari iterasi sebelumnya
int Jtip = 0; //menghitung jumlah tip (tipping) 

// Pin untuk Tipping Bucket (Sensor Hujan)
int tippingBucketPin = 13;  // Sesuaikan dengan pin yang digunakan pada ESP32

void setup() {
  pinMode(tippingBucketPin, INPUT); //digunakan untuk menetapkan mode input pada pin yang mewakili sensor curah hujan

  // Inisialisasi komunikasi I2C dengan pin default SDA (D21) dan SCL (D22)
  Wire.begin(21, 22);

  // Inisialisasi LCD dengan 16 kolom dan 2 baris
  lcd.begin(); //digunakan untuk menginisialisasi objek
  lcd.backlight(); //digunakan untuk menghidupkan lampu latar (backlight) pada modul LCD

  lcd.setCursor(2, 0); //digunakan untuk mengatur posisi kursor pada modul LCD berbasis I2C yang dikendalikan oleh objek LiquidCrystal_I2C
  lcd.print("Curah Hujan"); //digunakan untuk menampilkan teks "Curah Hujan" pada layar LCD 
  lcd.setCursor(5, 1);
  lcd.print("RESET");

  delay(500);

  lcd.clear();

  // tambah baudrate
  Serial.begin(115200);
  //konek wifi
    WiFi.begin(ssid, password);
    // pengondisian jika wifi belum konek
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  // Pengndisian jika sudah konek
  Serial.println("Connected to WiFi");

}

void loop() {
  sensore = digitalRead(tippingBucketPin);

  if (sensore != lastState) {
    mmTotali = mmTotali + mmPerPulse;
    Jtip++;

    lcd.clear(); // Clear the LCD to update the display
  }

  lcd.setCursor(0, 0);
  lcd.print("Rainfall: ");
  lcd.print(mmTotali, 2);  // Display with 2 decimal places
  lcd.print("mm");

  lcd.setCursor(0, 1);
  lcd.print("Tip: ");
  lcd.print(Jtip);

  delay(100);

  lastState = sensore;

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Persiapkan data untuk dikirim
    String postData = "rainfall=" + String(mmTotali) + "&tip_count=" + String(Jtip);
 
    // Koneksi ke server REST API
    http.begin(serverUrl);

    // untuk form body pada proses post data
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    // untuk eksekusi post data
    int httpResponseCode = http.POST(postData);

    // untuk mengetahui jika proses berhasil atau tidak
    if (httpResponseCode > 0) {
      Serial.print("Data dikirim dengan sukses. Kode respons: ");
      // Serial.println(httpResponseCode);
    } else {
      Serial.print("Gagal mengirim data. Kode respons: ");
      Serial.println(httpResponseCode);
    }

    http.end();
    
  } else {
    Serial.println("Koneksi WiFi tidak tersedia.");
  }
  // Interval berapa lama proses melakukan looping lagi
  delay(5000);
}


// Programuntuk koneksi ke wifi
void connectToWifi(){
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi"); 
}
