import 'package:christmas_lights_client/clfuncs/clfuncs.dart';
import 'package:christmas_lights_client/values/values.dart';
import 'package:flutter/material.dart';

class LightsInteraction extends StatefulWidget {
  _LightsInteractionState createState() => _LightsInteractionState();
}

class _LightsInteractionState extends State<LightsInteraction> {
  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text(appName + " Interact"),
      ),
      body: renderBody(context),
    );
  }

  Widget renderBody(BuildContext context) {
    return Center(
      child: modeList(),
    );
  }

  Widget modeList() {
    List<ListTile> _items = <ListTile>[];
    _items.add(ListTile(
        title: Text("Random"),
        subtitle: Text("Randomly change colors"),
        onTap: () {
          setMode(context,1);
        }));

    _items.add(ListTile(
        title: Text("Confetti"),
        subtitle: Text("Popping colors like confetti"),
        onTap: () {
          setMode(context,3);
        }));
    _items.add(ListTile(
        title: Text("Rainbow Solid"),
        subtitle: Text("Solid Rainbow"),
        onTap: () {
          setMode(context,4);
        }));
    _items.add(ListTile(
        title: Text("Rainbow BPM"),
        subtitle: Text("Rainbow thumping at a speed"),
        onTap: () {
          setMode(context,5);
        }));
    _items.add(ListTile(
        title: Text("'Murica Chase"),
        subtitle: Text("Red,white,and blue confetti"),
        onTap: () {
          setMode(context,6);
        }));
    _items.add(ListTile(
        title: Text("Voobing Red"),
        subtitle: Text("Red flashing in-and-out, like RED ALERT"),
        onTap: () {
          setMode(context,10);
        }));
    _items.add(ListTile(
        title: Text("Voobing Green"),
        subtitle: Text("Green flashing in-and-out"),
        onTap: () {
          setMode(context,20);
        }));
    _items.add(ListTile(
        title: Text("Voobing Blue"),
        subtitle: Text("Blue flashing in-and-out"),
        onTap: () {
          setMode(context,30);
        }));
    _items.add(ListTile(
        title: Text("Voobing Orange"),
        subtitle: Text("Orange flashing in-and-out"),
        onTap: () {
          setMode(context,40);
        }));
    _items.add(ListTile(
        title: Text("Swipe Red/Green"),
        subtitle: Text("Red and green swipe over each other"),
        onTap: () {
          setMode(context,50);
        }));
    _items.add(ListTile(
        title: Text("Swipe Red/Blue"),
        subtitle: Text("Red and blue swipe over each other"),
        onTap: () {
          setMode(context,51);
        }));
    _items.add(ListTile(
        title: Text("Swipe Blue/Green"),
        subtitle: Text("Blue and green swipe over each other"),
        onTap: () {
          setMode(context,52);
        }));
    _items.add(ListTile(
        title: Text("Gradient"),
        subtitle: Text("Color Gradient Mode"),
        onTap: () {
          setMode(context,99);
        }));

    return ListView.builder(
        shrinkWrap: true,
        itemCount: _items.length,
        itemBuilder: (context, index) {
          return _items[index];
        });
  }
}
