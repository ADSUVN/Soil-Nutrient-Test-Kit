
#include <ModbusMaster.h>
#include <SoftwareSerial.h>

#define NPK_RX 4
#define NPK_TX 5
#define ESP_RX 2
#define ESP_TX 3

#define DEBUG

ModbusMaster node;
SoftwareSerial npk_uart(NPK_RX, NPK_TX);
SoftwareSerial esp_uart(ESP_RX, ESP_TX);

unsigned long lastSend = 0;
bool receivedData = false;
int phosphateValue,potassiumValue,nitrateValue;
void setup() {
  // Initialize Serial communication for debugging
  Serial.begin(9600);
  npk_uart.begin(9600);
  esp_uart.begin(115200);
  // Configure the ModbusMaster object
  node.begin(1, npk_uart);  // Slave address 1, Serial communication
}

void loop() {
  // Read values from Modbus registers
  uint16_t nitrateValue = readModbusRegister(0x001E);    // Address 001E
  uint16_t phosphateValue = readModbusRegister(0x001F);  // Address 001F
  uint16_t potassiumValue = readModbusRegister(0x0020);  // Address 0020
  uint16_t deviceAddress = readModbusRegister(0x0100);   // Address 0100
  uint16_t speedValue = readModbusRegister(0x0101);      // Address 0101

  // uint8_t result;

  // result = node.readHoldingRegisters(30, 3);

  // if (result == node.ku8MBSuccess) {                   // Nếu đọc dữ liệu thành công
  //   Serial.println("Data received from ES-NPK-SOIL");  // In thông báo trên Serial Monitor
  //   receivedData = true;                               // Cập nhật biến boolean về trạng thái true để cho biết đã nhận được dữ liệu từ ES-PH-SOIL-01
  // } else {
  //   Serial.println("Error: ");
  //   Serial.println(result);
  // }

  // if (receivedData) {  // Nếu đã nhận được dữ liệu từ ES-PH-SOIL-01
  //   // Gửi dữ liệu đến Arduino Uno
  //   nitrateValue = node.getResponseBuffer(0);
  //   phosphateValue = node.getResponseBuffer(1);
  //   potassiumValue = node.getResponseBuffer(2);
  //   Serial.print("Nito: ");
  //   Serial.print(node.getResponseBuffer(0));
  //   Serial.println("mg/Kg");
  //   Serial.print("Photpho: ");
  //   Serial.print(node.getResponseBuffer(1));
  //   Serial.println("mg/Kg");
  //   Serial.print("Kali: ");
  //   Serial.print(node.getResponseBuffer(2));
  //   Serial.println("mg/Kg");
  //   receivedData = false;  // Cập nhật biến boolean về trạng thái false để sẵn sàng nhận dữ liệu từ ES-PH-SOIL-01 tiếp theo
  // }
  // delay(1000);

  int val = analogRead(A0);
  int humi = map(val, 0, 1023, 0, 100);

#ifdef DEBUG
  // Display the read values
  Serial.print("Nitrate Value: ");
  Serial.println(nitrateValue);
  Serial.print("Phosphate Value: ");
  Serial.println(phosphateValue);
  Serial.print("Potassium Value: ");
  Serial.println(potassiumValue);
  Serial.print("Humi: ");
  Serial.println(humi);
#endif

#ifdef ESP_TX
  if (millis() - lastSend > 2000) {
    String temp = String(nitrateValue) + ", " + String(phosphateValue) + ", " + String(potassiumValue) + ", " + String(humi) + "\n";
    esp_uart.print(temp);
    lastSend = millis();
  }
#endif
  delay(1000);  
}

// Function to read a Modbus register
uint16_t readModbusRegister(uint16_t address) {
  uint16_t result;
  uint8_t status;

  // Keep retrying until the status is not 226

  status = node.readHoldingRegisters(address, 1);

  if (status == node.ku8MBSuccess) {
    result = node.getResponseBuffer(0);  // Get the value from the response buffer
  } else if (status == 226) {
    Serial.println("Slave device busy, waiting...");
  } else {
    Serial.println("Error reading Modbus register");
    result = 0;  // Return a default value
  }


  return result;
}
