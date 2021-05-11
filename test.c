#include "parser.h"

int main(int argc,char* argv[]) {
	char str[] = "float f_2 = 1.37 + fn(-4) - (2.2*-3)/-5.4e-2;\nif(f<=>3) { printf(\"foo\\n\"); } char c = 'c'; char s2[] = \"+2\";\nint i = 1 << 4;";
	
	printf(">>>>\n%s\n<<<<", str);
	
	token* ts = scan(str);
	
	int i = 0;
	while(ts[i].len != 0) {
		printf("token %d: '%.*s'\n", i, ts[i].len, ts[i].start);
		i++;
	}
	
	printf("done.\n");
	
	return 0;
}

