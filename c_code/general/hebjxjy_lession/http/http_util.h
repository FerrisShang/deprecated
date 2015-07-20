#ifndef __HTTP_UTIL_H__
#define __HTTP_UTIL_H__

#define HTTP_BUF_LEN 4096
#define RETURN_STR "\r\n"
#define AGENT_ARR "Mozilla/5.0 (Windows NT 5.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/33.0.1750.112 Safari/537.36"
#define COOKIE_PREFIX "JSESSIONID="
#define COOKIE_LEN 43
#define SERVER_IP "115.28.82.250"
#define SERVER_PORT 80

struct http_handle{
	char *buf;
	char cookie[COOKIE_LEN+1];
	char check_code[5];
	int buf_len;
};

struct http_handle *create_http_handle(void);
void destory_http(struct http_handle* hhttp);
int create_http_str(struct http_handle *hhttp, 
		char *get,
		char *host,
		char *accept,
		char *agent,
		char *ref,
		char *accept_code,
		char *accept_lan,
		char *cookie,
		char *post,
		char *len,
		char *origin,
		char *xref,
		char *text
		);

int is_http_recv_done(char *buf, int buf_len);
char *set_cookie(struct http_handle *hhttp, char *buf);
char *get_check_code(struct http_handle *hhttp, char *buf, int buf_len);

#endif /* __HTTP_UTIL_H__ */
