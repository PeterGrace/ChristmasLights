from kivy.uix.tabbedpanel import TabbedPanel
from kivy.uix.colorpicker import ColorPicker
from kivy.app import App
from kivy.uix.floatlayout import FloatLayout
import requests
from ssdp_search import getSSDP
from kivy.logger import Logger


class RootWidget(FloatLayout):
    def refresh_zones(self):
        devices=getSSDP()
        if devices is not None:
            if 'root' in devices:
                if 'URLBase' in devices['root']:
                    self.urlbase = devices['root']['URLBase']
                    Logger.info(f"set urlbase to {self.urlbase}")
                    self.ids.btn_refresh.text = self.urlbase
            else:
                self.urlbase = ""
        else:
            self.urlbase= ""

        pass
    def reboot_unit(self):
        url=f"{self.urlbase}reboot/please"
        payload={}
        try:
            requests.post(url, json=payload, timeout=(2,2))
            Logger.info("request sent")
        except Exception:
            Logger.warn("unable to set mode.")        

    def set_mode(self, modenum):
        url=f"{self.urlbase}lights"
        payload={}
        payload["mode"]=modenum
        payload["brightness"]=128
        try:
            requests.post(url, json=payload, timeout=(2,2))
            Logger.info("request sent")
        except Exception:
            Logger.warn("unable to set mode.")
    def send_colors(self, cpicker):
        print(cpicker)
        if isinstance(cpicker, ColorPicker):
            url=f"{self.urlbase}lights"
            payload={}
            payload["mode"]=90
            payload["hue"] = int(cpicker.hsv[0]*255)
            payload["saturation"] = int(cpicker.hsv[1]*255)
            payload["brightness"] = int(cpicker.hsv[2]*255)
            try:
                requests.post(url, json=payload, timeout=(2,2))
                Logger.info("Request sent.")
            except Exception:
           	    Logger.warn("could not contact lights.")
            

class MainApp(App):
    def build(self):
        r = RootWidget()
        r.refresh_zones()
        return r

if __name__ == '__main__':
    MainApp().run()

