### Build/test container ###
# Define builder stage
FROM wfh-web-server:base as builder

# Share work directory
COPY . /usr/src/project
WORKDIR /usr/src/project/build

RUN pkg-config --cflags --libs libmongocxx

# Build and test
RUN cmake -DCMAKE_PREFIX_PATH=/usr/local/lib ..
RUN make
RUN ctest --output-on_failure


### Deploy container ###
# Define deploy stage
FROM ubuntu:bionic as deploy

WORKDIR /app/build

# Copy server output binary to "."
COPY --from=builder /usr/src/project/build/bin/server .
COPY --from=builder /usr/src/project/config/nginx_config .
COPY --from=builder /usr/src/project/data/ ../data/
COPY --from=builder /usr/local /usr/local
COPY --from=builder /usr/share /usr/share
COPY --from=builder /usr/lib /usr/lib
COPY --from=builder /usr/bin /usr/bin

# Set the library path appropriately
RUN LD_LIBRARY_PATH=/usr/local/lib \
    && export LD_LIBRARY_PATH 

# Expose some port(s)
EXPOSE 80

# Use ENTRYPOINT to specify the binary name
# Update with real server name
ENTRYPOINT ["./server"]

# Use CMD to specify arguments to ENTRYPOINT
# Update with real server args
CMD ["nginx_config"]
