# make the soinstala program
#

SDIR=src
IDIR=include
CFLAGS=-I$(IDIR) 

ODIR=obj
EDIR=bin

LIBS=-lrt -lpthread

_DEPS = main.h memoria.h prodcons.h controlo.h ficheiro.h tempo.h escalonador.h rececionista.h instalador.h cliente.h so.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o memoria.o prodcons.o controlo.o ficheiro.o tempo.o escalonador.o rececionista.o instalador.o cliente.o so.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	gcc -c -o $@ $< $(CFLAGS)

$(EDIR)/soinstala: $(OBJ)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)


