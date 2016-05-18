
DEBUG_OP = -D_DEBUG 
CPP_DEBUG_OP = -g3 -D_DEBUG 

OPTIMIZATION_OP = 
CPP_OPTIMIZATION_OP = -O0 

COMPILE_FLAGS = $(DEBUG_OP) $(OPTIMIZATION_OP) 

CPP_COMPILE_FLAGS = $(CPP_DEBUG_OP) $(CPP_OPTIMIZATION_OP) -std=c++11 -Wall -c -fmessage-length=0 -Wno-unused-function -Wno-extern-c-compat -fPIC 

LINK_FLAGS = -shared -Wl,--no-undefined 

AR_FLAGS = 

EDC_COMPILE_FLAGS = 