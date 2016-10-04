#include "pcap_dump_worker.h"
using namespace v8;
using Nan::Callback;
using Nan::AsyncQueueWorker;
using Nan::AsyncWorker;
using Nan::Callback;
using Nan::HandleScope;
using Nan::New;
using Nan::Null;
using Nan::To;

// Executed inside the worker-thread.
// It is not safe to access V8, or V8 data structures
// here, so everything we need for input and output
// should go on `this`.
void PcapDumpWorker::Execute () {
    if (pcap_lookupnet(device.c_str(), &net, &mask, errbuf) == -1) {
        net = 0;
        mask = 0;
        fprintf(stderr, "warning: %s - this may not actually work\n", errbuf);
        SetErrorMessage(errbuf);
        return;
    }
    pcap_handle = pcap_create(device.c_str(), errbuf);
    if (pcap_handle == NULL) {
        SetErrorMessage(errbuf);
        return;
    }

    // 64KB is the max IPv4 packet size
    if (pcap_set_snaplen(pcap_handle, 65535) != 0) {
       SetErrorMessage("error setting snaplen");
        return;
    }

    // always use promiscuous mode
    if (pcap_set_promisc(pcap_handle, 1) != 0) {
        SetErrorMessage("error setting promiscuous mode");
        return;
    }

    // Try to set buffer size.  Sometimes the OS has a lower limit that it will silently enforce.
    if (pcap_set_buffer_size(pcap_handle, buffer_size) != 0) {
        SetErrorMessage("error setting buffer size");
        return;
    }


    // set "timeout" on read, even though we are also setting nonblock below.  On Linux this is required.
    if (pcap_set_timeout(pcap_handle, 1000) != 0) {
        SetErrorMessage("error setting read timeout");
        return;
    }

    if (pcap_activate(pcap_handle) != 0) {
        SetErrorMessage(pcap_geterr(pcap_handle));
        return;
    }
    if ((pcap_output_filename.size()) > 0) {
        pcap_dump_handle = pcap_dump_open(pcap_handle,pcap_output_filename.c_str());
        if (pcap_dump_handle == NULL) {
            SetErrorMessage("error opening dump");
            return;
        }
    }


    if (filter.size() != 0) {
        if (pcap_compile(pcap_handle, &fp, filter.c_str(), 1, net) == -1) {
            SetErrorMessage(pcap_geterr(pcap_handle));
            return;
        }

        if (pcap_setfilter(pcap_handle, &fp) == -1) {
            SetErrorMessage(pcap_geterr(pcap_handle));
            return;
        }

        pcap_loop(pcap_handle, num_packets, OnPacketReady, (unsigned char *)pcap_dump_handle);
        pcap_freecode(&fp);
        /*
         * Close the savefile opened in pcap_dump_open().
         */
        pcap_dump_close(pcap_dump_handle);
        /*
         * Close the packet capture device and free the memory used by the
         * packet capture descriptor.
         */
        pcap_close(pcap_handle);
    }
}

// Executed when the async work is complete
// this function will be run inside the main event loop
// so it is safe to use V8 again
void PcapDumpWorker::HandleOKCallback () {

    Nan::HandleScope scope;

    Local<Value> argv[] = {
        Nan::Null()
      , New<Number>(num_packets)
    };

    callback->Call(2, argv);
}



void PcapDumpWorker::OnPacketReady(u_char *s, const struct pcap_pkthdr* pkthdr, const u_char* packet) {
   pcap_dump(s, pkthdr, packet);
}
