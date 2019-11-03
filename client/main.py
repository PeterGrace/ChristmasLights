from kivy.uix.tabbedpanel import TabbedPanel
from kivy.uix.colorpicker import ColorPicker
from kivy.app import App
from kivy.uix.floatlayout import FloatLayout
from ssdp import getSSDP
from kivy.logger import Logger

from kivymd.bottomsheet import MDListBottomSheet, MDGridBottomSheet
from kivymd.button import MDIconButton
from kivymd.date_picker import MDDatePicker
from kivymd.dialog import MDDialog
from kivymd.label import MDLabel
from kivymd.list import ILeftBody, ILeftBodyTouch, IRightBodyTouch, BaseListItem, OneLineListItem,TwoLineListItem
from kivymd.material_resources import DEVICE_TYPE
from kivymd.navigationdrawer import MDNavigationDrawer, NavigationDrawerHeaderBase
from kivymd.selectioncontrols import MDCheckbox
from kivymd.snackbar import Snackbar
from kivymd.theming import ThemeManager
from kivymd.time_picker import MDTimePicker
import httpx
from async_gui.toolkits.kivy import KivyEngine

engine = KivyEngine()

class RootWidget(FloatLayout):
    def refresh_zones(self, caller=None):
        self.ids.lst_zonelist.clear_widgets()
        self.ids.lst_zonelist.add_widget(OneLineListItem(text="Refresh Zones", on_release=self.refresh_zones))
        getSSDP(self.populate_zones)
    
    def populate_zones(self, devices):
        if devices is not None:
            if 'root' in devices:
                if 'URLBase' in devices['root']:
                    self.urlbase = devices['root']['URLBase']
                    Logger.info(f"set urlbase to {self.urlbase}")
                    self.ids.lst_zonelist.add_widget(TwoLineListItem(text=devices['root']['device']['friendlyName'],secondary_text=devices['root']['URLBase']))
            else:
                self.urlbase = ""
        else:
            self.urlbase= ""

        pass
    def reboot_unit(self):
        url=f"{self.urlbase}reboot/please"
        payload={}
        try:
            httpx.post(url, json=payload, timeout=(2,2))
            Logger.info("request sent")
        except Exception:
            Logger.warn("unable to set mode.")        

    def set_mode(self, modenum):
        url=f"{self.urlbase}lights"
        payload={}
        payload["mode"]=modenum
        payload["brightness"]=128
        try:
            httpx.post(url, json=payload, timeout=(2,2))
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
                httpx.post(url, json=payload, timeout=(2,2))
                Logger.info("Request sent.")
            except Exception:
           	    Logger.warn("could not contact lights.")
            

class MainApp(App):
    theme_cls = ThemeManager()
    def build(self):
        r = RootWidget()        
        return r

class IconLeftSampleWidget(ILeftBodyTouch, MDIconButton):
    pass


if __name__ == '__main__':
    MainApp().run()

