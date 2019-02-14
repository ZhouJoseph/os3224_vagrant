#include <stdio.h>
int g(int a){
	return a*11;
}

int f(int a, int b){
	return g(a) + g(b);
}

int main(void){
	printf("%d\n", f(10,20));
	return 0;
}

