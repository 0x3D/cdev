#include <stdio.h>


void f(int x,double* xsquared, double* xcubed){
	*xsquared = x*x;
	*xcubed = x*x*x;
}

int main(int argc, char **argv)
{
	float x = 5.0f;
	double xsquared, xcubed;
	f(x, &xsquared, &xcubed);
	printf("x = %f, x^2 = %f, x^3 = %f\n", x, xsquared, xcubed);
	return 0;
}
