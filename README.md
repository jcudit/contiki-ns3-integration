contiki-ns3-integration
=======================

Contiki customized to integrate with ns-3

cd contiki/examples/ns3-ping6
make clean
make
# this produces the contiki executable example-ping6.ns3
cd ../../../ns3
./waf build
./waf --run socket-bridge-example
