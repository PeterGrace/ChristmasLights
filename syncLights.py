from requests import Session
from datetime import datetime, timedelta

strips = [
    '10.65.3.239',
    '10.65.3.249'
]


s = Session()

time = datetime.now().timestamp()
five_secs = time+5

for strip in strips:
    url = "http://{}/lights".format(strip)
    payload = {}
    payload['sync_start'] = five_secs
    # 3 is confetti mode, 5 is rainbow bpm mode
    payload['mode'] = 5;
    payload['bpm'] = 2000;
    s.post(url, json=payload)


