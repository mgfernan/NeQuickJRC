FROM python:3.12-slim-bookworm

# Set the working directory
WORKDIR /workspace

# Copy the rest of the project files
COPY . .

RUN apt-get update && \
    apt-get install -y \
    gcc \
    git \
    build-essential
