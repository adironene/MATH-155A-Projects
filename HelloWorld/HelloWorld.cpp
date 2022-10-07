#include <stdio.h>
int main()
{
	printf("Hello World.\n");
	int i = 0;
	for (int j = 0; j < 7; j++) {
		i += j * j;
	}
	printf("i=%d.\n", i);
	getc(stdin);
}