include $(MODULE_DIR)/Makefile.common.in

CFLAGS += -Wall -O2
LDFLAGS += -s 
LIBS = -lcrypt -lresolv -lcurl -lssl -lcrypto -lpthread -lz

NAME = settime
OBJS = settime.o 

all: $(NAME)
	@true

$(NAME): $(OBJS)
	$(QUIET_LD)$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

%.o: %.c
	$(QUIET_CC)$(CC) $(CFLAGS) -c -o $@ $<
	
install: all
	cp -f settime $(WD_USER_BINARY)/
	
clean:
	rm -f  *.o

.PHONY: all install clean
