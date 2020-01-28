# Specify this vars to compile program.

# Additional Include Directories
BOOST_INCPATH =$(BOOST_ROOT)
OPENSSL_INCPATH =C:\SDK\openssl\openssl-1.0.2u\openssl-1.0.2u\inc32

# Additional Library Directories
BOOST_LIBPATH	=$(BOOST_ROOT)\stage\lib
OPENSSL_LIBPATH =C:\SDK\openssl\openssl-1.0.2u\openssl-1.0.2u\out32

# Compilation commands
all : tyfirc.lib

### tyfirc ###
TYFIRC_HEADERS =tyfirc\tyfirc-msgpack.h tyfirc\tyfirc-misc.h tyfirc\tyfirc-scmessage.h
tyfirc.lib : tyfirc-misc.obj tyfirc-msgpack.obj tyfirc-scmessage.obj
	lib tyfirc-misc.obj tyfirc-msgpack.obj tyfirc-scmessage.obj /OUT:release\tyfirc.lib
	
tyfirc-msgpack.obj : tyfirc\tyfirc-msgpack.h tyfirc\tyfirc-msgpack.cpp
	cl /c /EHsc tyfirc\tyfirc-msgpack.cpp

tyfirc-misc.obj : tyfirc\tyfirc-misc.h tyfirc\tyfirc-misc.cpp
	cl /c /EHsc tyfirc\tyfirc-misc.cpp

tyfirc-scmessage.obj : tyfirc\tyfirc-scmessage.h tyfirc\tyfirc-scmessage.cpp
	cl /c /EHsc tyfirc\tyfirc-scmessage.cpp
###

### tyfirc-client ###
all : tyfirc-client.exe

tyfirc-client.exe: $(TYFIRC_HEADERS) tyfirc-ircclientapp.obj tyfirc-chatrw.obj tyfirc-client\main.cpp
	cl tyfirc-ircclientapp.obj tyfirc-chatrw.obj /EHsc /I$(BOOST_INCPATH) /I$(OPENSSL_INCPATH) /Ityfirc tyfirc-client\main.cpp\
	/link /DYNAMICBASE "tyfirc.lib" "ssleay32.lib" "libeay32.lib" "kernel32.lib" "user32.lib" "gdi32.lib" "winspool.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" "odbc32.lib" "odbccp32.lib"\
	/LIBPATH:release /LIBPATH:$(BOOST_LIBPATH) /LIBPATH:$(OPENSSL_LIBPATH) /OUT:release\tyfirc-client.exe

tyfirc-ircclientapp.obj : $(TYFIRC_HEADERS) tyfirc-client\tyfirc-irclientapp.h tyfirc-client\tyfirc-ircclientapp.cpp tyfirc-client\tyfirc-chatrw.h
	cl /c /EHsc /I$(BOOST_INCPATH) /I$(OPENSSL_INCPATH) /Ityfirc tyfirc-client\tyfirc-ircclientapp.cpp

tyfirc-chatrw.obj : $(TYFIRC_HEADERS) tyfirc-client\tyfirc-chatrw.h tyfirc-client\tyfirc-chatrw.cpp
	cl /c /EHsc /I$(BOOST_INCPATH) /I$(OPENSSL_INCPATH) /Ityfirc tyfirc-client\tyfirc-chatrw.cpp
###

### tyfirc-server ###
all : tyfirc-server.exe

tyfirc-server.exe : tyfirc-authmanager.obj tyfirc-ircserverapp.obj tyfirc-sessionlist.obj tyfirc-session.obj
	cl tyfirc-authmanager.obj tyfirc-ircserverapp.obj tyfirc-sessionlist.obj tyfirc-session.obj /EHsc /I$(BOOST_INCPATH) /I$(OPENSSL_INCPATH) /Ityfirc tyfirc-server\main.cpp\
	/link /DYNAMICBASE "tyfirc.lib" "ssleay32.lib" "libeay32.lib" "kernel32.lib" "user32.lib" "gdi32.lib" "winspool.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" "odbc32.lib" "odbccp32.lib"\
	/LIBPATH:release /LIBPATH:$(BOOST_LIBPATH) /LIBPATH:$(OPENSSL_LIBPATH) /OUT:release\tyfirc-server.exe

tyfirc-authmanager.obj : tyfirc-server\tyfirc-authmanager.h tyfirc-server\tyfirc-authmanager.cpp
	cl /c /EHsc /I$(BOOST_INCPATH) /I$(OPENSSL_INCPATH) /Ityfirc tyfirc-server\tyfirc-authmanager.cpp

tyfirc-ircserverapp.obj : $(TYFIRC_HEADERS) tyfirc-server\tyfirc-ircserverapp.h tyfirc-server\tyfirc-ircserverapp.cpp tyfirc-server\tyfirc-authmanager.h tyfirc-server\tyfirc-session.h tyfirc-server\tyfirc-sessionlist.h
	cl /c /EHsc /I$(BOOST_INCPATH) /I$(OPENSSL_INCPATH) /Ityfirc tyfirc-server\tyfirc-ircserverapp.cpp

tyfirc-sessionlist.obj : $(TYFIRC_HEADERS) tyfirc-server\tyfirc-sessionlist.h tyfirc-server\tyfirc-sessionlist.cpp tyfirc-server\tyfirc-session.h
	cl /c /EHsc /I$(BOOST_INCPATH) /I$(OPENSSL_INCPATH) /Ityfirc tyfirc-server\tyfirc-sessionlist.cpp
	
tyfirc-session.obj : $(TYFIRC_HEADERS) tyfirc-server\tyfirc-session.h tyfirc-server\tyfirc-session.cpp
	cl /c /EHsc /I$(BOOST_INCPATH) /I$(OPENSSL_INCPATH) /Ityfirc tyfirc-server\tyfirc-session.cpp
###

clean :
	del tyfirc-misc.obj tyfirc-msgpack.obj tyfirc-scmessage.obj tyfirc-ircclientapp.obj tyfirc-chatrw.obj tyfirc-authmanager.obj tyfirc-ircserverapp.obj tyfirc-sessionlist.obj tyfirc-session.obj release\tyfirc.lib release\tyfirc-client.exe release\tyfirc-server.exe