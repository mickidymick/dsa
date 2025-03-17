all: dsa

clean:
	rm -f dsa_test

rebuild: clean all

dsa: dsa_test.c
	gcc -o3 dsa_test.c utils.c dsa.c -Iinclude -o dsa_test -Linclude -lm -lnuma -ldml -lstdc++
# 	gcc -g dsa_test.c utils.c dsa.c -Iinclude -o dsa_test -Linclude -lm -lnuma -ldml -lstdc++ -fno-omit-frame-pointer -mno-omit-leaf-frame-pointer

test: dsa_test
	@sudo sh -c "sync; echo 3 > /proc/sys/vm/drop_caches"
# 	@echo 3 | sudo tee -a /proc/sys/vm/drop_caches > /dev/null
# 	@LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/lib64 taskset --cpu-list 1 ./dsa_test 1
# 	@LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/lib64 taskset --cpu-list 1 ./dsa_test 8
# 	@LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/lib64 taskset --cpu-list 60 ./dsa_test 8
# 	@LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/lib64 taskset --cpu-list 1 ./dsa_test 32
	@LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/lib64 taskset --cpu-list 1 ./dsa_test 128
# 	@LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/lib64 taskset --cpu-list 60 ./dsa_test 128
# 	@LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/lib64 taskset --cpu-list 1 ./dsa_test 512
# 	@LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/lib64 taskset --cpu-list 60 ./dsa_test 512
# 	@LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/lib64 taskset --cpu-list 1 ./dsa_test 1024
# 	@LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/lib64 taskset --cpu-list 60 ./dsa_test 1024
# 	@LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/lib64 taskset --cpu-list 1 ./dsa_test 4096
# 	@LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/lib64 taskset --cpu-list 1 ./dsa_test 8192
# 	@LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/lib64 taskset --cpu-list 60 ./dsa_test 8192
# 	@LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/lib64 taskset --cpu-list 1 ./dsa_test 16384
# 	@LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/lib64 taskset --cpu-list 60 ./dsa_test 16384
# 	@LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/lib64 taskset --cpu-list 1 ./dsa_test 32768

gdb: dsa_test
	LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/lib64 taskset --cpu-list 0 gdb ./dsa_test
