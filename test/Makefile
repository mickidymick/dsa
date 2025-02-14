all: dsa

clean:
	rm -f dsa_test

rebuild: clean all

dsa: dsa_test.c
	gcc -g dsa_test.c utils.c dsa.c -Iinclude -o dsa_test -Linclude -lm -lnuma -ldml -lstdc++

test: dsa_test
	echo 3 | sudo tee -a /proc/sys/vm/drop_caches
# 	LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/lib64 taskset --cpu-list 1 ./dsa_test 256
# 	LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/lib64 taskset --cpu-list 1 ./dsa_test 4
	LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/lib64 taskset --cpu-list 1 ./dsa_test 4096
# 	LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/lib64 taskset --cpu-list 1 ./dsa_test 16384

gdb: dsa_test
	LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/lib64 taskset --cpu-list 1 gdb ./dsa_test
