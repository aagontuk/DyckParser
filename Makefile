all: dyckparser

dyckparser:
	g++ -Wall -g dyckparser.cpp -o dyckparser

check:
	echo "[[][[][][[]]][][]]" | ./dyckparser 

clean:
	rm dyckparser
