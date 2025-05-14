# 🎛️ RC 조종기 기반 RGB LED & 밝기 제어 시스템

본 프로젝트는 RC 조종기의 3채널(PWM 신호)을 활용하여 RGB 색상, 밝기, LED ON/OFF를 실시간으로 제어하는 아두이노 기반 조명 제어 시스템입니다.
https://youtu.be/qZboEh_sRr8?si=mOmtKh2dSuq_Y3n5

---

## 🔧 기능 요약

| 채널 | 기능 설명                         | 핀 번호 |
|------|----------------------------------|---------|
| CH1  | RGB 색상 제어 (Hue → HSV 변환)   | A2      |
| CH3  | 밝기 조절 (0~255, PWM)           | A0      |
| CH5  | LED ON/OFF 스위치                | A1      |

---

## 🛠 사용 부품

- 아두이노 UNO (또는 호환 보드)
- RC 수신기 (PWM 출력)
- RGB LED (또는 3색 LED)
- 일반 LED
- 220Ω 저항 × 4개
- 점퍼선 및 브레드보드

---

## 🔌 회로 연결

### 📥 입력 (RC 수신기 → 아두이노)
- CH1 → A2 (색상 제어)
- CH3 → A0 (밝기 제어)
- CH5 → A1 (스위치)

### 💡 출력 (아두이노 → LED)
- D3 → RGB R 핀
- D5 → RGB G 핀
- D10 → RGB B 핀
- D9 → 일반 LED (밝기 제어용 PWM)
- D6 → LED ON/OFF 제어 핀

> ⚠️ 모든 LED 핀에는 저항(220Ω)을 직렬로 연결해 주세요.

---

## 📡 작동 방식

1. RC 조종기에서 PWM 신호 입력 (1000μs ~ 2000μs)
2. `PinChangeInterrupt` 라이브러리로 각 채널의 펄스 폭 측정
3. 펄스 폭에 따라 다음 제어 수행:
   - **CH1** → HSV 색상(Hue) 계산 후 RGB 출력
   - **CH3** → 밝기 값으로 변환 후 PWM 출력
   - **CH5** → 스위치 역할, LED 전체 ON/OFF

---

## 🧠 주요 알고리즘

- `micros()`를 사용하여 펄스폭 측정
- `map()`과 `constrain()`으로 PWM 값 보정
- HSV → RGB 변환 수식 사용 (`hsvToRgb()` 함수)
- 입력 변화 감지를 위한 인터럽트 핸들러 사용

---

## 🖥 시리얼 출력 예시

```text
CH5 A Switch: ON → LEDs ENABLED (Brightness Restored)
CH3 Brightness: 178
CH1 Hue: 124.80 → RGB(0, 255, 83)
