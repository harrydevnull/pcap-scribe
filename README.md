
Capturing packets live and write into file using nodejs and libpcap

pcap-scribe
================


[![Build Status](https://travis-ci.org/harrydevnull/pcap-scribe.svg?branch=master)](https://travis-ci.org/harrydevnull/pcap-scribe)
[![Coverage Status](https://coveralls.io/repos/github/harrydevnull/pcap-scribe/badge.svg?branch=master)](https://coveralls.io/github/harrydevnull/pcap-scribe?branch=master)

## Installation

You will need `libpcap` installed.  Most OSX machines seem to have it.  All major Linux distributions have it available
either by default or with a package like `libpcap-dev`.

The easiest way to get `pcap-scribe` and its tools is with `npm`:

    npm install pcap-scribe

If you want to hack on the source code, you can get it from github.  Clone the repo like this:

    git clone https://github.com/harrydevnull/pcap-scribe

To compile the native code bindings, do this:

    cd pcap-scribe
    npm install 

Assuming it built without errors, you should be able to run the examples and then write your own packet
capture programs.

### Starting a capture session

To start a capture session, call `pcap.PcapDumpSession` with an interface name and a pcap filter string:

    var pcap = require("pcap-scribe"); 
        pcap_dump = new pcap.PcapDumpSession(interface, filter,bufferSize,pcap file name ,false,Number of packets to be captured);
        
        pcap_dump.on('pcap_write_complete_async',function(message){
                console.log("done.....",message);
        });

        pcap_dump.on('pcap_write_error',function(message){
                console.log("pcap_write_error.....",message);
        });

        pcap_dump.startAsyncCapture();

        //eg: pcap_dump = new pcap.PcapDumpSession('en0', "ip proto \\tcp",10*1024*1024,"tmp95.pcap",false,5);


`interface` is the name of the interface on which to capture packets.  If passed an empty string, `libpcap`
will try to pick a "default" interface, which is often just the first one in some list and not what you want.

`filter` is a pcap filter expression, see `pcap-filter(7)` for more information.  An empty string will capture
all packets visible on the interface.

Note that `pcap-scribe` always opens the interface in promiscuous mode, which generally requires running as root.

`PcapDumpSession` is an `EventEmitter` that emits either `pcap_write_complete_async` event when the write to file is complete
or `pcap_write_error` event in case there is an error.



