
import 'package:flutter/material.dart';
import 'package:upnp/upnp.dart';

class AppState with ChangeNotifier {
  DeviceDiscoverer discoverer = new DeviceDiscoverer();
  List<Device> _devices = <Device>[];
  String _url = "";

  set deviceList(List<Device> devs) {
    _devices = devs;
    notifyListeners();
  }
  List<Device> get deviceList {
    return _devices;
  }
  void addDevice(Device dev) {
    _devices.add(dev);
    notifyListeners();
  }
  void clearDevices() {
    _devices.clear();
    notifyListeners();
  }

  set url(String newurl) {
    _url = newurl;
    notifyListeners();
  }
  get url {
    return _url;
  }
}