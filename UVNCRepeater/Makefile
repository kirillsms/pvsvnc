CFLAGS=-Wall
OBJDIR=/etc/uvnc
repeater: repeater.o repeaterproc.o openbsd_stringfuncs.o iniparser.o readini.o repeaterutil.o repeaterevents.o
	g++ $(CFLAGS) -o repeater repeater.o repeaterproc.o openbsd_stringfuncs.o iniparser.o readini.o repeaterutil.o repeaterevents.o
	gcc $(CFLAGS) -o start-stop-daemon start-stop-daemon.c
repeater.o: repeater.cpp
	g++ $(CFLAGS) -c repeater.cpp

repeaterproc.o: repeaterproc.cpp
	g++ $(CFLAGS) -c repeaterproc.cpp

openbsd_stringfuncs.o: openbsd_stringfuncs.cpp
	g++ $(CFLAGS) -c openbsd_stringfuncs.cpp

iniparser.o: iniparser.cpp
	g++ $(CFLAGS) -c iniparser.cpp

readini.o: readini.cpp
	g++ $(CFLAGS) -c readini.cpp

repeaterutil.o: repeaterutil.cpp
	g++ $(CFLAGS) -c repeaterutil.cpp

repeaterevents.o: repeaterevents.cpp
	g++ $(CFLAGS) -c repeaterevents.cpp

clean:
	rm -f *.o repeater start-stop-daemon

install:
	cp -R repeater /usr/sbin/uvncrepeatersvc
	cp -R start-stop-daemon /usr/sbin/start-stop-daemon
	cp -R uvncrepeater /etc/init.d/uvncrepeater
	@if test ! -d ${OBJDIR}; then \
                mkdir /etc/uvnc; \
        else :; fi
	cp -R uvncrepeater.ini /etc/uvnc/uvncrepeater.ini
	cat message
