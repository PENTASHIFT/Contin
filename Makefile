contin : contin.c
	cc contin.c -o contin 
	mv contin /usr/bin/contin 
	mv contin.sh /etc/init.d/contin 
	mv contin.log /var/log/contin.log 
	mv contin.conf /etc/contin.conf 
	service contin start
clean :
	rm /var/log/contin.log /etc/contin.conf /usr/bin/contin /etc/init.d/contin

