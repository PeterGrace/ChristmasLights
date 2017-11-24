from kivy.uix.tabbedpanel import TabbedPanel
from kivy.uix.colorpicker import ColorPicker
from kivy.app import App
from kivy.uix.floatlayout import FloatLayout
import requests



class RootWidget(FloatLayout):
    def send_colors(self, cpicker):
        print(cpicker)
        if isinstance(cpicker, ColorPicker):
            url="http://10.65.3.236/lights"
            payload={}
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

