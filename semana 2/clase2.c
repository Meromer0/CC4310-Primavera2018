#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>

void searchXip(FILE *fp, struct tm rango, char *dom);
void searchXdom(FILE *fp, struct tm rango, char *ip);

int main(int argc, char *argv[]) {
	FILE *fp;
	if (argc < 5) return -1;

	assert(argv[1]!=NULL);
	fp = fopen(argv[1], "r");
	assert(fp!=NULL);
	
	assert(argv[2]!=NULL);
	time_t t = time(NULL) - atoi(argv[2])*60;
	struct tm fecha_buscada = *localtime(&t);

	assert(argv[3]!=NULL);
	/*
	if (argv[3] == 'xip')  searchXip(fp,fecha_buscada,argv[4]);
	else if (argv[3] == 'xdom') searchXdom(fp,fecha_buscada,argv[4]);
	*/
	fclose(fp);
	return 0;
}
