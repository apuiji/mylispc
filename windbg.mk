include Makefile

DEST = windbg

${DEST}/mylispc.exe: $(addprefix ${DEST}/, ${OBJS})
	echo 123

${DEST}/%.o: %.c ${HEADS}
	gcc $< -c -g -I . -O2 -o $@

clean:
	echo>${DEST}/mylispc.exe
	echo>${DEST}/a.o
	del ${DEST}\mylispc.exe
	del ${DEST}\*.o

.PHONY: clean
