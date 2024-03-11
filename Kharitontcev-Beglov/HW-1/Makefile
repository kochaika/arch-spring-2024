all: build test clean
build:
	cargo build --release

test:
	cargo test
	$(MAKE) build
	$(MAKE) test -C tests

clean:
	cargo clean
	$(MAKE) clean -C tests
