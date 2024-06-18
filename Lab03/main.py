from scapy.all import rdpcap, IP, ICMP

FIRST_SRC = 0
LAST_SRC = -2

FIRST_DEST = 1
LAST_DEST = -1

def get_average_time(addr, packets):
    time_taken = 0
    addr_packets = [packet for packet in packets if packet[IP].src == addr]
    
    for i in range(len(addr_packets) - 1):
        delta_time = addr_packets[i + 1].time - addr_packets[i].time
        time_taken += delta_time

    return time_taken / len(addr_packets)  

def pcap_analyze(file_name : str):
    pcap_packets = rdpcap(file_name)
    
    # Filters packets of other types as they aren't of interest
    filtered_packets = []
    for packet in pcap_packets:
        if IP in packet:
            filtered_packets.append(packet)
        
    # From the first packet gets source and destination address
    src_addr = filtered_packets[0][IP].src
    dest_addr = filtered_packets[0][IP].dst
    print(f"Source IP address: {src_addr}")
    print(f"Destination IP address: {dest_addr}")
    print()

    # Gets average time between packets, using src and dest addrs to diferentiate
    print("Average time between packets:")
    avg_time_src = get_average_time(src_addr, filtered_packets)
    avg_time_dest = get_average_time(dest_addr, filtered_packets)
    print(f"- Source: {avg_time_src: .3f} s")
    print(f"- Destination: {avg_time_dest: .3f} s")
    print()
    
    # Calculates the number of bytes sent by each host
    bits_sent_src = 0
    bits_sent_dest = 0
    for packet in filtered_packets:
        if packet[IP].src == src_addr:
            bits_sent_src += 8 * len(packet[ICMP])
        else:
            bits_sent_dest += 8 * len(packet[ICMP])

    # Gets start and end time for each host and convert to seconds
    start_time_src = filtered_packets[FIRST_SRC].time 
    end_time_src = filtered_packets[LAST_SRC].time 
    start_time_dest = filtered_packets[FIRST_DEST].time 
    end_time_dest = filtered_packets[LAST_DEST].time

    # Calculates throughput
    print("Throughput:")
    tp_src = bits_sent_src / (end_time_src - start_time_src)
    tp_dest = bits_sent_dest / (end_time_dest - start_time_dest)
    print(f"- Source: {tp_src: .3f} bits/s")
    print(f"- Destination: {tp_dest: .3f} bits/s")
    print()

    # Calculates total number of packets
    print("Total number of packets received:")
    src_count = 0
    dest_count = 0
    for packet in filtered_packets:
        if packet[IP].dst == src_addr:
            src_count += 1
        elif packet[IP].dst == dest_addr:
            dest_count += 1
    print(f"- Source: {src_count} packets")
    print(f"- Destination: {dest_count} packets")

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
