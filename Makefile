EXEC=remoterx
GCC=gcc
GCCFLAGS=-I /opt/radio/src/edgpio
INC=remoterx.h
LD=gcc
LDFLAGS=-L../edgpio
LIBS=-ledgpio
OBJS=main.o millis.o net.o commands.o

$(EXEC): $(OBJS) ../edgpio/libedgpio.a
	$(LD) $(OBJS) $(LDFLAGS) $(LIBS) -o $(EXEC)

%.o: %.c $(INC)
	$(GCC) $(GCCFLAGS) -c $< -o $@

clean:
	rm -f $(EXEC)
	rm -f $(OBJS)

install:
	supervisorctl stop remoterx
	cp remoterx /opt/radio/sbin
	supervisorctl start remoterx
