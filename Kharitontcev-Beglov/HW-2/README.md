# Simulavr in Docker
Using this example you can work with [simulavr](https://www.nongnu.org/simulavr/) inside container. Not need to install anything on your host machine except Docker  

## How to install Docker
Use official manual: https://docs.docker.com/engine/install/

## How to use
Run container, which contains simulavr and necessary environment:  
`docker compose -f docker-compose.yml -p simulavr up -d`

Login into container:  
`docker compose exec simulavr /bin/bash`

After you finished your work with container use this command to shut down it:  
`docker compose -f docker-compose.yml -p simulavr down`  

## Example
Folder [avr-project](./avr-project) is mapped by default into container, and you will be in this folder after login into container.
You can use it as a shader work directory between container and your host machine.  
You can use Makefile in this directory to make a "hello world" test (run commands in `/avr-project` folder **inside** of container):
```shell
make
make run
```

It will build simple.c and run it in the simulavr. Read more about this example [here](https://www.nongnu.org/simulavr/simple_ex.html).