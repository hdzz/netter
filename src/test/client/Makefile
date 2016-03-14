#基本
CC = g++

ver=release
CFLAGS=-Wall -g -std=c++11
ifeq ($(ver), release)
CFLAGS := $(CFLAGS) -O2 
endif

LDFLAGS= -lpthread -lnetter
AR=ar
CP=/bin/cp
RM=/bin/rm -rf

# 二进制目标
BIN=client

#源文件目录
SrcDir= .
#头文件目录
IncDir= . ../../netter
#连接库目录
LibDir= ../../netter

SRCS=$(foreach dir,$(SrcDir),$(wildcard $(dir)/*.cpp))
INCS=$(foreach dir,$(IncDir),$(addprefix -I,$(dir)))
LINKS=$(foreach dir,$(LibDir),$(addprefix -L,$(dir)))
CFLAGS := $(CFLAGS) $(INCS)
LDFLAGS:= $(LINKS) $(LDFLAGS)

OBJS = $(SRCS:%.cpp=%.o)
.PHONY:all clean

all:$(BIN)
$(BIN):$(OBJS)
	$(CC) -o $(BIN) $(OBJS) $(LDFLAGS)
	@echo " OK!\tCompile $@ "

%.o:%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	@echo "\tCleaning files..."
	@$(RM) $(OBJS) $(BIN)
