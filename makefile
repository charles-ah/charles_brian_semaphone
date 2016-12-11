compile:
	gcc control.c -o control
	gcc client.c -o client
clean:
	rm -f control client
	rm -f story.txt
