# Use a full GCC environment to compile
FROM gcc:latest AS builder
COPY . /app
WORKDIR /app
# Compile the C++ code
RUN g++ oops.cpp -o tracker.cgi

# Use a lightweight web server for the final "Live" app
FROM alpine:latest
RUN apk add --no-cache thttpd
WORKDIR /app
# Copy the compiled program and the HTML from the builder
COPY --from=builder /app/tracker.cgi .
COPY --from=builder /app/oops.html .
# Create empty data files so the app doesn't crash on start
RUN touch data.txt budget.txt && chmod 666 data.txt budget.txt

# Start the server on port 8080
CMD ["thttpd", "-D", "-p", "8080", "-d", "/app", "-c", "**.cgi"]
