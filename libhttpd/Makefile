.PHONY: all clean

INCLUDES=-I .

CC_FLAGS +=-O0 -fPIC -Wall -DHAVE_STDARG_H

OBJECTS= api.o \
		 protocol.o \
		 ip_acl.o \
		 version.o 

TARGET=libhttpd

all:${OBJECTS}
	$(AR) cr $(TARGET).a ${OBJECTS} $(LIBS) $(LD_FLAFS)

clean:
	rm -rf ${OBJECTS} ${TARGET}.a
	
 
${OBJECTS}: %.o: %.c
	$(CC) $(CC_FLAGS) -c $<  ${INCLUDES}
