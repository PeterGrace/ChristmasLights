import socket
import requests
import xmltodict
import json
import re

msg = \
    'M-SEARCH * HTTP/1.1\r\n' \
    'HOST:239.255.255.250:1900\r\n' \
    'ST:urn:schemas-upnp-org:device:ControllableLight:1\r\n' \
    'MX:2\r\n' \
    'MAN:"ssdp:discover"\r\n' \
    '\r\n'

# Set up UDP socket
def getSSDP():

    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
    s.settimeout(2)
    s.bind(('0.0.0.0', 65507))
    s.sendto(msg.encode('ascii'), ('239.255.255.250', 1900) )
    s.sendto(msg.encode('ascii'), ('239.255.255.250', 1900) )
    s.sendto(msg.encode('ascii'), ('239.255.255.250', 1900) )

    gotcha = False

    try:
        while not gotcha:
            data, addr = s.recvfrom(65507)
            gotcha=True
            print("Received response from {}".format(addr[0]))
            m=re.search(r"LOCATION: (.*?xml)", data)
            if m is not None:
                    loc=m.group(1)
                    print("Going to get data from {}".format(loc))
                    resp = requests.get(loc)
                    respdict=xmltodict.parse(resp.text)
                    return respdict

    except socket.timeout:
        print("Socket timeout")

