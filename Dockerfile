# Use an ARM64 base image
FROM arm64v8/ubuntu:latest

# Use a non-interactive frontend for apt to avoid prompts
ENV DEBIAN_FRONTEND=noninteractive

# Install build tools and libraries
RUN apt-get update -qq && \
    apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    git && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# Copy project files into the container
COPY . /usr/src/myapp

# Set the working directory
WORKDIR /usr/src/myapp

# Build your application (adjust as necessary)
# Ensure your Makefile is properly configured to not require user input
RUN make

# Make sure the launch script is executable
RUN chmod +x ./launch.sh

# Command to run your application
CMD ["./launch.sh"]
