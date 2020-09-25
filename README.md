# ShitHaneul
**싙하늘**은 [서동휘](https://github.com/suhdonghwi)님이 개발하신 한글 프로그래밍 언어 [누리](https://github.com/suhdonghwi/nuri)의 서드파티 C++ 백엔드입니다.

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
누리 레포지토리에서 제공되는 예제를 [hyperfine](https://github.com/sharkdp/hyperfine)으로 준비 3회 후 10회 평균 실행 시간을 측정하였습니다(누리 0.2.0 - 싙하늘 0.2.0 기준).

|이름|하늘|싙하늘(gcc)|싙하늘(clang)|
|:-:|:-:|:-:|:-:|
|closure.hn|6.3ms|6.6ms|6.5ms|
|factorial.hn|6.3ms|6.7ms|6.6ms|
|fibonacci.hn|1.433s|3.214s|2.708s|
|list_sort.hn|3.026s|2.272s|2.252s|
|list_sum.hn|8.6ms|7.6ms|7.4ms|
|local_fibonacci.hn|1.129s|3.074s|2.694s|
|map.hn|6.2ms|6.7ms|6.6ms|
|odd_even.hn|11.4ms|7.7ms|7.5ms|
|scope.hn|6.2ms|6.6ms|6.6ms|
|string_concat.hn|6.2ms|6.6ms|6.6ms|
|struct.hn|6.3ms|6.5ms|6.6ms|
|sum.hn|6.4ms|6.7ms|6.6ms|
|synonym_antonym.hn|6.2ms|7ms|6.5ms|

측정 환경은 다음과 같습니다.

|CPU|RAM|저장 장치|운영체제|
|:-:|:-:|:-:|:-:|
|i7-8700K|16GB 2666MHz|삼성 SSD 840|Windows 10 20H1 - Ubuntu WSL1|

## 라이선스
- 싙하늘의 소스 코드는 [GPLv3](https://github.com/kmc7468/ShitHaneul/blob/master/LICENSE) 라이선스에 의해 보호받습니다.
- [하늘](https://github.com/suhdonghwi/haneul)에서 그대로 포팅된 알고리즘은 서동휘님에게 저작권이 있습니다.