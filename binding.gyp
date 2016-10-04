{
  "targets": [
    {
      "target_name": "pcap_binding",
      "sources": [  "pcap_dump_worker.cpp","pcap_binding.cc" ],
      "include_dirs" : [
    "<!(node -e \"require('nan')\")"
],
      
      "link_settings": {
          "libraries": [
              "-lpcap"
          ]
      }
    }
  ]
}