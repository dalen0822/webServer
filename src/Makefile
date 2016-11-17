.PHONY: all clean

INCLUDES=-I . \
		 -I ../libhttpd

LIBS=-L../libhttpd -lhttpd -lpthread

CC_FLAGS=-O0 -fPIC 

CXX_INCLUDES= 

CXX_LIBS= 

CXX_FLAGS=-O2 -fPIC -DHAVE_NETINET_IN_H -DHAVE_INTTYPES_H -std=c++0x

SOURCES=safe.c threadpool.c webServer.c http.c unit.c

OBJECTS=safe.o threadpool.o webServer.o http.o unit.o


TARGET=webServer 

all:${SOURCES}  ${OBJECTS}
	${CC} ${CC_FLAGS} -o $(TARGET) ${OBJECTS}  ${LIBS} ${CXX_LIBS} ${LD_FLAGS}
	cp -f ${TARGET} ../bin

clean:
	rm -rf ${OBJECTS} ${TARGET} *~

${OBJECTS}: %.o: %.c
	$(CC) -c $(CC_FLAGS) $< -o $@ ${INCLUDES}


