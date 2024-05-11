include Makefile

mylispc: $(addprefix linuxdbg/, ${OBJS})
	echo 123

linuxdbg/%.o: %.c ${HEADS}
	gcc $< -c -g -I . -O2 -o $@

clean:
	touch linuxdbg/a.o
	rm linuxdbg/*.o

.PHONY: clean
