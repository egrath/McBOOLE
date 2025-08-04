#ifdef 
CC			= cc
CFLAGS		= -std=c89 -c -Werror
LDFLAGS		= ""

SRCDIR		= ../src
OBJS		= alloc.o buildgra.o check.o detect.o incubeso.o init.o inputcub.o lists.o mcboole.o messages.o outcubes.o outputcu.o param.o place.o prime.o scan.o select.o setvar.o solve.o time.o
EXE			= mcboole.exe

$(EXE):		$(OBJS)
			$(CC) $^ -o $@

$(OBJS):	%.o: $(SRCDIR)/%.c
			cc $(CFLAGS) $< -o $@

clean:
			rm $(OBJS) $(EXE)

