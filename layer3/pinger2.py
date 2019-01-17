#coding = utf-8
import logging
import sys
logging.getLogger("scapy.runtime").setLevel(logging.ERROR)
from scapy.all import * #brzydko tak importowac wszystko :<


# Traceback (most recent call last):                                                                                        
# File "pinger.py", line 16, in <module>                                                                                    
# answer = sr1(ARP(pdst=prefix+str(addr)), timeout=1, verbose=0)                                                        
# File "C:\Users\dp\Projects\\networking\env\lib\site-packages\scapy\sendrecv.py", line 393, in sr1                          
# s=conf.L3socket(promisc=promisc, filter=filter, nofilter=nofilter, iface=iface)                                       
# File "C:\Users\dp\Projects\\networking\env\lib\site-packages\scapy\\arch\windows\__init__.py", line 1234, in __init__       
# raise RuntimeError("Sniffing and sending packets is not available: "                                                
# RuntimeError: Sniffing and sending packets is not available: winpcap is not installed 
# <- trzeba zainstalowac wireshark z winpcap


if len(sys.argv) !=2:
    print("Usage -./pinger.py [/24 network_address]")
    print("Example - ./pinger.py 172.16.36.0")
    print("Example will perform an ICMP scan of the 172.16.36.0/24")
    sys.exit()

address = str(sys.argv[1])
prefix = address.split('.')[0] + '.' + address.split('.')[1] + '.' + address.split('.')[2] + '.'

for addr in range(0,254):
    answer = sr1(ICMP(pdst=prefix+str(addr)), timeout=1, verbose=0)
    # print(prefix+str(addr))
    if answer == None:
        print("-------------------")
    else:
        print(answer)
        print(prefix+str(addr))