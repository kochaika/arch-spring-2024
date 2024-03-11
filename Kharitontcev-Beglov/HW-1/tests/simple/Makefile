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
	@$(call compile,"print_works")
	@$(call run,"print_works")
	@$(call compile,"const_works")
	@$(call run,"const_works")
	@$(call compile,"simple_expr")
	@$(call run,"simple_expr")
	@$(call compile,"simple_sub")
	@$(call run,"simple_sub")
	@$(call compile,"simple_variable")
	@$(call run,"simple_variable")
	@$(call compile,"unary_not")
	@$(call run,"unary_not")
	@$(call compile,"complex_expr")
	@$(call run,"complex_expr")
	@$(call compile,"complex_variables")
	@$(call run,"complex_variables")
	@$(call compile,"ifs")
	@$(call run,"ifs")
	@$(call compile,"complex_ifs")
	@$(call run,"complex_ifs")
	@$(call compile,"while")
	@$(call run,"while")
clean:
	rm -rf *.code *.mem *.out
