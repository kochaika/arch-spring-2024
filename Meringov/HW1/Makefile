EMULATOR_SRC = Emulator
COMPILER_SRC = Compiler


all: emulator build_compiler test

emulator:


compiler: build_compiler alias_compiler

build_compiler: .FORCE
	(cd $(COMPILER_SRC) && ./gradlew installDist)
alias_compiler: build_compiler
	ln -f -s $(COMPILER_SRC)/build/install/Compiler/bin/Compiler Compiler.sh

.FORCE:

unit_test:

end_to_end_test:

test: unit_test end_to_end_test

clean_emulator:
	(cd $(EMULATOR_SRC) && $(MAKE) clean)

clean_compiler:
	(cd $(COMPILER_SRC) && ./gradlew clean)

clean: clean_emulator clean_compiler
	rm Compiler.sh 2> /dev/null; true