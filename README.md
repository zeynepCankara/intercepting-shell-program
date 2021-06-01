# Intercepting Shell Program

## The directory content:

- Makefile : compiles and links the files
- producer.c : producer part of the simple producer-consumer program
- consumer.c : consumer part of the simple producer-consumer program
- isp.c : contains the intercepting shell program
- report.pdf : contains the experiment results and implementation details
- project1.pdf: contains the project description

### Shell execution:
Find the directory project locates

- `$ make`
- `$ ./isp <N> <mode 1: normal, 2: tapped>`

### Running the producer-consumer model from the intercepting shell:

  - `isp$ ./producer <M> | ./consumer <M>`