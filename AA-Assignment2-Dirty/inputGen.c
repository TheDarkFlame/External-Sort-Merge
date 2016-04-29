#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#define MAX_VALUE 1000 //maximum value rand can take
int main(int argc, char * argv[]){
	if (argc>2)
		printf("invalid input, use:\ninputGen <input count>");
	long int runCount = strtol(argv[1],NULL,10);
	srand(time(NULL));

	FILE *fp = fopen("input.txt","w");
	int i;
	int j=1;
	for(i=0;i<runCount;i++){
		fprintf(fp,"%03d ",rand()%MAX_VALUE);
        fprintf(fp,"%03d\n",rand()%MAX_VALUE);
		if(i%(runCount/100)==0)
			printf("%d%% complete\n",j++);
	}
	fclose(fp);
	system("pause");
return 0;
}
