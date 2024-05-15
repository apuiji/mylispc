include Makefile

DEST = linuxdbg

${DEST}/libmylispc.a: $(addprefix ${DEST}/, ${OBJS})
	ar -rsv $@ $^

${DEST}/%.o: %.cc ${HEADS}
	clang++ $< -c -g -O2 -o $@ -std=c++2b -stdlib=libc++

clean:
	touch ${DEST}/libmylispc.a ${DEST}/a.o
	rm ${DEST}/libmylispc.a ${DEST}/*.o

.PHONY: clean
