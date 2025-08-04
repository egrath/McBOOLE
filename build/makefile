SRCDIR		= ../src
CFLAGS		= -std=c89 -c -Werror
LDFLAGS		= ""

mcbool:		alloc.o buildgra.o check.o detect.o incubeso.o init.o inputcub.o lists.o mcboole.o messages.o outcubes.o outputcu.o param.o place.o prime.o scan.o select.o setvar.o solve.o time.o
			$(CC) $? -o $@

alloc.o:	$(SRCDIR)/alloc.c
			$(CC) $(CFLAGS) $(SRCDIR)/alloc.c

buildgra.o:	$(SRCDIR)/buildgra.c
			$(CC) $(CFLAGS) $(SRCDIR)/buildgra.c

check.o:	$(SRCDIR)/check.c
			$(CC) $(CFLAGS) $(SRCDIR)/check.c

detect.o:	$(SRCDIR)/detect.c
			$(CC) $(CFLAGS) $(SRCDIR)/detect.c

incubeso.o:	$(SRCDIR)/incubeso.c
			$(CC) $(CFLAGS) $(SRCDIR)/incubeso.c

init.o:		$(SRCDIR)/init.c
			$(CC) $(CFLAGS) $(SRCDIR)/init.c

inputcub.o:	$(SRCDIR)/inputcub.c
			$(CC) $(CFLAGS) $(SRCDIR)/inputcub.c

lists.o:	$(SRCDIR)/lists.c
			$(CC) $(CFLAGS) $(SRCDIR)/lists.c

mcboole.o:	$(SRCDIR)/mcboole.c
			$(CC) $(CFLAGS) $(SRCDIR)/mcboole.c

messages.o:	$(SRCDIR)/messages.c
			$(CC) $(CFLAGS) $(SRCDIR)/messages.c

outcubes.o:	$(SRCDIR)/outcubes.c
			$(CC) $(CFLAGS) $(SRCDIR)/outcubes.c

outputcu.o:	$(SRCDIR)/outputcu.c
			$(CC) $(CFLAGS) $(SRCDIR)/outputcu.c

param.o:	$(SRCDIR)/param.c
			$(CC) $(CFLAGS) $(SRCDIR)/param.c

place.o:	$(SRCDIR)/place.c
			$(CC) $(CFLAGS) $(SRCDIR)/place.c

prime.o:	$(SRCDIR)/prime.c
			$(CC) $(CFLAGS) $(SRCDIR)/prime.c

scan.o:		$(SRCDIR)/scan.c
			$(CC) $(CFLAGS) $(SRCDIR)/scan.c

select.o:	$(SRCDIR)/select.c
			$(CC) $(CFLAGS) $(SRCDIR)/select.c

setvar.o:	$(SRCDIR)/setvar.c
			$(CC) $(CFLAGS) $(SRCDIR)/setvar.c

solve.o:	$(SRCDIR)/solve.c
			$(CC) $(CFLAGS) $(SRCDIR)/solve.c

time.o:		$(SRCDIR)/time.c
			$(CC) $(CFLAGS) $(SRCDIR)/time.c

clean:
			rm *.o mcbool
