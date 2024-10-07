#include "M5StickCPlus2.h"
#include "BluetoothSerial.h"  // Bluetooth用のライブラリ

BluetoothSerial bts;  // Bluetoothシリアルオブジェクトを作成

float accX = 0.0F, accY = 0.0F, accZ = 0.0F;     // 加速度
float gyroX = 0.0F, gyroY = 0.0F, gyroZ = 0.0F;  // ジャイロ
int X = 0, Y = 0;

void setup() {
  Serial.begin(115200);        // シリアルモニタ用ボーレート
  bts.begin("M5StickCPlus2");  // Bluetoothデバイス名を設定して開始
  auto cfg = M5.config();
  StickCP2.begin(cfg);
  StickCP2.Display.setRotation(1);
  StickCP2.Display.setTextColor(GREEN);
  StickCP2.Display.setTextDatum(top_center);
  StickCP2.Display.setFont(&fonts::FreeSansBold9pt7b);
  StickCP2.Display.setTextSize(0.8);
}

// 画面出力関数
void Display_print() {
  StickCP2.Display.clear();  // 画面をクリア

  // 各データを表示する際のカーソル位置を調整
  StickCP2.Display.setCursor(40, 5);  // 上部に加速度Xデータ
  StickCP2.Display.printf("Accel X: %0.2f", accX);

  StickCP2.Display.setCursor(40, 20);  // 少し下に加速度Yデータ
  StickCP2.Display.printf("Accel Y: %0.2f", accY);

  StickCP2.Display.setCursor(40, 35);  // さらに下に加速度Zデータ
  StickCP2.Display.printf("Accel Z: %0.2f", accZ);

  StickCP2.Display.setCursor(40, 60);  // ジャイロXデータ
  StickCP2.Display.printf("Gyro X: %0.2f", gyroX);

  StickCP2.Display.setCursor(40, 75);  // ジャイロYデータ
  StickCP2.Display.printf("Gyro Y: %0.2f", gyroY);

  StickCP2.Display.setCursor(40, 90);  // ジャイロZデータ
  StickCP2.Display.printf("Gyro Z: %0.2f", gyroZ);
}

void loop(void) {
  auto imu_update = StickCP2.Imu.update();

  M5.Imu.getAccelData(&accX, &accY, &accZ);    // 加速度データを取得
  M5.Imu.getGyroData(&gyroX, &gyroY, &gyroZ);  // ジャイロデータを取得

  if (imu_update) {
    Display_print();

    // Bluetoothでデータを送信
    bts.printf("%f\n", gyroX);
  }

  delay(100);  // 100ミリ秒の遅延
}
