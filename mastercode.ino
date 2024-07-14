#include <Wire.h> 
// I2C통신을 위해 사용되는 Wire 라이브러리
#include <NewPing.h>
// 초음파 거리 센서를 쉽게 제어하기 위해 사용되는 NewPing 라이브러리
#include <MsTimer2.h> 
// 주기적인 작업을 수행하기 위해 사용하는 MsTimer2 라이브러리

#define SONAR_NUM 3      
#define MAX_DISTANCE 100

#define SIZE 5

#define sensorPin A1

#define Front 0
#define Left  1 
#define Right 2

#define TRIG1 26 // 초음파 센서 1번 Trig 핀 번호
#define ECHO1 27 // 초음파 센서 1번 Echo 핀 번호

#define TRIG2 30 // 초음파 센서 2번 Trig 핀 번호
#define ECHO2 31 // 초음파 센서 2번 Echo 핀 번호

#define TRIG3 34 // 초음파 센서 3번 Trig 핀 번호
#define ECHO3 35 // 초음파 센서 3번 Echo 핀 번호

////////////////////////////초음파 센서////////////////////////////

//초음파 센서 배열 생성
NewPing sonar[SONAR_NUM] = {   
  NewPing(TRIG1, ECHO1, MAX_DISTANCE), 
  NewPing(TRIG2, ECHO2, MAX_DISTANCE),
  NewPing(TRIG3, ECHO3, MAX_DISTANCE)
};

//각 센서의 데이터를 저장할 배열
float front_sensorData[SIZE] = {0.0};
float left_sensorData[SIZE] = {0.0};
float right_sensorData[SIZE] = {0.0};
float resistance_sensorData[SIZE] = {0.0};

//정면 초음파 센서의 이동 평균을 계산하는 함수
float recursive_moving_average_front(float ad_value) {
  static float avg_front = 0.0;  //avg_front 변수 정적 변수로 정의

//배열의 데이터를 한 칸씩 앞으로 이동
  for (int i = 0; i <= SIZE - 2; i++) {
    front_sensorData[i] = front_sensorData[i + 1]; 
  }
  
  //센서로 받은 새로운 데이터 값 추가
  front_sensorData[SIZE - 1] = ad_value;
  //재귀이동평균 계산 
  avg_front = avg_front + (front_sensorData[SIZE - 1] - front_sensorData[0]) / (float)SIZE;
  
  //계산한 전방 초음파 센서 값 평균 계산 
  return avg_front;
}

//좌측 초음파 센서의 이동 평균을 계산하는 함수
float recursive_moving_average_left(float ad_value) {
  static float avg_left = 0.0;  //avg_left 변수 정적 변수로 정의

//배열의 데이터를 한 칸씩 앞으로 이동
  for (int i = 0; i <= SIZE - 2; i++) {
    left_sensorData[i] = left_sensorData[i + 1]; 
  }
  
  //센서로 받은 새로운 데이터 값 추가  
  left_sensorData[SIZE - 1] = ad_value;
  //재귀이동평균 계산
  avg_left = avg_left + (left_sensorData[SIZE - 1] - left_sensorData[0]) / (float)SIZE;
  
  //계산한 좌측 초음파 센서 값 평균 계산
  return avg_left;
}

//우측 초음파 센서의 이동 평균을 계산하는 함수
float recursive_moving_average_right(float ad_value) {
  static float avg_right = 0.0;  //avg_right 변수 정적 변수로 정의

//배열의 데이터를 한 칸씩 앞으로 이동
  for (int i = 0; i <= SIZE - 2; i++) {
    right_sensorData[i] = right_sensorData[i + 1]; 
  }
  //센서로 받은 새로운 데이터 값 추가  
  right_sensorData[SIZE - 1] = ad_value;
  //재귀이동평균 계산
  avg_right = avg_right + (right_sensorData[SIZE - 1] - right_sensorData[0]) / (float)SIZE;
  
  //계산한 우측 초음파 센서 값 평균 계산
  return avg_right;
}

////////////////////////////가변 저항////////////////////////////

//가변저항 값의 이동 평균을 계산하는 함수
float recursive_moving_average_resistance(float add_value) {
  static float avg_resistance = 0.0;  //avg_resistance 변수 정적 변수로 정의

//배열의 데이터를 한 칸씩 앞으로 이동
  for (int i = 0; i <= SIZE - 2; i++) {
    resistance_sensorData[i] = resistance_sensorData[i + 1]; 
  }
  //센서로 받은 새로운 데이터 값 추가
  resistance_sensorData[SIZE - 1] = add_value;
  //재귀이동평균 계산
  avg_resistance = avg_resistance + (resistance_sensorData[SIZE - 1] - resistance_sensorData[0]) / (float)SIZE;

  //계산한 가변 저항 센서 값 평균 계산
  return avg_resistance;
}

float result = 0.0; // result변수 전역 변수로 선언

void Read_sensor(void) {
   float add_value = 0.0; //변수 초기화
   add_value = analogRead(A0); //센서 값 읽기
   result = recursive_moving_average_resistance(add_value); //재귀이동평균 계산 함수 값 result 변수에 대입
}

void setup() {
//각 초음파 센서 핀 설정
  pinMode(TRIG1, OUTPUT);
  pinMode(ECHO1, INPUT);

  pinMode(TRIG2, OUTPUT);
  pinMode(ECHO2, INPUT);

  pinMode(TRIG3, OUTPUT);
  pinMode(ECHO3, INPUT);
  
  Serial.begin(115200); //시리얼 통신 시작

  MsTimer2::set(100, Read_sensor); // 100ms마다 Read_sensor 함수 호출
  MsTimer2::start(); // MsTimer2 시작

  Wire.begin(); //I2C 통신 시작
}

void loop() {
// 각 초음파 센서 거리값 읽어서 대입
  float front_sonar = sonar[Front].ping_cm();
  float left_sonar  = sonar[Left].ping_cm();  
  float right_sonar = sonar[Right].ping_cm();
  
  // 초음파 센서 거리 값이 0으로 측정될 때, 최대 거리로 설정
  if (front_sonar == 0.0 ) front_sonar = MAX_DISTANCE; 
  if (left_sonar == 0.0) left_sonar = MAX_DISTANCE;
  if (right_sonar == 0.0) right_sonar = MAX_DISTANCE;
  
  // 재귀 이동 평균 계산 함수에 값 대입하여 재귀 이동 평균 계산 후 대입
  float front_avg = recursive_moving_average_front(front_sonar);
  float left_avg = recursive_moving_average_left(left_sonar); 
  float right_avg = recursive_moving_average_right(right_sonar);

  //초음파 센서 거리값, 가변 저항 값 출력
  Serial.print("정면 초음파: "); Serial.print(front_avg); Serial.print("cm ");
  Serial.print("왼쪽 초음파: "); Serial.print(left_avg); Serial.print("cm ");
  Serial.print("오른쪽 초음파: "); Serial.print(right_avg); Serial.print("cm ");
  Serial.print("가변저항: "); Serial.print(fabs(result));
  Serial.println("    ");

////////////////////////////I2C 통신 (송신)////////////////////////////

// 데이터 전송을 위해 float 값을 변환하는 유니온 선언
  union {
    float value;
    byte bytes[4];
  } frontUnion, leftUnion, rightUnion, resultUnion;

// 각 유니온의 float 값에 계산한 해당 센서 평균 값을 저장 
  frontUnion.value = front_avg; //전면 센서 평균 값을 전면 센서 값 저장 유니온의 value 멤버에 저장
  leftUnion.value = left_avg;   //좌측 센서 평균 값을 좌측 센서 값 저장 유니온의 value 멤버에 저장
  rightUnion.value = right_avg; //우측 센서 평균 값을 우측 센서 값 저장 유니온의 value 멤버에 저장
  resultUnion.value = result;   //가변 저항 센서 평균 값을 가변저항 값 저장 유니온의 value 멤버에 저장

  Wire.beginTransmission(8); // 슬레이브 주소 8번으로 통신 시작
  
  //데이터 전송 함수(Wire.write(); - I2C 버퍼에 데이터를 기록하는 역할)
  
  Wire.write(frontUnion.bytes, 4); // 정면 초음파 센서 값 (4바이트)
  Wire.write(leftUnion.bytes, 4); // 왼쪽 초음파 센서 값 (4바이트)
  Wire.write(rightUnion.bytes, 4); // 오른쪽 초음파 센서 값 (4바이트)
  Wire.write(resultUnion.bytes, 4); // 가변 저항 값 (4바이트)
  
  // I2C전송 종료 및 전송 함수(Wire.endTransmission(); - I2C 통신 종료 및 버퍼에 저장된 데이터 전송)
  Wire.endTransmission(); // 데이터 송신 종료

  delay(100); //0.1초 대기 
}
