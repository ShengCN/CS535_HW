#pragma once

#include <iostream>

using namespace std;


class V3 {
public:
	float xyz[3];
	V3() {};
	V3(float x);
	V3(float x, float y, float z);
	V3 Normalize();
	float Length();
	V3 cross(V3 v1);
	float& operator[](int i);
	float operator*(V3 v1);
	V3 operator*(float scf);
	V3 operator/(float scf);
	V3 operator-(V3 v1);
	bool operator==(V3 v1);
	bool operator!=(V3 v1);
	friend ostream& operator<<(ostream& ost, V3 v);
};