all: isp producer consumer

isp: isp.c
	gcc -o isp isp.c

producer: producer.c
	gcc -o producer producer.c

consumer: consumer.c
	gcc -o consumer consumer.c

clean:
	rm fr isp isp.o *~
	rm fr producer producer.o *~
	rm fr consumer consumer.o *~
