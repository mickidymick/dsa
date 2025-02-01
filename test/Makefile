all: dsa

clean:
	rm -f dsa test_dsa

rebuild: clean all

dsa: dsa.c test_dsa.c
	gcc -g dsa.c -Iinclude -o dsa -Linclude -lm -lnuma -ldml -lstdc++
	gcc -g test_dsa.c -Iinclude -o test_dsa -Linclude -lm -lnuma -ldml -lstdc++
	gcc -g async_dsa.c -Iinclude -o async_dsa -Linclude -lm -lnuma -ldml -lstdc++
	gcc -g small_async_dsa.c -Iinclude -o small_async_dsa -Linclude -lm -lnuma -ldml -lstdc++

test: small_async_dsa
	LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/lib64 taskset --cpu-list 1 ./small_async_dsa

# test: async_dsa
# 	LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/lib64 ./async_dsa 64
# 	LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/lib64 ./async_dsa 2048
# 	LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/lib64 ./async_dsa 4096
# 	LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/lib64 ./async_dsa 8192
# 	LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/lib64 ./async_dsa 16384
