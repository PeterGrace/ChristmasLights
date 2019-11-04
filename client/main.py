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
from kivy.clock import Clock
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
        self.ids.scr_mngr.current = 'Loading'
        self.canvas.ask_update()
        self.ids.lst_zonelist.clear_widgets()
        self.ids.lst_zonelist.add_widget(OneLineListItem(text="Refresh Zones", on_release=self.refresh_zones))
        Clock.schedule_once(self.trigger_ssdp_fetch,1)
    
    def trigger_ssdp_fetch(self,dt):
        getSSDP(self.populate_zones)

    def populate_zones(self, devices):
        if devices is not None:
            for device in devices:
                if 'root' in device:
                    if 'URLBase' in device['root']:
                        self.urlbase = device['root']['URLBase']
                        Logger.info(f"set urlbase to {self.urlbase}")
                        self.ids.lst_zonelist.add_widget(TwoLineListItem(text=device['root']['device']['friendlyName'],secondary_text=device['root']['URLBase'],on_release=self.set_urlbase))
                    elif 'device' in device['root']:
                        if 'serviceList' in device['root']['device']:
                            if 'service' in device['root']['device']['serviceList']:
                                if 'URLBase' in device['root']['device']['serviceList']['service']:
                                    self.ids.lst_zonelist.add_widget(
                                        TwoLineListItem(text=device['root']['device']['friendlyName'],
                                        secondary_text=device['root']['device']['serviceList']['service']['URLBase'],
                                        on_release=self.set_urlbase)
                                    )
        self.ids.scr_mngr.current = 'Zones'

    
    def set_urlbase(self, callee):
        if callee.secondary_text[-1] != '/':
            self.urlbase = callee.secondary_text
            self.urlbase += "/"
        else:
            self.urlbase = callee.secondary_text
        Logger.info(f"URLBase = {self.urlbase}")
        Snackbar(text=f"Now addressing {callee.text}").show()
        self.ids.toolbar.title=f"{callee.text} - CLUI"


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

