#pragma once

// ���ڿ�
#include <iostream>
#include <string>

#include<math.h>
#include<time.h>

class Bird {
public:
	int kind = 0;
	int LorR = 0;
	bool catchstate = false;
	double speed[4] = { 1.0,1.4,1.7,2.0 }; // �� �ӵ�
	int size[4][2] = { {104, 80},{104, 80},{80, 61},{80,61} }; // ���� ũ�� (���� ��� ���� 1.3 ����)
	int originalSize[4][2] = { {104, 80},{104, 80},{80, 61},{80,61} }; // ���� ũ�� (���� ��� ���� 1.3 ����)
	POINT l_xy = { rand() % 200, rand() % 600 }; // ���� ��ȯ �� ��ǥ
	POINT r_xy = { rand() % 200 + 800, rand() % 600 }; // ������ ��ȯ �� ��ǥ]
};