import requests
from multiprocessing.dummy import Pool as ThreadPool

strips = [
    #'10.65.3.239',
    #'10.65.3.249',
    '10.65.3.236'
]

payload={}
payload = {}
#payload['sync_start'] = 2000
# 3 is confetti mode, 5 is rainbow bpm mode
payload['mode'] = 90;
payload['bpm'] = 10;
payload['hue'] = 0;
payload['saturation'] = 64;
payload['brightness'] = 255;


def send_message(ipaddr):
    url = "http://{}/lights".format(ipaddr)
    requests.post(url, json=payload)
    

pool = ThreadPool(len(strips))
pool.map(send_message, strips)
pool.close()