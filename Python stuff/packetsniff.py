import os
import socket
s = socket.socket(socket.AF_INET, socket.SOCK_RAW)
s.setsockopt(socket.IPPROTO_IP, socket.IP_HDRINCL, 1)
s.bind(("192.168.0.199", 0))
s.ioctl(socket.SIO_RCVALL, socket.RCVALL_ON)
print(s.getsockname())
counter = 10

while counter != 0:
    packet = s.recv(4096)
    counter -= 1
    print(packet)
    
    #binaryparse

    hexxed=packet.hex()
    sepped = [hexxed[x:x+2] for x in range(0,len(hexxed),2)]
    # print(sepped)
    # print("")
    sepped = [hexxed[x:x+2] for x in range(0,40,2)]
    #separates the list into groupings of 2 hex characters...Remember that this means each list element equals one byte or octet (!).
    #['45', '00', '00', 'cb', '51', 'c0', '00', '00', '01', '11' prot, '00', '00', 'c0', 'a8', '00', 'c7', 'ef', 'ff', 'ff', 'fa']
    #is an example. Within each list element are 2 hex characters = 1 byte = 8 bits.
    #They don't have to be interpreted this way, for example list element 1 will always be 2 fields of 4 bits each i.e., version 4, IHL 5.
    #version, 4 bits
    version = list(sepped[0])[0]
    IHL = list(sepped[0])[1]
    DSCPandECN = sepped[1]
    # print(sepped)
    if DSCPandECN != "00":
        split_that_shit = f'{int(DSCPandECN,16):<b}'
        if len(split_that_shit) >= 6:
            DSCP = split_that_shit[0:6]
            DSCP = int(DSCP, 2)
            ECN = split_that_shit[6:8]
            ECN = int(ECN, 2)
        else:
                DSCP = int(split_that_shit, 2)
                ECN = "none"
    else:
        DSCP = "none"
        ECN = "none"
    length = sepped[2] + sepped[3]
    length = int(length, 16)
    ID = sepped[4] + sepped[5]
    ID = int(ID,16)
    flagfragment = sepped[6] + sepped[7]
    if flagfragment != "0000":
        faf_hex= int(flagfragment,16)
        faf_split = f'{faf_hex:<b}'
        flags = faf_split[0:3]
        flags = int(flags,2)
        if flags == 4:
            flags = "Don't fragment"
        else:
            flags="IDK!!!"
        fragment = faf_split[3:]
    else:
        flags = "none"
        fragment = "none"
    ttl = int(sepped[8],16)
    protocol = int(sepped[9],16)
    if protocol == 1:
           protocol='ICMP'
    elif protocol == 6:
           protocol='TCP'
    elif protocol == 17:
           protocol='UDP'
    elif protocol == 41:
           protocol='IPv6'
    elif protocol == 132:
           protocol=='SCTP'

    header_check=int((sepped[10]+sepped[11]),16)
    sourceip=sepped[12] + sepped [13] + sepped [14] + sepped [15]
    sourceip=int(sourceip,16)
    sourceipbin=f'{sourceip:b}'
    pog = [sourceipbin[x:x+8] for x in range(0,len(sourceipbin),8)]
    pog2 = [str(int(pog[x],2)) for x in range(0, len(pog))]
    sourceip='.'.join(pog2)

    destip=sepped[16] + sepped[17] + sepped[18] + sepped[19]
    destip=int(destip,16)
    destipbin=f'{destip:b}'
    pog3 = [destipbin[x:x+8] for x in range(0,len(destipbin),8)]
    pog4 = [str(int(pog3[x],2)) for x in range(0, len(pog3))]
    destip='.'.join(pog4)

    print(f"Version= {version}\nIHL= {IHL}\nDSCP= {DSCP}\nECN= {ECN}\nlength= {length}\nID= {ID}\nfragment flags= {flags}\nfragment= {fragment}\nttl= {ttl}\nprotocol= {protocol}\nheader_check= {header_check}\nsourceip= {sourceip}\ndestip= {destip}")