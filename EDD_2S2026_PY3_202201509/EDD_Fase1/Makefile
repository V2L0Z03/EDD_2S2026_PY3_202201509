#
#CXX = g++
#CXXFLAGS = -std=c++17 -Wall -Iinclude

#SRC = src/structures/ListaDobleBeneficios.cpp \
      src/structures/ListaCircularPromos.cpp \
      src/structures/ListaCircularDobleSolicitudes.cpp \
      src/structures/ArbolBST.cpp \
      src/structures/MatrizDispersa.cpp \
      src/services/CSVReader.cpp \
      src/services/GraphvizReporter.cpp \
      src/main.cpp

#OBJ = $(SRC:src/%.cpp=build/%.o)
#TARGET = bin/programa.exe

#all: create_dirs $(TARGET)

#create_dirs:
#	@mkdir -p bin build/structures build/models build/services build/ui reports_output

#$(TARGET): $(OBJ)
#	$(CXX) $(CXXFLAGS) -o $@ $^

#build/%.o: src/%.cpp
#	$(CXX) $(CXXFLAGS) -c $< -o $@

#clean:
#	rm -rf build bin reports_output/*.dot reports_output/*.png

#.PHONY: all clean create_dirs

#---------------------------
CXX = g++

# --- CONFIGURACIÓN DE QT ---
# Reemplaza esta ruta con la carpeta donde instalaste Qt (por ejemplo, en MSYS2 o el instalador oficial)
QT_DIR = C:/Qt/6.11.1/mingw_64

# Rutas de inclusión de las cabeceras de Qt
QT_INCLUDES = -I$(QT_DIR)/include \
              -I$(QT_DIR)/include/QtWidgets \
              -I$(QT_DIR)/include/QtGui \
              -I$(QT_DIR)/include/QtCore

# Librerías de Qt para el enlazador (Linker)
QT_LIBS = -L$(QT_DIR)/lib -lQt6Widgets -lQt6Gui -lQt6Core

# Ruta al ejecutable MOC de Qt
MOC = $(QT_DIR)/bin/moc

# --- BANDERAS DEL COMPILADOR ---
CXXFLAGS = -std=c++17 -Wall -Iinclude -Iinclude/vendor $(QT_INCLUDES)

# --- ARCHIVOS FUENTE Y OBJETOS ---
SRC = src/structures/ListaDobleBeneficios.cpp \
      src/structures/ListaCircularPromos.cpp \
      src/structures/ListaCircularDobleSolicitudes.cpp \
      src/structures/ListaReservas.cpp \
      src/structures/EstructuraClientes.cpp \
      src/structures/ArbolB.cpp \
      src/structures/ArbolBST.cpp \
      src/structures/ArbolAVL.cpp \
      src/structures/MatrizDispersa.cpp \
      src/services/CSVReader.cpp \
      src/services/JSONReader.cpp \
      src/services/GraphvizReporter.cpp \
      src/ui/LoginWindow.cpp \
      src/ui/MainWindow.cpp \
      src/main.cpp

# Cabeceras que utilizan Q_OBJECT y requieren pasar por MOC
MOC_HEADERS = include/ui/LoginWindow.hpp include/ui/MainWindow.hpp

# Archivos C++ intermediarios generados por MOC
MOC_SRCS = build/ui/moc_LoginWindow.cpp build/ui/moc_MainWindow.cpp

# Objetos resultantes del MOC
MOC_OBJS = build/ui/moc_LoginWindow.o build/ui/moc_MainWindow.o

# Objetos totales a compilar
OBJ = $(SRC:src/%.cpp=build/%.o) $(MOC_OBJS)
TARGET = bin/programa.exe

# --- REGLAS ---
all: create_dirs $(MOC_SRCS) $(TARGET)

create_dirs:
	if not exist "bin" mkdir "bin"
	if not exist "build\structures" mkdir "build\structures"
	if not exist "build\models" mkdir "build\models"
	if not exist "build\services" mkdir "build\services"
	if not exist "build\ui" mkdir "build\ui"
	if not exist "reports_output" mkdir "reports_output"
	if not exist "Asientos" mkdir "Asientos"

# Regla de enlace final agregando las librerías de Qt
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(QT_LIBS)

# Regla general para compilar archivos .cpp normales a .o
build/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Regla para compilar el archivo generado por MOC
build/ui/moc_%.o: build/ui/moc_%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Regla para transformar las cabeceras .hpp con Q_OBJECT a código C++ válido mediante MOC
build/ui/moc_%.cpp: include/ui/%.hpp
	$(MOC) $(QT_INCLUDES) $< -o $@

clean:
	if exist build rmdir /s /q build
	if exist bin rmdir /s /q bin
	if exist reports_output del /q reports_output\*.dot
	if exist reports_output del /q reports_output\*.png

.PHONY: all clean create_dirs