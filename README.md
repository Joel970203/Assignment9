# 🎮 [9번 과제] 숫자 야구 게임

Unreal Engine 기반의 멀티플레이 숫자 야구 게임입니다.  
Listen Server 구조를 활용하여 호스트와 게스트가 실시간으로 게임을 플레이하며, 제한된 턴 안에 정답을 유추하는 턴제 게임입니다.

---

## 🌐 네트워크 구조

- **Listen Server 구조**
  - 호스트는 서버와 클라이언트 역할을 동시에 수행합니다.
  - 클라이언트는 해당 서버에 접속해 플레이합니다.

- **통신 방식**
  - `Client → Server`: `Server_SendInputToGameMode()` (RPC)
  - `Server → Client`: `Client_DisplayResult()` (RPC)
  - 채점 결과 및 시스템 메시지는 모든 플레이어에게 동기화됩니다.

---

## 🧠 게임 규칙

- 서버는 중복 없는 3자리 정답을 생성합니다. (예: 123, 847)
- 플레이어는 `/123` 형식으로 입력합니다.
- 채점 방식:
  - `3S0B`: 3 스트라이크 (정답)
  - `1S2B`: 1 스트라이크, 2 볼
  - `OUT`: 스트라이크/볼 없음
  - `'Invalid Input'`: 잘못된 입력, 횟수는 차감됨

---

## 🔄 게임 흐름

### 1. 게임 시작
- 서버가 정답 생성
- ScoreBoard 및 타이머 초기화

### 2. 턴 제어
- 호스트 → 게스트 순서로 교대로 입력
- 턴이 아닐 경우 입력은 무시됨

### 3. 타이머 기능
- 제한 시간: 10초
- 시간 초과 시 자동으로 실패 처리 (횟수 차감)

### 4. 승패 조건
- 먼저 `3S` 입력한 플레이어가 즉시 승리
- 승자는 점수 +1 획득 → 게임 자동 리셋
- 두 플레이어가 모두 3번 실패 시 무승부 → 게임 리셋

---

## 🧪 주요 기능

- [x] 서버에서 정답 생성 및 채점
- [x] Client → Server로 메시지 전달
- [x] Server → Client로 결과 브로드캐스트
- [x] 플레이어별 시도 횟수 제한
- [x] 턴 제어 시스템
- [x] 제한 시간 타이머 및 자동 실패 처리
- [x] ScoreBoard 동기화
- [x] 위젯 기반 결과 출력
- [x] 승패 출력 후 2초 뒤 UI 자동 초기화 및 리게임

---

## 📷 UI 구성 (UMG)

| 텍스트 박스 번호   | 용도                             |
|--------------------|----------------------------------|
| EditableTextBox_91 | 플레이어 입력 텍스트 박스        |
| EditableTextBox_92 | 결과 로그 출력 텍스트 박스       |
| EditableTextBox_93 | ScoreBoard (Guest / Host 점수)   |
| EditableTextBox_94 | 제한 시간 출력 박스              |

---

## 🏗️ 구현 과정

### ✅ 입력 UI → 서버 전달
- `Widget_BaseBall`에서 `/123` 입력을 감지
- `BaseBallController`의 `Server_SendInputToGameMode()` RPC 호출
- `GameMode`에서 판정 및 로직 처리

### ✅ GameMode 로직
- `GenerateRandomNumber()`로 정답 생성
- `ProcessPlayerInput()`에서 정답과 비교
- `3S`이면 승리 처리, 실패 시 TryCounts 증가
- `ProcessPlayerInputFromUI()`에서 결과 브로드캐스트 처리

### ✅ 결과 표시
- `Client_DisplayResult()`에서 결과를 각 클라이언트 위젯에 출력
- 메시지 형식 예시: Host 1 guess : "147" -> 1S2B


### ✅ 턴 제어 및 제한 시간
- `CurrentTurnPlayer` 변수를 기준으로 입력 유효성 체크
- 입력 성공 시 턴 전환
- `TurnTimerHandle`로 타이머 설정, 초과 시 자동 실패

### ✅ 승리 / 무승부 처리
- `3S`가 나오면 바로 `WIN!!` 메시지 출력 및 점수 업데이트
- 둘 다 3번 실패 시 `DRAW!!` 메시지 출력
- 2초 후 `Client_ClearChatBox()` 호출로 UI 초기화

### ✅ ScoreBoard 동기화
- `EditableTextBox_93`에 호스트와 게스트 점수 출력
- 매 게임 시작 시 클라이언트들에게 점수 전송 (`Client_UpdateScore()`)

---
