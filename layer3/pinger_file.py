#coding = utf-8
import logging
import sys
logging.getLogger("scapy.runtime").setLevel(logging.ERROR)
from scapy.all import * #brzydko tak importowac wszystko :<
import base64

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
    print("Usage -./pinger_file.py <filename>")
    print("Example - ./pinger_file.py iplist.txt")
    print("Example will perform an ICMP scan of the  IP addressed listed in iplist.txt")
    sys.exit()


ip_list = open(str(sys.argv[1]), 'r')


for addr in ip_list:
    answer = sr1( IP(dst=addr.strip())/ICMP() , timeout=1, verbose=0) #opakowane w pakiet protokolu IP - routowalne poza lokalna siec
    if answer:
        print(addr.strip())
        print(answer.display())
        print("----------------------")