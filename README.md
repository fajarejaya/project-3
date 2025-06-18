# 💧 IoT-Based Air Humidifier with ESP32

Proyek ini merupakan sistem **Air Humidifier otomatis** berbasis **ESP32 C6 WROOM 1** dengan dukungan **Blynk IoT Platform**, **Sensor DHT22**, **Relay SRD-05VDC-SL-C**, dan **LCD I2C** untuk monitoring suhu dan kelembapan serta pengendalian mist maker secara otomatis dan manual.

---

## 📁 Struktur Repositori

```bash
.
├── diagrams/
│   ├── wiring-diagram.png
│   └── system-flowchart.png
├── src/
│   ├── main.cpp             # Program utama untuk ESP32
├── README.md                # Dokumentasi proyek
├── platformio.ini           # Konfigurasi proyek PlatformIO
```

📝 Deskripsi Singkat
IoT Air Humidifier ini bekerja dengan dua mode:
- Auto Mode: Mist maker akan aktif jika kelembapan < 70.9% dan akan mati jika > 85.0%.
- Manual Mode: Mist maker dikendalikan secara langsung via aplikasi Blynk.

LCD 1602 I2C menampilkan:
- Baris 1: Suhu & kelembapan (Contoh: T: 27.5C H: 66.3%)

- Baris 2: Mode dan status relay (Contoh: M: Auto S: ON)

📲 Blynk Integration
| Virtual Pin | Fungsi                      |
| ----------- | --------------------------- |
| `V0`        | Mode: Auto / Manual         |
| `V1`        | Kontrol Manual ON / OFF     |
| `V2`        | Menampilkan Suhu (°C)       |
| `V3`        | Menampilkan Kelembapan (%)  |
| `V4`        | Status Mist Maker: ON / OFF |

🧰 Hardware yang Digunakan
    1. ESP32 C6 WROOM 1
    2. Sensor DHT22
    3. Relay 5V SRD-05VDC-SL-C
    4. Mist Maker 113kHz 16mm
    5. LCD 1602A + I2C Module
    6. Power Supply 5V DC

📊 Diagram
  ⚡ Wiring Diagram
      
  🔁 System Flowchart

  
