#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#if 0
struct mem_record{
	int addr;
	char func[32];
	int line;
	int state;
};

struct mem_record mem_record[4096];
int mem_record_cnt;

void add_rec(int addr, const char *func, int line)
{
	struct mem_record *p;
	int i;
	for(i=0;i<mem_record_cnt;i++){
		if(addr == mem_record[i].addr){
			mem_record[i].state++;
			return;
		}
	}
	p = &mem_record[mem_record_cnt];
	p->addr = addr;
	memcpy(p->func, func, 31);
	p->line = line;
	mem_record[i].state = 1;
	mem_record_cnt++;
}
void rm_rec(int addr)
{
	int i;
	for(i=0;i<mem_record_cnt;i++){
		if(addr == mem_record[i].addr){
			mem_record[i].state--;
		}
	}
}
void mem_dump(void)
{
	int i;
	printf("total:%d\n", mem_record_cnt);
	for(i=0;i<mem_record_cnt;i++){
		if(mem_record[i].state != 0){
			printf("@\t%08x\t%d\t%s\t%d\n", mem_record[i].addr, 
					mem_record[i].state, mem_record[i].func, mem_record[i].line);
		}
	}
}

void* _mem_malloc(int size, const char *func, int line)
{
	void *p;
	p = malloc(size);
	printf("%s@%d => malloc:%8p\n", func, line, p);
	add_rec((int)p, func, line);
	return p;
}

void* _mem_calloc(int size, int num, const char *func, int line)
{
	void *p;
	p = calloc(size, num);
	printf("%s@%d => calloc:%8p\n", func, line, p);
	add_rec((int)p, func, line);
	return p;
}

void _mem_free(void *ptr, const char *func, int line)
{
	rm_rec((int)ptr);
	free(ptr);
	printf("%s@%d => free:%8p\n", func, line, ptr);
	ptr = 0;
}

void mem_free_cb(void *ptr)
{
	rm_rec((int)ptr);
	free(ptr);
	ptr = 0;
}
#endif

