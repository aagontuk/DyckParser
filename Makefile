all: dyckparser paral_scan

dyckparser: dyckparser.cpp
	g++ -Wall -g dyckparser.cpp -o dyckparser

paral_scan: paral_scan.cpp
	g++ -Wall -g paral_scan.cpp -o paral_scan -lpthread

check:
	echo "[[][[][][[]]][][]]" | ./dyckparser 

clean:
	rm dyckparser paral_scan
