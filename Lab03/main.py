from scapy.all import rdpcap, IP

def main():
    pcap = rdpcap("h1toh3.pcap")
    
    # Filters packets of other types as they aren't of interest
    valid = []
    for p in pcap:
        if IP in p:
            valid.append(p)
        
    # From the first packet gets source and destination address
    source_addr = valid[0][IP].src
    dest_addr = valid[0][IP].dst
    print(f"Source IP address: {source_addr}")
    print(f"Destination IP address: {dest_addr}")

    # Calculates the number of bytes sent by each host
    bytes_sent_1 = 0
    bytes_sent_2 = 0
    for p in valid:
        if p[IP].src == source_addr:
            bytes_sent_1 += len(p[IP])
        else:
            bytes_sent_2 += len(p[IP])

    # Gets start and end time for each host and convert to seconds
    start_time_1 = valid[0][IP].time 
    end_time_1 = valid[-2][IP].time 
    start_time_2 = valid[1][IP].time 
    end_time_2 = valid[-1][IP].time

    # Calculates throughput
    print("Throughput:")

    for idx, p in enumerate(valid):
        print(idx, p[IP].time)


    tp_1 = bytes_sent_1 / (end_time_1 - start_time_1)
    tp_2 = bytes_sent_2 / (end_time_2 - start_time_2)
    print(f"- Source throughput: {tp_1: .3f} bytes/s")
    print(f"- Destination throughput: {tp_2: .3f} bytes/s")


main()