## make file for libmempool

MAINFILE=libmempool.so
LIBVER=1.0.1
LIBFILE=$(MAINFILE).$(LIBVER)
SONAME=$(MAINFILE).1

MANPATH=/usr/local/man


all: $(LIBFILE)


LIBS=
ARGS=-Wall -g -O2
REQH=

# Need to be able to make 'man-pages' as well.  Not sure where to get the source for those... 

libmempool.o: mempool.c mempool.h $(REQH)
	gcc -c -fPIC mempool.c -o $@ $(ARGS)

libmempool.a: libmempool.o
	@>$@
	@rm $@
	ar rcs $@ $^

$(LIBFILE): libmempool.o
	gcc -shared -Wl,-soname,$(SONAME) -o $@ $^


lib-test: lib-test.c $(REQH) /usr/include/mempool.h
	gcc lib-test.c -o $@ -lmempool

install: $(LIBFILE) mempool.h
	cp mempool.h /usr/include/
	cp $(LIBFILE) /usr/lib/
	@-test -e /usr/lib/$(MAINFILE) && rm /usr/lib/$(MAINFILE)
	ln -s /usr/lib/$(LIBFILE) /usr/lib/$(MAINFILE)
	nice ldconfig
	@echo "Install complete."

man-pages: manpages/libmempool.3
	@mkdir tmp.install
	@cp manpages/* tmp.install/
	@gzip tmp.install/*.3
	cp tmp.install/*.3.gz $(MANPATH)/man3/
	@rm -r tmp.install	
	@echo "Man-pages Install complete."

clean:
	@-[ -e libmempool.o ] && rm libmempool.o
	@-[ -e libmempool.so* ] && rm libmempool.so*
	@-test -e lib-test && rm lib-test

