test: *.h *.c
	gcc -O3 *.c -o test -lm

clean:
	rm test.bin.*