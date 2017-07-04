CXX = g++
CXXFLAGS = -std=c++11 -I../include
LDFLAGS = -lmpfr -lgmp

EXEC = ExampleFAD2 ExampleTAD1 \
	ExampleTAD2

all: $(EXEC)
$(EXEC): % : %.o
	$(CXX) -o $@ $? $(LDFLAGS)
	
clean:
	rm -rf *.o *.txt $(EXEC)
	
result:
	rm ../output/*.txt
	@-echo ""
	@-echo "Generating output text to ../output"
	@-echo ""
	@-for i in $(EXEC); do \
		./$$i >> ../output/$${i}_result.txt; \
	done
