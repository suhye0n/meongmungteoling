# 멍뭉터링

### 멍뭉터링 소개
강아지는 체온이 건강의 중요한 척도입니다. 반려동물 앱이 시중에 많이 나와 있으나, 실시간으로 강아지의 체온을 모니터링하여 알려주는 앱은 아직 출시되지 않았습니다. 대처보다 예방이 중요한 만큼, 강아지의 체온 상태를 실시간으로 모니터링하여 알려주는 앱과 강아지 가방을 만들고자 하였습니다.

### 멍뭉터링 기능
- 애견 체온 측정
- 애견 체온 상태를 앱 및 가방의 LED로 확인 가능
- 앱에서 강아지 야간 산책 시 안전을 위한 LED ON/OFF 제어 가능
- 앱에서 강아지 심신 안정을 위한 멜로디 ON/OFF 제어 가능 

### 전체 시스템 구조
![image](https://user-images.githubusercontent.com/63187994/208288582-7726426d-6389-43cc-ad2c-fc324c4ee1e4.png)

### 개발 일정

### 업무 분장
- 나현진: 재료 준비와 발표 및 마감 보조
- 박수현: 안드로이드 어플리케이션 및 소켓 통신 구현
- 전예지: 회로 연결 및 서버 측 함수 구현(온도 제어 함수, 조도 관련 함수, 멀티쓰레딩, 뮤텍스)

### 개발 시 문제점 및 해결 방안
- 소켓 바인드 에러
  - 문제점: 소켓을 재연결 할 시 경우에 따라 바인드 에러가 뜸
  - 해결 방안: 해당 포트를 이미 사용하고 있어서 발생하는 문제로, 라즈베리파이를 재부팅하여 해결함
- 소켓 데이터 전송 문제
  - 문제점: 소켓 통신 시 정수형으로 값을 전송할 때 값이 제대로 전송되지 않는 문제 발생
  - 해결 방안: 정수값을 shift 연산을 통하여 byte 형식으로 바꾸어 전송하여 해결함

### 데모 영상
https://user-images.githubusercontent.com/63187994/208288839-12356be1-f429-441b-b340-d91ad5f53343.mp4
