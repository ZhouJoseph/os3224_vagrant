// Copyright @ Kaixuan Zhou 2019
// Intro to OS HW1
// uniq for xv6

#include "types.h"
#include "stat.h"
#include "user.h"
int maxSize = 1024;
char *prevline, *curline;
int cflag = 0, dflag = 0, iflag = 0;

// porc == 1: curline
// porc == 0: prevline
char* resize_buf(char* buf, int porc){
	maxSize *= 2;
	char* res = strcpy(malloc(sizeof(char)*maxSize),buf);
	free(buf);
	if(porc == 1){curline = res;}
	else{prevline = res;}
	return res;
}

// fd = file descriptor
// cf = check file
// porc == 1: curline
// porc == 0: prevline
int getline(int fd, char * buf, int count, int porc){
	int i = 0;
	int cf; char c;
	while(i < count){
		cf = read(fd, &c, 1);
		if(cf < 1){
			if(i != 0){ 
				buf[i++] = '\n';
				break; 
			}
			return (-1);
		}
		buf[i++] = c;
		if(c == '\n'){ break; }
		if(i == count - 1){
			count *= 2;
			maxSize = count;
			buf = resize_buf(buf, porc);
		}
	}
	buf[i] = '\0';
	return i;
}


void uniq_print(char* line, int num_seen){
	if(cflag){
		printf(1, "%4d %s", num_seen, line);
	}else{
		printf(1, "%s", line);
	}
}

char to_lower(char ch) {
	return (ch <= 'Z' && ch >= 'A') ? ch + 32 : ch;
}

// case insensitive check for strcmp
// return equal ? 0 else some_int
int strcmpCase(const char* p, const char* c){
	const char* s1 = p;
	const char* s2 = c;
	while(*s1 != '\0'){
		int d = to_lower(*s1++) - to_lower(*s2++);
		if(d){ return d; }
	}
	if(*s2 == '\0'){ return 0; }
  	return 1;
}


int not_equal(char* p, char* c){
	// iflag -- case insensitive
	return (iflag) ? strcmpCase(p, c) : strcmp(p,c);
}

void uniq(int fd){
	char* swap;
	int num_seen = 1;
	prevline = malloc(sizeof(char)*maxSize);
	if( getline(fd,prevline,maxSize,0) < 0 ){ free(prevline);exit(); }
	curline = malloc(sizeof(char)*maxSize);
	while( getline(fd,curline,maxSize,1) > 0 ){
		if(not_equal(prevline,curline)){
			// for dflag, we only print lines that appear multiple times
			if((dflag && num_seen > 1) || !dflag){ uniq_print(prevline,num_seen); }
			// Swap and reset num_seen
			swap = curline;
			curline = prevline;
			prevline = swap;
			num_seen = 1;
		}else{
			num_seen++;
		}
	}
	if((dflag && num_seen > 1) || !dflag)
		uniq_print(prevline,num_seen);
	free(prevline); free(curline);
}

void usage(){
	printf(2, "%s\n%s\n%s\n%s\n%s\n", 
			"[USAGE]: uniq [-c] [-d] [-i] filename",
			"[USAGE]: -c and -d flag cannot be used at the same time",
			"[FLAG]-c --count: display the counts for each line",
			"[FLAG]-d --duplicate: only display duplicate lines",
			"[FLAG]-i --ignore: perform case insensitive uniq"
		);
}

int main(int argc, char * argv[]){
	int fd = 0;
    int i = 1;
    for(; i < argc; ++i)
    {
    	if(argv[i][0] == '-')
    	{
    		switch(argv[i][1])
    		{
    			case 'c': cflag = 1; continue;
    			case 'd': dflag = 1; continue;
    			case 'i': iflag = 1; continue;
    			default: 
    				usage(); 
    				exit();
    		}
    	}
    	else{
    		if((fd = open(argv[i], 0)) < 0)
    		{
		        printf(1, "[WARNING] uniq: cannot open %s\n", argv[i]);
		        usage();
				exit();
		    }
    	}
    }
    if(cflag && dflag){usage();exit();}
	uniq(fd);
	if(fd){ close(fd); }
	exit();
}