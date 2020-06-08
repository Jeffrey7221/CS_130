### Test Coverage Container ###
# Define builder stage
FROM wfh-web-server:base as builder

# Share work directory
COPY . /usr/src/project
WORKDIR /usr/src/project/build_coverage

# Build and test
RUN cmake -DCMAKE_BUILD_TYPE=Coverage -DCMAKE_PREFIX_PATH=/usr/local/lib ..
RUN make
RUN cd .. \
    && mkdir build \
    && cd build \
    && cmake -DCMAKE_PREFIX_PATH=/usr/local/lib .. \
    && make
RUN make coverage