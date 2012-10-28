# $Id: Makefile,v 1.3 2002/11/27 00:17:50 drey Exp $

OBJS = calendar.o io.o day.o ostern.o paskha.o
CFLAGS = -Wall -MD

calendar: $(OBJS)

clean:
	rm -rf $(OBJS) calendar

include $(wildcard *.d)
