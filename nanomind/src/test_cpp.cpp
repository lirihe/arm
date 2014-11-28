/*
 * test_cpp.cpp
 *
 *  Created on: 19/07/2011
 *      Author: johan
 */

#include <stdio.h>

#if ENABLE_CPP

using namespace std;

class test_rectangle {
	int *width, *height;
public:
	test_rectangle(int a, int b) {
		printf("Constructor overload with arguments\r\n");
		width = new int;
		height = new int;
		*width = a;
		*height = b;
	}
	test_rectangle() {
		printf("Constructor default \r\n");
		width = new int;
		height = new int;
		*width = 10;
		*height = 20;
	}
	~test_rectangle() {
		printf("Destructor\r\n");
		delete width;
		delete height;
	}
	int area() {
		return (*width * *height);
	}
};

test_rectangle rectb;

extern "C" void test_cpp_call(void) {
	printf("C-function calling C++ object\r\n");
	test_rectangle recta(3, 4);
	printf("recta area: %u\r\n", recta.area());
	printf("rectb area: %u\r\n", rectb.area());
}

#endif
