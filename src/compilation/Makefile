CC = g++
CFLAGS = -std=c++11 -O0

default: compile

compile: compile.o unit1.o unit2.o multi_tape_turing_machine.o transition.o tape.o
	$(CC) $(CFLAGS) compile.o unit1.o unit2.o multi_tape_turing_machine.o transition.o tape.o

compile.o: compile.cpp
	$(CC) $(CFLAGS) -c compile.cpp

unit1.o: unit1.cpp unit1.h ../tm_definition/multi_tape_turing_machine.h ../tm_definition/transition.h
	$(CC) $(CFLAGS) -c unit1.cpp

unit2.o: unit2.cpp unit2.h
	$(CC) $(CFLAGS) -c unit2.cpp

multi_tape_turing_machine.o: ../tm_definition/multi_tape_turing_machine.cpp ../tm_definition/multi_tape_turing_machine.h ../tm_definition/turing_machine.h ../tm_definition/transition.h ../tm_definition/tape.h
	$(CC) $(CFLAGS) -c ../tm_definition/multi_tape_turing_machine.cpp

transition.o: ../tm_definition/transition.cpp ../tm_definition/transition.h
	$(CC) $(CFLAGS) -c ../tm_definition/transition.cpp

tape.o: ../tm_definition/tape.cpp ../tm_definition/tape.h
	$(CC) $(CFLAGS) -c ../tm_definition/tape.cpp

# for debugging purposes
unit2: unit2.cpp unit2.h utils.cpp utils.h
	$(CC) $(CFLAGS) unit2.cpp utils.cpp

# also for debugging
assemblySimulator: assemblyRunner.cpp utils.cpp utils.h
	$(CC) $(CFLAGS) assemblyRunner.cpp utils.cpp

clean:
	$(RM) a.out *.o
