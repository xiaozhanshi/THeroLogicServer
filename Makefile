
#set the final compile target
TARGET = Logic_Server

PREFIX_BIN =

#set the c compile
CC=gcc
CFLAGS= 
#-g2 -Wall

#set the cpp compile
CXX=g++  -L/opt/lib/mysql-connector-c-6.0.2-linux-glibc2.3-x86-32bit/lib -L/usr/local/lib -I/usr/local/include/hiredis  -L/usr/local/lib/ -lcurl

CPPFLAGS =  -L/opt/lib/mysql-connector-c-6.0.2-linux-glibc2.3-x86-32bit/lib -L/usr/local/lib
#-g2 -Wall -O2
LIBS = -lmysqlclient -lhiredis -lssl
LINKFLAGS = -DIS_DEVELOPMENT_VERSION

#set the include dirs
INCLUDEDIRS = ./ ./GameLib/ ./Server/ ./mysql/ ./GameLib/tinyxml/ ./GameLib/MD5/ ./GameLib/jsoncpp/include/ ./Server/herosystem/RegistSystem/ ./Server/herosystem/PushSystem/ ./Server/herosystem/PushSystem/Helper/
INCLUDEDIRS += /opt/lib/mysql-connector-c-6.0.2-linux-glibc2.3-x86-32bit/include/
INCLUDES = $(foreach tmp, $(INCLUDEDIRS), -I $(tmp))

#set my source dirs
MYSOURCEDIRS = ./ ./GameLib/ ./Server/ ./mysql/ ./GameLib/tinyxml/ ./GameLib/MD5/ ./GameLib/jsoncpp/src/lib_json/ ./Server/herosystem/RegistSystem/ ./Server/herosystem/PushSystem/ ./Server/herosystem/PushSystem/Helper/

SOURCEDIRS = $(MYSOURCEDIRS)


C_SOURCES = $(foreach tmp, $(SOURCEDIRS), $(wildcard $(tmp)*.c))
C_OBJS = $(patsubst %.c, %.o, $(C_SOURCES))


CPP_SOURCES = $(foreach tmp, $(SOURCEDIRS), $(wildcard $(tmp)*.cpp))
CPP_OBJS = $(patsubst %.cpp, %.o, $(CPP_SOURCES))



.c.o:
	$(CC) -c -o $*.o $(CFLAGS) $(INCLUDES) $*.c
.cpp.o:
	$(CXX) -c -o $*.o $(CPPFLAGS) $(INCLUDES) $*.cpp

compile: $(CPP_OBJS) $(C_OBJS) $(OTHERS_C_OBJS) $(OTHERS_CPP_OBJS)
	$(CXX) $(LINKFLAGS) -o $(TARGET) $^ $(LIBS)

clean:
	rm -f $(CPP_OBJS) $(C_OBJS)
	rm -f $(TARGET)

install: $(TARGET)
	cp $(TARGET) $(PREFIX_BIN)

uninstall:
	rm -f $(PREFIX)/$(PREFIX_BIN)

rebuild: clean
