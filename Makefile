# swaprootname - root window name swapper.

include config.mk

SRC = swaprootname.c
OBJ = ${SRC:.c=.o}

all: options swaprootname

options:
	@echo swaprootname build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"

.c.o:
	@echo CC $<
	@${CC} -c ${CFLAGS} $<

${OBJ}: config.mk

swaprootname: ${OBJ}
	@echo CC -o $@
	@${CC} -o $@ ${OBJ} ${LDFLAGS}

clean:
	@echo cleaning
	@rm -f swaprootname ${OBJ} swaprootname-${VERSION}.tar.gz

dist: clean
	@echo creating dist tarball
	@mkdir -p swaprootname-${VERSION}
	@cp -R LICENSE Makefile README config.mk ${SRC} swaprootname-${VERSION}
	@tar -cf swaprootname-${VERSION}.tar swaprootname-${VERSION}
	@gzip swaprootname-${VERSION}.tar
	@rm -rf swaprootname-${VERSION}

install: all
	@echo installing executable file to ${DESTDIR}${PREFIX}/bin
	@mkdir -p ${DESTDIR}${PREFIX}/bin
	@cp -f swaprootname ${DESTDIR}${PREFIX}/bin
	@chmod 755 ${DESTDIR}${PREFIX}/bin/swaprootname

uninstall:
	@echo removing executable file from ${DESTDIR}${PREFIX}/bin
	@rm -f ${DESTDIR}${PREFIX}/bin/swaprootname

.PHONY: all options clean dist install uninstall
