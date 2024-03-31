# Solution

Main logic happens during the interrupt.
The idea is to swap stacks on each interrupt.
Program counter is automatically saved/restored on stack.
Therefore, if we want to save other registers, we can just push them to stack.
However, for now, there is nothing special saved on the stack.

`save_stack` and `restore_stack` work with stack, however, a constant (14) is currently hardcoded.

I also had to change the `low_task` function code, because I want to get first interrupt in it to get the stack saved.

# Tests

Run `make test` after running `make` (so, build should be available).
