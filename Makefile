
all: gen_signal

gen_signal: gen_signal.o
	g++ -o gen_signal gen_signal.o
