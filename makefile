CXX = clang++
CXX_FLAGS = -g
LNK_FLAGS = 
INCLUDE = -Iexternal
LIBS = -lpthread
TARGET = xilo
CXX_FILES = $(wildcard src/*.cpp)
ENTRY = src/xilo.cpp

${TARGET}: ${CXX_FILES}
	${CXX} ${CXX_FLAGS} -o $@ ${ENTRY} ${LNK_FLAGS} ${INCLUDE} ${LIBS}
