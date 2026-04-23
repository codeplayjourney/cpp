# compile
g++ -std=c++17 exchange.cpp -o exchange
g++ -std=c++17 feed_handler.cpp -o feed_handler

# terminal 1 — start receiver first
./feed_handler
# terminal 2 — start sender
./exchange

multicast works for anyone subscribed within local network
unicast has 1-1 connection and has to have to specific address