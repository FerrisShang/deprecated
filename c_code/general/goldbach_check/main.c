#include "stdio.h"
#include "stdlib.h"
#include "time.h"

#define NUM_LIMIT 300000000

int primes_cnt;
int *primes;

int init_primes(void)
{
	long long i, j;
	char *prime_map = (char*)calloc(NUM_LIMIT, sizeof(char));
	if(prime_map == 0){
		return 0;
	}
	printf("Init primes map\n");
	for(i=2;i<NUM_LIMIT/2;i++){
		if(prime_map[i] == 0){
			for(j=i*i;j<NUM_LIMIT;j+=i){
				prime_map[j] = 1;
			}
		}
	}
	printf("Count primes number in %d\n", NUM_LIMIT);
	for(i=2;i<NUM_LIMIT;i++){
		if(prime_map[i] == 0){
			primes_cnt++;
		}
	}
	printf("Init primes number array\n");
	primes = (int*)malloc(primes_cnt * sizeof(int));
	if(primes == 0){
		return 0;
	}
	primes_cnt = 0;
	for(i=2;i<NUM_LIMIT;i++){
		if(prime_map[i] == 0){
			primes[primes_cnt] = i;
			primes_cnt++;
		}
	}
	free(prime_map);
	printf("Init primes number array success, total %d primes\n", primes_cnt);
	return primes_cnt;
}
int comp(const void *n1, const void *n2)
{
	return *(int*)n1 - *(int*)n2;
}
void primes_check(int start_num, int end_num, int print_flag)
{
	int i, num;
	for(num=start_num;num<=end_num;num+=2){
		int found_flag = 0;
		for (i=0; i<NUM_LIMIT; i++){
			// find difference by subtracting current prime from num
			int diff = num - primes[i];
			// Search if the difference is also a prime number
			if (bsearch(&diff, primes, primes_cnt, sizeof(int), comp)){
				if(print_flag == 1){
					printf("%d = %d + %d\n", num, primes[i], diff);
				}
				found_flag = 1;
				break;
			}
		}
		if(found_flag == 0){
			printf("ERROR:%d can't be decomposed into the sum of two prime numbers\n", num);
			return;
		}
	}
	return;
}
int main(int argc, char *argv[])
{
	clock_t start, finish;
	if(init_primes() == 0){
		printf("Not enough memory\n");
		return 0;
	}
	printf("Start to check\n");
	start = clock();
	primes_check(6, 30000, 0);
	finish = clock();
	printf("6~30000 check done, duration:%f seconds\n", (double)(finish-start)/CLOCKS_PER_SEC);
	start = clock();
	primes_check(6, 3000000, 0);
	finish = clock();
	printf("6~3000000 check done, duration:%f seconds\n", (double)(finish-start)/CLOCKS_PER_SEC);
	start = clock();
	primes_check(6, 300000000, 0);
	finish = clock();
	printf("6~300000000 check done, duration:%f seconds\n", (double)(finish-start)/CLOCKS_PER_SEC);
	free(primes);
	system("pause");
	return 0;
}
