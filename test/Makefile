.PHONY: all check clean

CC = gcc
CFLAGS = -std=gnu99 -Wall -g

OBJS = $(FILE_NAME).o
EXEC = $(FILE_NAME)
FILE_NAME = check

all:
	# generate the object file
	$(CC) $(CFLAGS) -c -o $(OBJS) $(FILE_NAME).c
	# generate the executable directly
	$(CC) $(OBJS) -o $(EXEC)

check:
	@./$(EXEC); \
	if [ $$? -eq 0 ] ; then \
		echo "PASS."; \
	else \
		echo "FAIL."; \
	fi

clean:
	rm -f $(OBJS) $(EXEC)