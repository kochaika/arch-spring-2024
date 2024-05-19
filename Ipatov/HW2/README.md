# How to run

## Docker

Run container with all needed dependencies:

`docker compose -f docker-compose.yml -p simulavr up -d`

Then login into it:

`docker compose exec simulavr /bin/bash`

## Make targets

To simply build the elf file from `simple.c` use the `make build` target

After this, you can run the simulavr using this elf with target `make run`

To clean the artifacts produced run `make clean`

To run tests (one test, to be precise), use the `make test` task (it will clean and build the needed executable by itself)

## How it works

The context switch happens on interrupt, with saving the state of stack and registers in their corresponding functions. 
