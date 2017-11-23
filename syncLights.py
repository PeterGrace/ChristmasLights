import requests
from multiprocessing.dummy import Pool as ThreadPool

strips = [
    '10.65.3.239',
    '10.65.3.249'
]

payload={}
payload = {}
payload['sync_start'] = 10000
# 3 is confetti mode, 5 is rainbow bpm mode
payload['mode'] = 3;
payload['bpm'] = 300;


def send_message(ipaddr):
    url = "http://{}/lights".format(ipaddr)
    requests.post(url, json=payload)
    

pool = ThreadPool(len(strips))
pool.map(send_message, strips)
pool.close()