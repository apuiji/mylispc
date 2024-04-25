include Makefile

windbg/libmylispc.dll.a: $(addprefix windbg/, ${OBJS})
	ar -rsv $@ $^

windbg/%.o: %.cc ${HHS}
	clang++ $< -c -g -O2 -o $@ -std=c++2b -stdlib=libc++

clean:
	echo>windbg\a.o
	echo>windbg\libmylispc.dll.a
	del windbg\*.o
	del windbg\libmylispc.dll.a

.PHONY: clean
