
#ifndef TEST 
#define TEST


#define LV_OUTPUT 0
#define LV_INFO 1
#define LV_DEBUG 2
#define LOGLEVEL LV_DEBUG

#define assertTrue(x, msg, args...) \
	{\
		if (x == 1){\
			fprintf(stderr, "ERROR::Assertion::ExpectedZero, '%d'\n", x);\
			fprintf(stderr, msg, ##args);\
			exit(EXIT_FAILURE);\
		}\
	}


#define LOG(level, msg, args...) \
	{\
		if (level <= LOGLEVEL) {\
			printf("LOG::");\
			printf(msg, ##args);\
			printf("\n");\
		}\
	}


#endif
