#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#define BLOCK_SIZE 4096

void searchDatebyBlock(time_t tiempo_buscado, FILE **fp, int salto);
void searchDatebyLine(time_t tiempo_buscado, FILE **fp);
void printRest(FILE **fp);
void searchXip(FILE **fp, char *ip);
void searchXdom(FILE **fp, char *dom);

int main(int argc, char *argv[]) {
	FILE *fp;

	assert(argv[1]!=NULL);
	fp = fopen(argv[1], "r");
	assert(fp!=NULL);
	
	assert(argv[2]!=NULL);
	time_t t = time(NULL) - atoi(argv[2])*60;
	fseek(fp, -BLOCK_SIZE, SEEK_END);
	searchDatebyBlock(t, &fp, 2);
	searchDatebyLine(t, &fp);

	
	if (argc < 4) {
		printRest(&fp);
		fclose(fp);
		return 0;		
	}
	assert(argv[3] != NULL);
	assert(argv[4] != NULL);
	if (strcmp(argv[3],"xip") == 0)  searchXip(&fp,argv[4]);
	else if (strcmp(argv[3],"xdom") == 0) searchXdom(&fp,argv[4]);
	else printRest(&fp);
	
	fclose(fp);
	return 0;
}

void searchDatebyBlock(time_t tiempo_buscado, FILE **fp, int salto) {	
	int year, month, day, hour, min;
	char rest[1024];
	size_t bufsize = 1024;
    char *buffer = (char *)malloc(bufsize * sizeof(char));
	size_t trash = getline(&buffer, &bufsize,*fp);
	free(buffer);
	if (0 < fscanf(*fp, "%d-%d-%d,%d:%d:%s", &year, &month, &day, &hour, &min, rest)) {
		//printf("%d-%d-%d,%d:%d:%s\n", year, month, day, hour, min, rest);
		struct tm time = {0};
		time.tm_year = year - 1900;
    	time.tm_mon = month - 1;
    	time.tm_mday = day;
    	time.tm_hour = hour;
    	time.tm_min = min;
    	time_t t = mktime(&time);

		if (t < tiempo_buscado) {
			printf("Terminamos de buscar el bloque\n");
			return;
		}
		fseek(*fp, -BLOCK_SIZE*salto, SEEK_END);
		searchDatebyBlock(tiempo_buscado, fp, salto*2);
	} else {
		printf("Llegamos al principio del archivo\n");
		fseek(*fp, 0, SEEK_SET);
	}
}

void searchDatebyLine(time_t tiempo_buscado, FILE **fp) {
	int year, month, day, hour, min;
	char rest[1024];
	while (fscanf(*fp, "%d-%d-%d,%d:%d:%s", &year, &month, &day, &hour, &min, rest) != EOF) {
		//printf("%d-%d-%d,%d:%d:%s\n", year, month, day, hour, min, rest);
		
		struct tm time = {0};
		time.tm_year = year - 1900;
    	time.tm_mon = month - 1;
    	time.tm_mday = day;
    	time.tm_hour = hour;
    	time.tm_min = min;
    	time_t t = mktime(&time);

		if (t > tiempo_buscado) {
			printf("Terminamos de buscar la linea\n");
			printf("%d-%d-%d,%d:%d:%s\n", year, month, day, hour, min, rest);
			return;
		}
					
	}
	printf("Llegamos al final del archivo\n");
}

void printRest(FILE **fp) {
	int year, month, day, hour, min, seconds, mili;
	char rest[1024];
	while (fscanf(*fp, "%d-%d-%d,%d:%d:%d.%d,%s", &year, &month, &day, &hour, &min, &seconds, &mili, rest) != EOF) {
		printf("%d-%d-%d,%d:%d:%d.%d,%s\n", year, month, day, hour, min, seconds, mili, rest);
	}
}

void searchXip(FILE **fp, char *ip) {
	int year, month, day, hour, min, seconds, mili;
	char rest[1024];
	while (fscanf(*fp, "%d-%d-%d,%d:%d:%d.%d,%s", &year, &month, &day, &hour, &min, &seconds, &mili, rest) != EOF) {
		char *xip = strtok(rest, ",");
		if (strcmp(ip, xip) == 0) printf("%d-%d-%d,%d:%d:%d.%d,%s,%s\n", year, month, day, hour, min, seconds, mili, xip, rest);
	}
}

void searchXdom(FILE **fp, char *dom) {
	int year, month, day, hour, min, seconds, mili;
	char rest[1024];
	while (fscanf(*fp, "%d-%d-%d,%d:%d:%d.%d,%s", &year, &month, &day, &hour, &min, &seconds, &mili, rest) != EOF) {
		char *xip = strtok(rest, ",");
		if (strcmp(dom, rest) == 0) printf("%d-%d-%d,%d:%d:%d.%d,%s,%s\n", year, month, day, hour, min, seconds, mili, xip, rest);

	}
}