compile:
	gcc control.c -o control
	gcc client.c -o client
clean:
	rm control
	rm *.out
	rm story.txt
