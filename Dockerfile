# Use a C++ build environment
FROM gcc:latest

# Install a web library (example using Crow dependencies)
RUN apt-get update && apt-get install -y libasio-dev

# Copy your code into the container
COPY . /usr/src/myapp
WORKDIR /usr/src/myapp

# Compile your C++ code
RUN g++ oops.cpp -lpthread -o my_backend

# Tell the cloud which port to use
EXPOSE 8080

# Run the backend
CMD ["./my_backend"]