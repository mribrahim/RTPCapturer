# RTPCapturer


RTP packet Capturing from Network Adaptor or .pcap file

To process .pcap file, give file path as argument 

-- example ->  **RTPCapturer -f C:\Users\IbrahimD\Desktop\pcap_goruntulu\vp8.pcap**


This project captures RTP Packets with payload type 8 (G.711) and payload type 96 (VP8 video codec).


- G.711 rtp packets decoded as .wav file to the folder /wav_files/

- VP8 raw data is extracted from RTP and stored in IVF file format
