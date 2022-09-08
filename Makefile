# --------------------
# Comandos
# --------------------
# o sinal de menos significa "se der erro, ignora e não para"
# ou seja, quando ele não conseguir deletar um arquivo (já
# deletado, ou não gerado), ele ignora.
RM 		:= -rm
CC 		:= g++
# --------------------
# Pastas
# --------------------
SRCDIR 	:= src
OBJDIR 	:= obj
BINDIR 	:= bin
# --------------------
# Arquivos
# --------------------
SRC 	:= $(wildcard $(SRCDIR)/*.cpp)
MAIN 	:= $(SRCDIR)/main.cpp
MAINO 	:= $(OBJDIR)/main.o
TARGET 	:= $(BINDIR)/main
# -lXXX vai procurar um arquivo com nome libXXX.a
OBJ 	:= $(SRC:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
# --------------------
# Flags para o compilador
# --------------------
# Sobre as flags utilizadas: I é para a diretiva #include encontrar arquivos em
# tal pasta. -Wall pede todos os avisos (Warning:all) e -g ajuda no debugger
# porque preserva o número da linha de código.
CXXFLAGS:= -Iinc -Wall -g
LNKFLAG :=  -static
	# -O2 -Os

# --------------------
# Regras de compilação
# --------------------

# all é o default. O comando 'make' sem argumentos cai aqui.
all: $(TARGET)

# Usamos o comando 'make clean' quando mudamos o tamanho de alguma coisa e
# queremos recompilar TUDO. Por padrão, ele só recompilaria arquivos .c que mudaram
clean:
	$(RM) $(OBJ) $(TSTOBJ)

# A 'receita' para arquivos obj (que são os .o) envolve os arquivos .c correspondentes
obj/%.o: src/%.cpp
	$(CC)  $(CFLAGS) -c $(@:$(OBJDIR)/%.o=$(SRCDIR)/%.cpp) -o $@

# O arquivo executável final depende de todos os arquivos .o (que não são de testes)
$(TARGET) : $(OBJ)
	$(CC) -o $(TARGET) $(OBJ) $(LIB)
