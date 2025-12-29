# Use a lightweight server that supports CGI
FROM alpine:latest

# Install C++ compiler and a small web server
RUN apk add --no-network --no-cache g++ make apache2-utils thttpd

# Create a folder for your app
WORKDIR /app

# Copy all your files (oops.cpp, oops.html)
COPY . .

# Compile your C++ code into a "CGI" executable
RUN g++ oops.cpp -o tracker.cgi

# Run a mini web server on port 8080
# It will serve oops.html and run tracker.cgi
CMD ["thttpd", "-D", "-p", "8080", "-d", "/app", "-c", "**.cgi"]