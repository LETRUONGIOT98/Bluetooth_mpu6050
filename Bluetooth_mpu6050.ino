#include <Wire.h>
#include <MPU6050.h>
#include <SoftwareSerial.h> // Thêm thư viện SoftwareSerial

MPU6050 mpu;
SoftwareSerial BTSerial(10, 11); // RX, TX cho Bluetooth HC-05

// Chọn mức độ nhạy
#define ACCEL_SCALE 0 // 0: ±2g, 1: ±4g, 2: ±8g, 3: ±16g

void setup() {
  Serial.begin(9600); // Kết nối với Serial Monitor
  BTSerial.begin(9600); // Khởi động Bluetooth HC-05 với tốc độ 9600 bps
  Wire.begin();

  // Khởi động MPU6050
  mpu.initialize();

  // Cấu hình độ nhạy cho gia tốc
  mpu.setFullScaleAccelRange(ACCEL_SCALE);
}

void loop() {
  // Đọc gia tốc
  int16_t ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);

  // Chuyển đổi giá trị gia tốc từ raw sang g
  float accelX = ax / 16384.0; // Đối với ±2g
  float accelY = ay / 16384.0; // Đối với ±2g
  float accelZ = az / 16384.0; // Đối với ±2g

  // Điều chỉnh độ nhạy
  switch (ACCEL_SCALE) {
    case 1: // ±4g
      accelX = ax / 8192.0;
      accelY = ay / 8192.0;
      accelZ = az / 8192.0;
      break;
    case 2: // ±8g
      accelX = ax / 4096.0;
      accelY = ay / 4096.0;
      accelZ = az / 4096.0;
      break;
    case 3: // ±16g
      accelX = ax / 2048.0;
      accelY = ay / 2048.0;
      accelZ = az / 2048.0;
      break;
    default: // ±2g
      // Không cần thay đổi vì đã mặc định ở trên
      break;
  }

  // In ra giá trị gia tốc lên Serial Monitor
  Serial.print("Trục X ");
  Serial.print(accelX);
  Serial.print("g, Y: ");
  Serial.print(accelY);
  Serial.print("g, Z: ");
  Serial.print(accelZ);
  Serial.println("g");

  // Tính gia tốc tổng hợp
  float totalAccel = sqrt(accelX * accelX + accelY * accelY + accelZ * accelZ);
  
  // Đưa ra thông báo theo mức độ
  String message;
  if (totalAccel > 16.0) {
    message = "Cảnh báo khẩn! Gia tốc cao vướt đã quá 16g!";
  } else if (totalAccel > 8.0) {
    message = "Cảnh báo: Gia tốc cao vượt quá 8g Quy Hiểm";
  } else if (totalAccel > 4.0) {
    message = "Cảnh báo: Gia tốc vượt quá 4g Cận Trọng";
  } else if (totalAccel > 2.0) {
    message = "Cảnh báo: Gia tốc vượt quá 2g Cận Thận!";
  } else if (totalAccel > 1.0) {
    message = "Cảnh báo: Gia tốc vượt quá 1g!";
  } else {
    message = "Gia tốc nằm trong giới hạn an toàn";
  }

  // In ra thông báo lên Serial Monitor
  Serial.println(message);
  String data = String(accelX)+ "," + String(accelY)+ "," + String(accelZ) + "," + String(message) ;
  /*BTSerial.print("Trục X ");
  BTSerial.print(accelX);
  BTSerial.print("g, Y: ");
  BTSerial.print(accelY);
  BTSerial.print("g, Z: ");
  BTSerial.print(accelZ);
  BTSerial.println("g");*/
  // Gửi thông điệp qua Bluetooth
  BTSerial.println(data); // Gửi thông điệp qua Bluetooth

  delay(100); // Đợi 500ms trước khi đọc lại
}
