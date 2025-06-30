# SNU_CED_2024_Teleop
서울대학교 CED 로봇팔 텔레오퍼레이션 프로젝트

이 프로젝트는 Dynamixel 모터와 HC-05 블루투스 모듈, OpenRB-150 보드를 이용하여 로봇팔을 원격 조작하는 시스템을 구현합니다.
두 개의 주요 스케치는 각각 모터 제어용 송신 스케치와 모터 상태 수신 스케치로 구성되어 있습니다.

## 🔧 사용된 하드웨어
OpenRB-150 보드

Dynamixel XL-330, XC-330 시리즈

HC-05 Bluetooth 모듈

## 📐 시스템 구조

[로봇 조작자 GUI/앱]
        ↓ Bluetooth
   [HC-05 모듈 (COM)]
        ↓ Serial
  [OpenRB-150 보드]
        ↓ DXL TTL
 [Dynamixel 모터]
### 📘 매뉴얼
자세한 하드웨어 연결 방식, GUI 사용법, 시스템 설정 등은 아래 링크의 Figma 매뉴얼을 참고하세요:

▶️ [SNU Telemanipulator Manual (Figma)](https://www.figma.com/design/mNXyuwHH3c6fjFySnpvfll/SNU_telemanipulator_manual?node-id=214-458&t=n6WVg75sW9KMmBMo-1)

