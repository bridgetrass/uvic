default: rdp

rdp: rdps.c rdpr.c
	gcc -o rdps rdps.c -pthread
	gcc -o rdpr rdpr.c
clean:
	-rm -f rdps
	-rm -f rdpr
