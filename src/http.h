#ifndef _HTTP_H_
#define _HTTP_H_
void http_callback_404(httpd *webserver, request *r);
void send_http_page(request * r, const char *title, const char *message);

#endif /* _HTTP_H_ */