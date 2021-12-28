CXX = clang++
CXX_FLAGS = -g
LNK_FLAGS = 
INCLUDE = -Iexternal
LIBS = -lpthread
TARGET = xilo
MAIN = src/xilo.cpp

${TARGET}: ${MAIN}
	${CXX} ${CXX_FLAGS} -o $@ $^ ${CXX_FLAGS} ${LNK_FLAGS} ${INCLUDE} ${LIBS}
