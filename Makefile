all: dyckparser paral_scan paral_sum

dyckparser: dyckparser.cpp
	g++ -Wall -g dyckparser.cpp -o dyckparser

paral_scan: paral_scan.cpp
	g++ -Wall -g paral_scan.cpp -o paral_scan -lpthread

paral_sum: paral_sum.cpp
	g++ -Wall -g paral_sum.cpp -o paral_sum -lpthread

check:
	echo "[[][[][][[]]][][]]" | ./dyckparser 

clean:
	rm dyckparser paral_scan paral_sum
