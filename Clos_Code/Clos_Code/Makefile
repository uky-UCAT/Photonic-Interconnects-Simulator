# makefile
# architecture
TARGET_ARCH = linux64

SYSTEMC = /usr/local/systemc-2.3.0a
INCDIR = -I. -I.. -I$(SYSTEMC)/include
LIBDIR = -L. -L.. -L$(SYSTEMC)/lib-$(TARGET_ARCH) -lsystemc-2.3.0 -Wl,-rpath,$(SYSTEMC)/lib-$(TARGET_ARCH)

LIBS   =  -lsystemc -lm -ldl




# compiler
CC     = g++ -std=c++98

# debug flag
DEBUG  = -g -g2
OTHER  = -Wall
#LDFLAG = -rdynamic

#CFLAGS = $(DEBUG) $(OTHER) $(LDFLAG)
CFLAGS = $(DEBUG) #$(LDFLAG)

MODULE = PNC64Cycles

SRCS = main.cpp buffer.cpp globalstatus.cpp noc.cpp power.cpp processingelement.cpp hub.cpp gatewayinterface.cpp status.cpp reservationtable.cpp benchmark.cpp

OBJS = $(SRCS:.cpp=.o)

EXE    = $(MODULE)

.SUFFIXES: .cc .cpp .o .x .so

$(EXE): $(OBJS) $(SYSTEMC)/lib-$(TARGET_ARCH)/libsystemc.a 
	$(CC) $(CFLAGS) $(INCDIR) $(LIBDIR) -o $@ $(OBJS) $(LIBS) 2>&1 | c++filt

.cpp.o:
	$(CC) $(CFLAGS) $(INCDIR) -o $@ -c $<

.cc.o:
	$(CC) $(CFLAGS) $(INCDIR) -o $@ -c $<

clean:
	rm -f $(OBJS) $(MODULE)
	#rm -f `find -name "*~"`


# DO NOT DELETE
