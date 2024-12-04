all: dsa

clean:
	rm -f dsa

rebuild: clean all

dsa: dsa.c
	g++ -g dsa.c -I /homes/jmcmicha/dml_lib/include/dml -o dsa -lm -lnuma -L~/dml_lib/lib -l:libdml.a

test: dsa
	LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/lib64 ./dsa
