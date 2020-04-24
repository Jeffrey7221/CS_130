### Build/test container ###
# Define builder stage
FROM wfh-web-server:base as builder

# Share work directory
COPY . /usr/src/project
WORKDIR /usr/src/project/build

# Build and test
RUN cmake ..
RUN make
RUN ctest --output-on_failure


### Deploy container ###
# Define deploy stage
FROM ubuntu:bionic as deploy

# Copy server output binary to "."
COPY --from=builder /usr/src/project/build/bin/server .
COPY --from=builder /usr/src/project/config/nginx_config .

# Expose some port(s)
EXPOSE 80

# Use ENTRYPOINT to specify the binary name
# Update with real server name
ENTRYPOINT ["./server"]

# Use CMD to specify arguments to ENTRYPOINT
# Update with real server args
CMD ["nginx_config"]
