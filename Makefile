##### Make sure all is the first target.
all:

CXX = g++
CC  = gcc

CFLAGS  += -g -pthread -Wall 
CFLAGS  += -rdynamic -funwind-tables
 
CFLAGS  += -I.    
CFLAGS  += -I/usr/local/include 

CFLAGS += -D__unused="__attribute__((__unused__))"

#LDFLAGS += -L./usr/lib/gpac
LDFLAGS +=  -ldl
LDFLAGS +=  -L/usr/local/lib/

LDFLAGS +=  -ldl -lm  -lstdc++
  
DFLAGS += -lrt -lpthread -pthread -lm -ldl 
LDFLAGS += -lpthread -lrt -ldl -lm



C_SRC=
C_SRC +=online_service.c
C_SRC +=cJSON.c
C_SRC +=md5.c
CXX_SRC=

OBJ=
DEP=


#LDFLAGS+= -lcamera

OBJ_CAM_SRV = online_service.o
TARGETS    += online_service
$(TARGETS): $(OBJ_CAM_SRV)
TARGET_OBJ += $(OBJ_CAM_SRV)

FILE_LIST := files.txt
COUNT := ./make/count.sh
MK := $(word 1,$(MAKEFILE_LIST))
ME := $(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))

OBJ=$(CXX_SRC:.cpp=.o) $(C_SRC:.c=.o)
DEP=$(OBJ:.o=.d) $(TARGET_OBJ:.o=.d)

CXXFLAGS +=  -std=c++11 $(CFLAGS)
#include ./common.mk
.PHONY: all clean distclean

all: $(TARGETS)

clean:
	rm -f $(TARGETS) $(FILE_LIST)
	find . -name "*.o" -delete
	find . -name "*.d" -delete

distclean:
	rm -f $(TARGETS) $(FILE_LIST)
	find . -name "*.o" -delete
	find . -name "*.d" -delete

-include $(DEP)

%.o: %.c $(MK) $(ME)
	@[ -f $(COUNT) ] && $(COUNT) $(FILE_LIST) $^ || true
	@$(CC) -c $< -MM -MT $@ -MF $(@:.o=.d) $(CFLAGS) $(LIBQCAM_CFLAGS)
	$(CC) -c $< $(CFLAGS) -o $@ $(LIBQCAM_CFLAGS)

%.o: %.cpp $(MK) $(ME)
	@[ -f $(COUNT) ] && $(COUNT) $(FILE_LIST) $^ || true
	@$(CXX) -c $< -MM -MT $@ -MF $(@:.o=.d) $(CXXFLAGS)
	$(CXX) -c $< $(CXXFLAGS) -o $@

$(TARGETS): $(OBJ)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)
	@[ -f $(COUNT) -a -n "$(FILES)" ] && $(COUNT) $(FILE_LIST) $(FILES) || true
	@[ -f $(COUNT) ] && $(COUNT) $(FILE_LIST) || true
