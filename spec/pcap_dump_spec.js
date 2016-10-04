var
  rewire = require("rewire"),
  pcap_dump = rewire("../pcap_dump"),
  should = require("should"),
  sinon = require("sinon");

describe("pcap_dump", function() {
  beforeEach(function() {
    var pcapServiceMock = {

      default_device: function() {
        return "en0";
      },
      findalldevs: function() {
        return ["en0", "eth0"];
      },
      /* jshint ignore:start */
      pcap_dump_async: function(device_name, filter, buffer_size, outfile, packet_ready, is_monitor, packet_write_complete) {
          return "LINKTYPE_ETHERNET";
        }
        /* jshint ignore:end */
    };
  
    pcap_dump.__set__({
      "binding": pcapServiceMock

    });

  });



  describe("#start should take the default values ", function() {
    beforeEach(function() {
      this.instance = pcap_dump.createPcapDumpSession();
    });
    it("buffer size should be 10485760 ", function() {
      this.instance.startAsyncCapture();
      should(this.instance.buffer_size).be.equal(10485760);
    });

    it("outfile should be tmp.pcap ", function() {
      this.instance.startAsyncCapture();

      should(this.instance.outfile).be.equal("tmp.pcap");

    });

    it("number_of_packets_to_be_read should be 1", function() {
      this.instance.startAsyncCapture();

      should(this.instance.number_of_packets_to_be_read).be.equal(1);

    });

    it("is_monitor should be false", function() {
      this.instance.startAsyncCapture();

      should(this.instance.is_monitor).be.equal(false);

    });


  });


  describe("#start should values given in  ", function() {
    beforeEach(function() {
      //(device_name, filter, buffer_size, outfile, packet_ready, is_monitor)
      this.instance = pcap_dump.createPcapDumpSession("eth1", "dst", 1000, "", "", false);
    });
    it("buffer size should be 1000 ", function() {
      this.instance.startAsyncCapture();
      should(this.instance.buffer_size).be.equal(1000);
    });
    it("findalldevs  should return the arrayy ", function() {
      this.instance.startAsyncCapture();
      var devices = pcap_dump.findalldevs();
      should(devices[0]).be.equal("en0");
      should(devices[1]).be.equal("eth0");

    });

  });


  describe("#Error and success should emit events  ", function() {
    beforeEach(function() {
     
    });
    it("on Success a packet should be emitted ", function() {
       var spy = sinon.spy();
       var dump = pcap_dump.createPcapDumpSession("eth1", "dst", 1000, "", "", false);
       dump.on('packet', spy);
       dump.on_packet("mypacket");
       sinon.assert.calledWith(spy, "mypacket");
    });
     it("on Success of pcap_write_complete_async an event should be emitted ", function() {
       var spy = sinon.spy();
       var dump = pcap_dump.createPcapDumpSession("eth1", "dst", 1000, "", "", false);
       dump.on('pcap_write_complete_async', spy);
       dump.on_pcap_write_complete_async(null,24);
       
       sinon.assert.calledWith(spy, {
            "packets_read": 24,
            "fileName":  "tmp.pcap"
        });
    });
    it("on error of pcap_write_complete_async an event should be emitted ", function() {

       var spy = sinon.spy();
       var dump = pcap_dump.createPcapDumpSession("eth1", "dst", 1000, "", "", false);
       dump.on('pcap_write_error', spy);
       dump.on_pcap_write_complete_async("myerror",24);
       
       sinon.assert.calledWith(spy, "myerror");
    });

  });



});