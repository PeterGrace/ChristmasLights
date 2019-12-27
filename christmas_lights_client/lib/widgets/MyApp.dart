import 'dart:developer';
import 'package:flushbar/flushbar.dart';
import 'package:flutter/material.dart';
import 'package:christmas_lights_client/values/values.dart';
import 'package:provider/provider.dart';

class MyApp extends StatelessWidget {
  // This widget is the root of your application.
  @override
  Widget build(BuildContext context) {
    return Provider<AppState>(
        create: (context) => new AppState(),
        child: MaterialApp(
          title: appName,
          theme: ThemeData(
            primarySwatch: Colors.blue,
          ),
          home: MyHomePage(),
        ));
  }
}

class MyHomePage extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return Scaffold(
            appBar: AppBar(
              title: Text(appName),
            ),
            body: renderBody(context),
            floatingActionButton: FloatingActionButton(onPressed: () {
              syncDevices(context);
            }));
  }
}

void syncDevices(BuildContext context) async {
  var state = Provider.of<AppState>(context);
  var disc = state.discoverer;
  Provider.of<AppState>(context).clearDevices();
  await disc.start();
  //disc.quickDiscoverClients(query: "urn:schemas-upnp-org:device:ControllableLight:1").listen((client) {
  //disc.quickDiscoverClients(query: "device:ControllableLight:1").listen((client) {
  disc.quickDiscoverClients().listen((client) {
    try {
      client.getDevice().then((dev) {
        Provider.of<AppState>(context).addDevice(dev);
        log("Added device ${dev.friendlyName}");
      });
    } catch (e) {
      log("ERROR: $e - ${client.location}");
    }
  });
}

Widget renderBody(BuildContext context) {
  return Center(
      child: ListView.builder(
          itemCount: Provider.of<AppState>(context).deviceList?.length ?? 0,
          itemBuilder: (context, index) {
            var _devs = Provider.of<AppState>(context).deviceList;
            return ListTile(
                title: Text("${_devs[index].friendlyName}"),
                subtitle: Text("${_devs[index].urlBase}"),
                onTap: () {
                  Provider.of<AppState>(context).url = _devs[index].urlBase;
                  return Flushbar(
                          message: "Updated url to ${Provider.of<AppState>(context).url}",
                          duration: Duration(seconds: 3))
                      .show(context);
                });
          }));
}
