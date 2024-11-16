##############################################################################
#                                                                            #
#    LLNL XFTP's makefile (to be used if you just can't get Imake to work).  #
#                                                                            #
#    Uncomment the section that looks most promising.  You will              #
#    probably need to reassign INCLUDE_DIRS and LIB_DIRS.                    #
#                                                                            #
#    See "SOCKS SUPPORT" section below if you wish to SOCKSify LLNL XFTP.    #
#                                                                            #
##############################################################################


CC = cc
CFLAGS = -O2 -ansi -D_POSIX_C_SOURCE=2 -D_XOPEN_SOURCE=500
INCLUDE_DIRS = -I/usr/local/include
LIBS = -lXm -lXt -lX11
LIB_DIRS = -L/usr/local/lib


#############################################################################
#                                                                           #
#                              SOCKS SUPPORT                                #
#                                                                           #
#    LLNL XFTP can be built to utilize the SOCKS firewall package, but      #
#    we make no guarantees.  Use no SOCKS library earlier than version      #
#    4.2, and make sure that the library is built with -DSHORTENED_RBIND.   #
#    The SOCKS distribution can be obtained from                            #
#                                                                           #
#               ftp://ftp.nec.com/pub/security/socks/cstc                   #
#                                                                           #
#############################################################################

SOCKS_FLAGS =
SOCKS_LIBS =

#
# To SOCKSify LLNL XFTP, uncomment these two lines:
#
#SOCKS_FLAGS = -DSOCKS -Dconnect=Rconnect -Dgetsockname=Rgetsockname -Dbind=Rbind -Daccept=Raccept -Dlisten=Rlisten -Dselect=Rselect
#SOCKS_LIBS = -lsocks



#############################################################################
#                                                                           #
#    Set the following five lines to appropriate values (e.g., you might    #
#    want to set MANSUFFIX to "1" for HP-UX platforms).                     #
#                                                                           #
#############################################################################

BINDIR = /usr/local/bin
MANDIR = /usr/local/man
MANSUFFIX = n
INSTALLED_MAN = $(MANDIR)/man$(MANSUFFIX)/xftp.$(MANSUFFIX)
INSTALLED_APPDEFS = /usr/lib/X11/app-defaults/XFtp

INCLUDES =	errorflag.h		list.h			logo50.h		logo64.h	\
			okflag.h		pixmaps.h		prefs.h			pulldown.h	\
			rconnect.h		startup.h		str.h			xfer.h		\
			xftp.h

SOURCES =	abort.c			aconnect.c		account.c		arconnect.c	\
			cache.c			changedir.c		cmds.c			color.c		\
			ctrlconn.c		cursor.c		dataconn.c		delete.c	\
			destdir.c		dirlist.c		dirlong.c 		dirname.c	\
			dirpath.c		dirtable.c		fallbacks.c		flag.c		\
			force.c			get.c			goto.c			help.c		\
			host.c			lconnect.c		lcopy.c			list.c		\
			local.c			log.c			ls.c			mainhelp.c	\
			match.c			misc.c			mkdir.c			move.c		\
			mpmonitor.c		password.c		path.c			pixmaps.c	\
			prefs.c			prefshelp.c		pulldown.c		put.c		\
			qidialogs.c		qsort.c			quote.c			rconnect.c	\
			rcopy.c			remote.c		rename.c		select.c	\
			server.c		startup.c		system.c		toplev.c	\
			verify.c		view1.c			view2.c			viewprefs.c	\
			xfer1.c			xfer2.c			xfer3.c			xfer4.c		\
			xfermon.c		xftp.c

OBJECTS =	abort.o			aconnect.o		account.o		arconnect.o	\
			cache.o			changedir.o		cmds.o			color.o		\
			ctrlconn.o		cursor.o		dataconn.o		delete.o	\
			destdir.o		dirlist.o		dirlong.o 		dirname.o	\
			dirpath.o		dirtable.o		fallbacks.o		flag.o		\
			force.o			get.o			goto.o			help.o		\
			host.o			lconnect.o		lcopy.o			list.o		\
			local.o			log.o			ls.o			mainhelp.o	\
			match.o			misc.o			mkdir.o			move.o		\
			mpmonitor.o		password.o		path.o			pixmaps.o	\
			prefs.o			prefshelp.o		pulldown.o		put.o		\
			qidialogs.o		qsort.o			quote.o			rconnect.o	\
			rcopy.o			remote.o		rename.o		select.o	\
			server.o		startup.o		system.o		toplev.o	\
			verify.o		view1.o			view2.o			viewprefs.o	\
			xfer1.o			xfer2.o			xfer3.o			xfer4.o		\
			xfermon.o		xftp.o

xftp: ${OBJECTS}
	${CC} ${CFLAGS} -o xftp ${OBJECTS} ${LIB_DIRS} ${LIBS} ${SOCKS_LIBS}

.c.o:
	${CC} ${CFLAGS} ${SOCKS_FLAGS} ${INCLUDE_DIRS} -c $<

sources: ${SOURCES} ${INCLUDES} XFtp

clean:
	rm -f ${OBJECTS} xftp core

install:
	-cp xftp $(BINDIR)
	-strip $(BINDIR)/xftp
	-cp XFtp.ad $(INSTALLED_APPDEFS)
	-chmod a+r $(INSTALLED_APPDEFS)

install.man:
	-cp xftp.man $(INSTALLED_MAN)
	-chmod a+r $(INSTALLED_MAN)

aconnect.o: list.h str.h xftp.h
arconnect.o: list.h str.h xftp.h
cache.o: list.h str.h xftp.h
changedir.o: list.h xftp.h
cmds.o: str.h xftp.h
ctrlconn.o: str.h xftp.h
dataconn.o: xftp.h
delete.o: list.h str.h xfer.h xftp.h
destdir.o: list.h xftp.h
dirlist.o: list.h xftp.h
dirlong.o: list.h str.h xftp.h
dirname.o: xftp.h
dirpath.o: xftp.h
dirtable.o: list.h xftp.h
flag.o: errorflag.h okflag.h pixmaps.h
get.o: str.h xfer.h xftp.h
goto.o: list.h xftp.h
host.o:	xftp.h
lconnect.o: list.h xftp.h
lcopy.o: xfer.h xftp.h
list.o: list.h
local.o: xftp.h
log.o: str.h xftp.h
ls.o: list.h str.h xftp.h
mainhelp.o: xftp.h
misc.o: logo50.h logo64.h xftp.h
mkdir.o: str.h xftp.h
move.o: list.h xfer.h xftp.h
password.o: rconnect.h str.h xftp.h
path.o: str.h xftp.h
pixmaps.o: pixmaps.h
prefs.o: prefs.h str.h xftp.h
prefshelp.o: prefs.h
pulldown.o: pulldown.h
put.o: xfer.h xftp.h
quote.o: list.h str.h xftp.h
rconnect.o: list.h str.h xftp.h
rcopy.o: str.h xfer.h xftp.h
remote.o: str.h xftp.h
rename.o: xftp.h
select.o: list.h str.h xftp.h
server.o: str.h xftp.h
startup.o: startup.h
toplev.o: pixmaps.h pulldown.h xftp.h
view1.o: str.h xfer.h xftp.h
view2.o: list.h str.h xftp.h
viewprefs.o: list.h str.h
xfer1.o: str.h xfer.h xftp.h
xfer2.o: str.h xfer.h xftp.h
xfer3.o: xfer.h xftp.h
xfer4.o: str.h xfer.h xftp.h
xfermon.o: xftp.h
xftp.o: xftp.h
