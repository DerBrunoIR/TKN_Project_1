
while true; 
do 
	cat $2 | nc -C localhost $1;
	sleep 1s
done


