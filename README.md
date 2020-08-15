# COL216-Assignment-8

This assignment involved implementing a pipelined MIPS processor in C++ which supports stalling in case of hazards. In the next assignment we implement forwarding to handle hazards. The instruction set supported is a small subset of the actual ISA. 

## Run

Go into assignment directory and run the Makefile.
```
make
```
This will create a `pipeline` binary. The binary needs instruction file name and memory file name as command-line arguments.
```
./pipeline instruction/1.txt memory/1.txt
```

## Test

Tests are only supported for Assignment-8. Go into this directory and run tests.
```
make test
```
This will run Google Test if it is installed in system. Procedure to install can be found on their webpage. \
There are other instruction files which can be run to see stalling and forwarding.
