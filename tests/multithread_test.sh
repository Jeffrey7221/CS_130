#!/bin/bash


# initializing server with port number
PORT_NUM=9000
echo "
server {
  port 9000;
  location \"/static\" StaticHandler {
    root /tests/static_data;
  }
  location \"/echo\" EchoHandler {
  }
}
" > "integration_config.txt"

../build/bin/server ./integration_config.txt &
pid_server=$$



# erase nondeterministic behavior
sleep 1


#run 3 commands in parallel

seq 1 3 | xargs -n1 -P3  curl "http://localhost:9000/static/example_data.txt" > multi_thread_test.txt

kill -9 $(( $pid_server + 1 ))


if [ -n file.txt ]
then
	rm ./integration_config.txt ./multi_thread_test.txt #txt file should just display 3 example data
	echo "multi_thread_test passed"
	exit 0
else
	rm ./integration_config.txt ./multi_thread_test.txt
	echo "multi_thread_test failed"
	exit 1;
fi