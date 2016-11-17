.PHONY: all clean

all:
	$(MAKE) -C libhttpd
	$(MAKE) -C src

clean:
	$(MAKE) -C libhttpd clean
	$(MAKE) -C src clean

