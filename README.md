# SNU_CED_2024_Teleop
서울대학교 CED 로봇팔 텔레오퍼레이션 프로젝트

이 프로젝트는 Dynamixel 모터와 HC-05 블루투스 모듈, OpenRB-150 보드를 이용하여 로봇팔을 원격 조작하는 시스템을 구현합니다.
두 개의 주요 스케치는 각각 모터 제어용 송신 스케치와 모터 상태 수신 스케치로 구성되어 있습니다.

## 🔧 사용된 하드웨어
OpenRB-150 보드

Dynamixel XL-330, XC-330 시리즈

HC-05 Bluetooth 모듈

Windows + Processing GUI 또는 Android 앱

## 📂 코드 구성
1. dynamixel_write.ino
역할: HC-05로부터 모터 위치 명령을 받아, 10개의 다이나믹셀 모터에 목표 위치(Goal Position)를 전달

특징:

Profile Velocity를 지정하여 일정 시간 내에 도달하게 제어

SyncWrite를 통해 다수의 모터에 동시에 위치 명령 전송

ID 5번, 10번은 OP_CURRENT_BASED_POSITION 모드로 설정

2. dynamixel_read.ino
역할: 10개 모터의 현재 위치(Present Position)를 SyncRead로 수신하여 HC-05로 송신

특징:

약 10ms 간격으로 위치 정보 전송

수신된 위치 정보는 ","로 구분된 문자열로 전송됨

## 📐 시스템 구조

[로봇 조작자 GUI/앱]
        ↓ Bluetooth
   [HC-05 모듈 (COM)]
        ↓ Serial
  [OpenRB-150 보드]
        ↓ DXL TTL
 [10개 Dynamixel 모터]
### 📘 매뉴얼
자세한 하드웨어 연결 방식, GUI 사용법, 시스템 설정 등은 아래 링크의 Figma 매뉴얼을 참고하세요:

▶️ SNU Telemanipulator Manual (Figma)
