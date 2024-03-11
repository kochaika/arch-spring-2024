define compile
	../../target/release/compiler -i $(1).klang -c $(1).code -m $(1).mem
	printf "\033[0;32m$(1) compiled✓\033[0m\n"
endef

define run
	../../target/release/mips_emulator -c $(1).code -m $(1).mem > $(1).out
	diff -q $(1).out $(1).ans
	printf "\033[0;32m$(1)✓\033[0m\n"
endef

test:
	@$(call compile,"fib")
	@$(call run,"fib")
	@$(call compile,"pow")
	@$(call run,"pow")

clean:
	rm -rf *.code *.mem *.out