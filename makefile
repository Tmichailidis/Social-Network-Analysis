CC 			=	gcc
CFLAGS 		=	-Wall -Wextra -O3
SOURCE 		=	$(wildcard *.c)
EXEC 		= 	sn_analysis
LDFLAGS		= 	-lpthread

$(EXEC): $(SOURCE)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(EXEC)
