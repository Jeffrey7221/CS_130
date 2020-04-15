1. Build and tag a base image
docker build -f docker/base.Dockerfile -t wfh-web-server:base .

2. Build an image for our webserver
docker build -f docker/Dockerfile -t wfh_web_server_image .

To build the container for test coverage
docker build -f docker/coverage.Dockerfile -t wfh_web_server_tcoverage .

3. Run the image for our webserver, mapping the local port 8080 to the container's port 80
docker run --rm -p 8080:80 --name my_run wfh_web_server_image:latest

4. To stop the currently running Docker image, use the following command in another terminal window
docker container stop my_run
