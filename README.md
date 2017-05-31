# RTPCapturer


RTP packet Capturring from Network adaptor or .pcap file

To process .pcap file example =  RTPCapturer -f C:\Users\IbrahimD\Desktop\pcap_goruntulu\vp8.pcap


This project captures RTP Packets with payload type 8 (G.711) and payload type 96 (VP8 video codec).

G.711 rtp packets decoded as .wav file to the folder /wav_files/
VP8 raw data is extracted fromRTP and stored in IVF file format
