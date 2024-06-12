from scapy.all import rdpcap, IP

FIRST_SRC = 0
LAST_SRC = -2

FIRST_DEST = 1
LAST_DEST = -1

def main():
    pcap_packets = rdpcap("h1toh3.pcap")
    
    # Filters packets of other types as they aren't of interest
    filtered_packets = []
    for packet in pcap_packets:
        if IP in packet:
            filtered_packets.append(packet)
        
    # From the first packet gets source and destination address
    source_addr = filtered_packets[0][IP].src
    dest_addr = filtered_packets[0][IP].dst
    print(f"Source IP address: {source_addr}")
    print(f"Destination IP address: {dest_addr}")

    # Calculates the number of bytes sent by each host
    bytes_sent_src = 0
    bytes_sent_dest = 0
    for packet in filtered_packets:
        if packet[IP].src == source_addr:
            bytes_sent_src += len(packet)
        else:
            bytes_sent_dest += len(packet)

    # Gets start and end time for each host and convert to seconds
    start_time_src = filtered_packets[FIRST_SRC].time 
    end_time_src = filtered_packets[LAST_SRC].time 
    start_time_dest = filtered_packets[FIRST_DEST].time 
    end_time_dest = filtered_packets[LAST_DEST].time

    # Calculates throughput
    print("Throughput:")

    tp_src = bytes_sent_src / (end_time_src - start_time_src)
    tp_dest = bytes_sent_dest / (end_time_dest - start_time_dest)
    print(f"- Source throughput: {tp_src: .3f} bytes/s")
    print(f"- Destination throughput: {tp_dest: .3f} bytes/s")


    # Calculates total number of packets
    print("Total number of packets received:")
    source_count = 0
    dest_count = 0
    for packet in filtered_packets:
        if packet[IP].dst == source_addr:
            source_count += 1
        elif packet[IP].dst == dest_addr:
            dest_count += 1
    print(f"- Source received: {source_count} packets")
    print(f"- Destination received: {dest_count} packets")


main()