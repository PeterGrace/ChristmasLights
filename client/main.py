from kivy.uix.tabbedpanel import TabbedPanel
from kivy.uix.colorpicker import ColorPicker
from kivy.app import App
from kivy.uix.floatlayout import FloatLayout
import requests
from ssdp_search import getSSDP


class RootWidget(FloatLayout):
    def refresh_zones(self):
        devices=getSSDP()
        
    def send_colors(self, cpicker):
        print(cpicker)
        if isinstance(cpicker, ColorPicker):
            url="http://10.65.3.236/lights"
            payload={}
            payload["mode"]=90
            payload["hue"] = int(cpicker.hsv[0]*255)
            payload["saturation"] = int(cpicker.hsv[1]*255)
            payload["brightness"] = int(cpicker.hsv[2]*255)
            requests.post(url, json=payload)
            print("Changing colors.")

class MainApp(App):
    def build(self):
        r = RootWidget()
        return r

if __name__ == '__main__':
    MainApp().run()

