#include <Wire.h> 
// I2C 통신을 위한 Wire 라이브러리 포함

union FloatUnion 
{
  float value;
  byte bytes[4];
};
// float 값을 바이트 배열로 변환하는 Union 정의 

void setup() 
{
  Serial.begin(115200); // 시리얼 통신 시작 (115200로 설정)
  Wire.begin(8); // I2C 슬레이브 주소 8번으로 설
  Wire.onReceive(receiveEvent); // 데이터 수신할 때 호출될 함수 등록
}

void loop() //
{
  delay(100); //0.1초 대기 
}

// 데이터 수신 함수 실행 
void receiveEvent(int howMany) // howMany는 데이터가 수신된 바이트 수를 나타내는 매개변수
{
  if (Wire.available() >= 16) // 모든 데이터(16개의 바이트)가 수신될 때까지 대기
  { 
    FloatUnion frontUnion, leftUnion, rightUnion, resultUnion;
    
    //바이트가 4바이트라 for문 이용하여 4번 반복
    
    for (int i = 0; i < 4; i++) 
    {
      frontUnion.bytes[i] = Wire.read(); // 정면 초음파 센서로부터 받은 데이터 값 수신
    }
    for (int i = 0; i < 4; i++) 
    {
      leftUnion.bytes[i] = Wire.read(); // 좌측 초음파 센서로부터 받은 데이터 값 수신
    }
    for (int i = 0; i < 4; i++) 
    {
      rightUnion.bytes[i] = Wire.read(); // 우측 초음파 센서로부터 받은 데이터 값 수신
    }
    for (int i = 0; i < 4; i++) 
    {
      resultUnion.bytes[i] = Wire.read(); // 가변 저항 센서로 받은 데이터 값 수신
    }
    
    // 수신된 데이터를 시리얼 모니터에 출력
    Serial.print("정면 초음파: "); Serial.print(frontUnion.value); Serial.print(" cm, ");
    Serial.print("왼쪽 초음파: "); Serial.print(leftUnion.value); Serial.print(" cm, ");
    Serial.print("오른쪽 초음파: "); Serial.print(rightUnion.value); Serial.print(" cm, ");
    Serial.print("가변저항: "); Serial.print(fabs(resultUnion.value)); Serial.println(" ");
  }
}
