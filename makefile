all: control.c write.c
	gcc -o control control.c
	gcc -o write write.c
