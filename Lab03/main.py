from scapy.all import rdpcap, IP, ICMP

FIRST_SRC = 0
LAST_SRC = -2

FIRST_DEST = 1
LAST_DEST = -1

def pcap_analyze(file_name : str):
    pcap_packets = rdpcap(file_name)
    
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
    print()

    # Calculates the number of bytes sent by each host
    bytes_sent_src = 0
    bytes_sent_dest = 0
    for packet in filtered_packets:
        if packet[IP].src == source_addr:
            bytes_sent_src += len(packet[ICMP])
        else:
            bytes_sent_dest += len(packet[ICMP])
    bits_sent_src = bytes_sent_src * 8
    bits_sent_dest = bytes_sent_dest * 8

    # Gets start and end time for each host and convert to seconds
    start_time_src = filtered_packets[FIRST_SRC].time 
    end_time_src = filtered_packets[LAST_SRC].time 
    start_time_dest = filtered_packets[FIRST_DEST].time 
    end_time_dest = filtered_packets[LAST_DEST].time

    
    # time_taken = 0
    # for i in range(len(filtered_packets)-1):
    #     delta_time = filtered_packets[i+1].time - filtered_packets[i].time
    #     time_taken += delta_time
    # print("@@@",time_taken/len(filtered_packets))
        

    time_taken = 0
    dest_packages = [a for a in filtered_packets if a[IP].src == dest_addr]
    for i in range(len(dest_packages) - 1):
        delta_time = dest_packages[i+1].time - dest_packages[i].time
        time_taken += delta_time
    print("@@@",time_taken/len(filtered_packets))
    print()



    # Calculates throughput
    print("Throughput:")
    tp_src = bits_sent_src / (end_time_src - start_time_src)
    tp_dest = bits_sent_dest / (end_time_dest - start_time_dest)
    print(f"- Source throughput: {tp_src: .3f} bits/s")
    print(f"- Destination throughput: {tp_dest: .3f} bits/s")
    print()

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

def main():

    #H1 TO H3
    print("-------------------------------------------------")
    print("H1 to H3, reading from file: h1toh3.pcap")
    print()
    pcap_analyze("h1toh3.pcap")
    print("-------------------------------------------------")
    
    #H2 TO H4
    print("H2 to H4, reading from file: h2toh4.pcap")
    print()
    pcap_analyze("h2toh4.pcap")
    print("-------------------------------------------------")
main()
