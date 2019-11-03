import socket
import httpx
import xmltodict
import json
import re
import asyncio
import netifaces
from async_gui.toolkits.kivy import KivyEngine

engine = KivyEngine()

msg = \
    'M-SEARCH * HTTP/1.1\r\n' \
    'HOST:239.255.255.250:1900\r\n' \
    'ST:urn:schemas-upnp-org:device:ControllableLight:1\r\n' \
    'MX:2\r\n' \
    'MAN:"ssdp:discover"\r\n' \
    '\r\n'
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
s.settimeout(2)
s.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
gws = netifaces.gateways()
ifidx = gws['default'][netifaces.AF_INET][1]
ifip = netifaces.ifaddresses(ifidx)[netifaces.AF_INET][0]['addr']
s.bind((ifip, 65507))

# Set up UDP socket
@engine.async
def getSSDP(callback):

    s.sendto(msg.encode('ascii'), ('239.255.255.250', 1900) )
    s.sendto(msg.encode('ascii'), ('239.255.255.250', 1900) )
    s.sendto(msg.encode('ascii'), ('239.255.255.250', 1900) )

    gotcha = False

    try:
        while not gotcha:
            data, addr = s.recvfrom(65507)
            gotcha=True
            print("Received response from {}".format(addr[0]))
            m=re.search(r"LOCATION: (.*?xml)", str(data))
            if m is not None:
                    loc=m.group(1)
                    print("Going to get data from {}".format(loc))
                    resp = httpx.get(loc)
                    respdict=xmltodict.parse(resp.text)
                    callback(respdict)

    except socket.timeout:
        print("Socket timeout")

