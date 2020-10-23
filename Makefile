all: dyckparser paral_scan

dyckparser:
	g++ -Wall -g dyckparser.cpp -o dyckparser

paral_scan:
	g++ -Wall -g paral_scan.cpp -o paral_scan -lpthread

check:
	echo "[[][[][][[]]][][]]" | ./dyckparser 

clean:
	rm dyckparser paral_scan
