#ifndef DEV_UTILS_C
#define DEV_UTILS_C


#define RECORD_TIME(IDENTIFIER) const clock_t IDENTIFIER = clock(); 
#define MEASURE_TIME(IDENTIFIER, MESSAGE) printf("%s :: %s\ntempo em segundos %f\n", #IDENTIFIER, MESSAGE, ((double) (clock() - IDENTIFIER))/CLOCKS_PER_SEC);

#endif //  DEV_UTILS_C
