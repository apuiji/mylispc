include Makefile

windbg/libmylispc.a: $(addprefix windbg/, ${OBJS})
	ar -rsv $@ $^

windbg/%.o: %.cc ${HHS}
	clang++ $< -c -g -I . -O2 -o $@ -std=c++2b -stdlib=libc++

clean:
	echo>windbg/libmylispc.a
	del windbg/libmylispc.a windbg/*.o

.PHONY: clean
