#!/bin/bash



# initializing server with port number
PORT_NUM=9000
echo "
server {
  listen 9000;
  location /static/ {
    root tests/static_data;
  }
}" > "integration_config.txt"

./bin/server ./integration_config.txt &
pid_server=$$

# initializing expected header responses
echo "HTTP/1.0 200 OK" > okay_header.txt
echo "HTTP/1.0 400 Bad Request" > bad_header.txt



# erase nondeterministic behavior
sleep 1



# TEST 1 (200 OK) good HTTP POST request
echo "POST /test HTTP/1.1
Host: foo.example
Content-Type: application/x-www-form-urlencoded
Content-Length: 27

field1=value1&field2=value2" > "test_file.txt"

cat test_file.txt | nc localhost $PORT_NUM | tr -d '\r'> "response_file.txt"

# check for a 200 OK Header
head -n 1 "response_file.txt" > response_header.txt
diff response_header.txt okay_header.txt &>/dev/null
if [[ $? -eq 0 ]]; then
    echo "200 OK: Header Success (1)"; 
else 
    echo "200 OK: Header Failed (1)";
    rm ./test_file.txt ./response_file.txt ./response_header.txt ./okay_header.txt ./bad_header.txt ./integration_config.txt
    kill -9 $(( $pid_server + 1 ))
    exit 1;
fi

# check the body of the good response
sed -i '1, 4d' "response_file.txt"
diff response_file.txt test_file.txt &>/dev/null
if [[ $? -eq 0 ]]; then
    echo "200 OK: Body Success (1)"; 
else 
    echo "200 OK: Body Failed (1)"; 
    rm ./test_file.txt ./response_file.txt ./response_header.txt ./okay_header.txt ./bad_header.txt ./integration_config.txt
    kill -9 $(( $pid_server + 1 ))
    exit 1;
fi

# clean up test 1 files
rm ./test_file.txt ./response_file.txt ./response_header.txt



# TEST 2 (400 Bad Request) deleted a necessary linebreak
echo "POST /test HTTP/1.1
Host: foo.example
Content-Type: application/x-www-form-urlencoded
Content-Length: 27
field1=value1&field2=value2" > "test_file.txt"

cat test_file.txt | nc localhost $PORT_NUM | tr -d '\r'> "response_file.txt"

# Check for a 400 bad request Header
head -n 1 "response_file.txt" > response_header.txt
diff response_header.txt bad_header.txt &>/dev/null
if [[ $? -eq 0 ]]; then
    echo "400 Bad Request: Header Success (2)"; 
else 
    echo "400 Bad Request: Header Failed (2)";
    rm ./test_file.txt ./response_file.txt ./response_header.txt ./okay_header.txt ./bad_header.txt ./integration_config.txt
    kill -9 $(( $pid_server + 1 ))
    exit 1;
fi

# Check the body of the bad response
sed -i '1, 4d' "response_file.txt"
diff response_file.txt test_file.txt &>/dev/null
if [[ $? -eq 0 ]]; then
    echo "400 Bad Request: Body Success (2)"; 
else 
    echo "400 Bad Request: Body Failed (2)"; 
    rm ./test_file.txt ./response_file.txt ./response_header.txt ./okay_header.txt ./bad_header.txt ./integration_config.txt
    kill -9 $(( $pid_server + 1 ))
    exit 1;
fi

# clean up test 2 files
rm ./test_file.txt ./response_file.txt ./response_header.txt



# TEST 3 (200 OK) attempt a simple GET request
curl -isS "http://localhost:$PORT_NUM" -o "response_file.txt" 

# Check for a 200 OK Header
head -n 1 "response_file.txt" | tr -d '\r'> response_header.txt
diff response_header.txt okay_header.txt &>/dev/null
if [[ $? -eq 0 ]]; then
    echo "200 OK: Header Success (3)"; 
else 
    echo "200 OK: Header Failed (3)";
    rm ./response_file.txt ./response_header.txt ./okay_header.txt ./bad_header.txt ./integration_config.txt
    kill -9 $(( $pid_server + 1 ))
    exit 1;
fi

# clean up test 3 files
rm ./response_file.txt ./response_header.txt



# TEST 4 (400 Bad Response) on a GET with two periods (bad format)
echo "GET / HTTP/1..1
Host: www.w3.org/pub/WWW/TheProject.html" > "test_file.txt"

cat test_file.txt | nc localhost $PORT_NUM | tr -d '\r'> "response_file.txt"

# Check for a 400 Bad Request Header
head -n 1 "response_file.txt" > response_header.txt
diff response_header.txt bad_header.txt &>/dev/null
if [[ $? -eq 0 ]]; then
    echo "400 Bad Request: Header Success (4)"; 
else 
    echo "400 Bad Request: Header Failed (4)";
    rm ./test_file.txt ./response_file.txt ./response_header.txt ./okay_header.txt ./bad_header.txt ./integration_config.txt
    kill -9 $(( $pid_server + 1 ))
    exit 1;
fi

# Check the body of the good response
sed -i '1, 4d' "response_file.txt"
diff response_file.txt test_file.txt &>/dev/null
if [[ $? -eq 0 ]]; then
    echo "400 Bad Request: Body Success (4)"; 
else 
    echo "400 Bad Request: Body Failed (4)"; 
    rm ./test_file.txt ./response_file.txt ./response_header.txt ./okay_header.txt ./bad_header.txt ./integration_config.txt
    kill -9 $(( $pid_server + 1 ))
    exit 1;
fi

# clean up test 4 files
rm ./test_file.txt ./response_file.txt ./response_header.txt


### TODO: Uncomment these tests after we finish the static files capability

# # TEST 5 (200 OK) on a GET to a static image
# curl -isS "http://localhost:$PORT_NUM/static/example_image.jpg" -o "response_file.jpg" 

# # Check for matching file contents
# diff response_file.jpg ../tests/static_data/example_image.txt &>/dev/null
# if [[ $? -eq 0 ]]; then
#     echo "200 OK: Image GET Success (5)"; 
# else 
#     echo "200 OK: Image GET Failed (5)";
#     rm ./response_file.jpg ./okay_header.txt ./bad_header.txt ./integration_config.txt
#     kill -9 $(( $pid_server + 1 ))
#     exit 1;
# fi

# # clean up test 5 files
# rm ./response_file.jpg



# # TEST 6 (400 Bad Request) on a GET to a non-existant static image
# curl -isS "http://localhost:$PORT_NUM/static/does_not_exist.jpg" -o "response_file.txt" 

# # Check for bad request header
# head -n 1 "response_file.txt" | tr -d '\r'> response_header.txt
# diff response_header.txt bad_header.txt &>/dev/null
# if [[ $? -eq 0 ]]; then
#     echo "400 Bad Request: Header Success (6)"; 
# else 
#     echo "400 Bad Request: Header Failed (6)";
#     rm ./response_file.txt ./response_header.txt ./okay_header.txt ./bad_header.txt ./integration_config.txt
#     kill -9 $(( $pid_server + 1 ))
#     exit 1;
# fi

# # clean up test 6 files
# rm ./response_file.jpg ./response_header.txt 



# kill the server, clean up global files, and end the integration test
rm ./integration_config.txt ./okay_header.txt ./bad_header.txt
kill -9 $(( $pid_server + 1 ))
echo "Finished Integration Test"
exit 0