CFLAGS=-Wall -ggdb
OBJDIR=/etc/uvnc
repeater: repeater.o repeaterproc.o openbsd_stringfuncs.o iniparser.o readini.o repeaterutil.o 
	g++ $(CFLAGS) -o repeater repeater.o repeaterproc.o openbsd_stringfuncs.o iniparser.o readini.o repeaterutil.o 
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

clean:
	rm -f *.o repeater 

install:
	cp -R repeater /usr/sbin/uvncrepeatersvc
	cp -R uvncrepeater /etc/init.d/uvncrepeater
	@if test ! -d ${OBJDIR}; then \
                mkdir /etc/uvnc; \
        else :; fi
	cp -R uvncrepeater.ini /etc/uvnc/uvncrepeater.ini
	cat message
