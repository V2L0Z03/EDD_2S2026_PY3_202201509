CXX = g++
CXXFLAGS = -std=c++17 -Wall -Iinclude -Iinclude/vendor -DASIO_STANDALONE

# Excluimos temporalmente a GraphvizReporter y cualquier otro que tenga dependencias de Qt.
# En la Fase 3, nos enfocaremos en las estructuras puras y el servidor web base.
SRC = src/structures/ListaDobleBeneficios.cpp \
      src/structures/ListaCircularPromos.cpp \
      src/structures/ListaCircularDobleSolicitudes.cpp \
      src/structures/ListaReservas.cpp \
      src/structures/EstructuraClientes.cpp \
      src/structures/ArbolB.cpp \
      src/structures/ArbolBST.cpp \
      src/structures/ArbolAVL.cpp \
      src/structures/MatrizDispersa.cpp \
      src/services/JSONReader.cpp \
      src/main.cpp

OBJ = $(SRC:src/%.cpp=build/%.o)
TARGET = bin/servidor.exe

# Dependencias para Sockets en Windows (usado por Crow y ASIO)
LIBS = -lws2_32 -lmswsock

all: create_dirs $(TARGET)

create_dirs:
	if not exist "bin" mkdir "bin"
	if not exist "build\structures" mkdir "build\structures"
	if not exist "build\models" mkdir "build\models"
	if not exist "build\services" mkdir "build\services"

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

build/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	if exist build rmdir /s /q build
	if exist bin rmdir /s /q bin

.PHONY: all clean create_dirs
