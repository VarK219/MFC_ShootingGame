#pragma once

// 문자열
#include <iostream>
#include <string>

#include<math.h>
#include<time.h>

class Bird {
public:
	int kind = 0;
	int LorR = 0;
	bool catchstate = false;
	double speed[4] = { 1.0,1.4,1.7,2.0 }; // 새 속도
	int size[4][2] = { {104, 80},{104, 80},{80, 61},{80,61} }; // 새의 크기 (가로 대비 세로 1.3 배율)
	int originalSize[4][2] = { {104, 80},{104, 80},{80, 61},{80,61} }; // 새의 크기 (가로 대비 세로 1.3 배율)
	POINT l_xy = { rand() % 200, rand() % 600 }; // 왼쪽 소환 새 좌표
	POINT r_xy = { rand() % 200 + 800, rand() % 600 }; // 오른쪽 소환 새 좌표]
};