#!/bin/bash

# Clean up old socket
rm -f /tmp/nflambda_5gcore.sock

# Start 5G Core server
echo "Starting NFLambda 5G Core..."
./build/nflambda_5gcore &
SERVER_PID=$!

# Wait for server to initialize
sleep 2

# Send test message
echo ""
echo "Running test client..."
./build/nflambda_5gcore_test_client

# Give server time to process
sleep 1

# Kill server
echo ""
echo "Stopping server..."
kill $SERVER_PID 2>/dev/null

# Clean up
rm -f /tmp/nflambda_5gcore.sock

echo "Test complete"