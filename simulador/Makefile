# Compilador
CXX = g++

# Flags de compilación
CXXFLAGS = -std=c++17 -Iinclude -Wall

# Directorios
SRCDIR = src
INCDIR = include
BUILDDIR = build
TARGET = bin/simulador

# Archivos fuente y objetos
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(patsubst $(SRCDIR)/%.cpp, $(BUILDDIR)/%.o, $(SOURCES))

# Regla principal
all: $(TARGET)

# Regla para crear el ejecutable
$(TARGET): $(OBJECTS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Regla para compilar los archivos objeto
$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Regla para limpiar los archivos generados
clean:
	rm -rf $(BUILDDIR) bin

# Para evitar que 'make' se confunda con archivos llamados 'all' o 'clean' :(
.PHONY: all clean
