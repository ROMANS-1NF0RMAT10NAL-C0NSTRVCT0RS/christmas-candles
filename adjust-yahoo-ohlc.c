#include <die.h>
#include <stdio.h>

int main(){
	unsigned int year,month,day;
	unsigned long volume;
	float open,high,low,close,adj_close,f;
	while(!feof(stdin)){
		if(fscanf(stdin,"%u-%u-%u,%f,%f,%f,%f,%lu,%f\n",&year,&month,&day,&open,&high,&low,&close,&volume,&adj_close)!=9) break;
		f=adj_close/close;
		printf("%u-%u-%u,%f,%f,%f,%f,%lu\n",year,month,day,open*f,high*f,low*f,adj_close,volume); /* don't know for sure but it appears yahoo already has the volume adjusted */}
	return 0; }

/* IN GOD WE TRVST. */
