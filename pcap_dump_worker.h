#ifndef PCAPDUMPWORKER_H
#define PCAPDUMPWORKER_H
#include <nan.h>
#include <string>
#include <pcap/pcap.h>

class PcapDumpWorker : public Nan::AsyncWorker {

 private:
    std::string device;
    std::string filter;
    int buffer_size;
    std::string pcap_output_filename;
    int num_packets;
    struct bpf_program fp;
    bpf_u_int32 mask;
    bpf_u_int32 net;
    pcap_t *pcap_handle;
    pcap_dumper_t *pcap_dump_handle;
    char errbuf[PCAP_ERRBUF_SIZE];

 public:
    PcapDumpWorker(
        Nan::Callback *callback,
        std::string device,
        std::string filter,
        int buffer_size,
        std::string pcap_output_filename,
        int num_packets
    )
    :
    Nan::AsyncWorker(callback),
    device(device),
    filter(filter),
    buffer_size(buffer_size),
    pcap_output_filename(pcap_output_filename),
    num_packets(num_packets)
    {}

    ~PcapDumpWorker() {
    }


    void Execute ();
    void HandleOKCallback ();
    static void OnPacketReady(u_char *s, const struct pcap_pkthdr* pkthdr, const u_char* packet);

};

#endif
