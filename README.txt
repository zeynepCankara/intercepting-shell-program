Name: Zeynep Cankara
Id: 21703381

The directory content:
    - Makefile : compiles and links the files
    - producer.c : producer part of the simple producer-consumer program
    - consumer.c : consumer part of the simple producer-consumer program
    - isp.c : contains the intercepting shell program
    - report.pdf : contains the experiment results and implementation details
 includes report.pdf, bilshell.c, Makefile, infile.txt, producer.c and consumer.c.


Shell execution:
	- From the directory
	- $ make
	- $ ./isp <N> <mode 1: normal, 2: tapped>

Running the producer-consumer model from the intercepting shell:
    - isp$ ./producer <M> | ./consumer <M>