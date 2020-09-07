# ShitHaneul
**싙하늘**은 [서동휘](https://github.com/suhdonghwi)님이 개발하신 한글 프로그래밍 언어 [누리](https://github.com/suhdonghwi/haneul)의 서드파티 C++ 백엔드입니다.

## 사용법
```
$ ./ShitHaneul <입력: 하늘 바이트 파일(*.hn)>
```

## 컴파일
```
$ git clone https://github.com/kmc7468/ShitHaneul.git
$ cd ShitHaneul
$ cmake .
$ cmake --build .
```

## 성능
누리 레포지토리에서 제공되는 예제를 [hyperfine](https://github.com/sharkdp/hyperfine)으로 준비 3회 후 10회 평균 실행 시간을 측정하였습니다(싙하늘 0.1.0 기준).

|이름|하늘|싙하늘(gcc)|싙하늘(clang)|
|:-:|:-:|:-:|:-:|
|closure.hn|6.4ms|6.7ms|6.7ms|
|factorial.hn|6.3ms|6.9ms|6.8ms|
|fibonacci.hn|1.267s|3.33s|2.908s|
|list_sum.hn|6.8ms|6.8ms|7ms|
|local_fibonacci.hn|1.005s|3.031s|2.6s|
|map.hn|6.3ms|6.9ms|6.8ms|
|odd_even.hn|17.7ms|7.9ms|7.8ms|
|scope.hn|6.2ms|6.9ms|6.8ms|
|string_concat.hn|6.2ms|7.2ms|7ms|
|struct.hn|6.5ms|7ms|6.8ms|
|sum.hn|6.3ms|7.3ms|6.8ms|

측정 환경은 다음과 같습니다.

|CPU|RAM 용량|저장 장치|운영체제|
|:-:|:-:|:-:|:-:|
|i7-8700K|16GB|삼성 SSD 840|Windows 10 19H2 - Ubuntu WSL1|

## 라이선스
- 싙하늘의 소스 코드는 [GPLv3](https://github.com/kmc7468/ShitHaneul/blob/master/LICENSE) 라이선스에 의해 보호받습니다.
- [하늘](https://github.com/suhdonghwi/haneul)에서 그대로 포팅된 알고리즘은 서동휘님에게 저작권이 있습니다.