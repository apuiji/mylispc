include Makefile

DEST = linuxdbg

${DEST}/mylispc: $(addprefix ${DEST}/, ${OBJS})
	echo 123

${DEST}/%.o: %.c ${HEADS}
	gcc $< -c -g -I . -O2 -o $@

clean:
	touch ${DEST}/mylispc ${DEST}/a.o
	rm ${DEST}/mylispc ${DEST}/*.o

.PHONY: clean
