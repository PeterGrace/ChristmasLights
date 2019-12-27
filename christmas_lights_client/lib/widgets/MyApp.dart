import 'package:christmas_lights_client/widgets/LightsInteraction.dart';
import 'package:christmas_lights_client/widgets/SelectZone.dart';
import 'package:flutter/material.dart';
import 'package:christmas_lights_client/values/values.dart';
import 'package:provider/provider.dart';

class MyApp extends StatelessWidget {
  // This widget is the root of your application.
  @override
  Widget build(BuildContext context) {
    return ChangeNotifierProvider<AppState>(
        create: (context) => new AppState(),
        child: MaterialApp(
          title: appName,
          theme: ThemeData(
            primarySwatch: Colors.blue,
          ),
          routes: <String, WidgetBuilder>{
            '/': (BuildContext context) => new SelectZone(),
            '/interact': (BuildContext context) => new LightsInteraction()
          }
        )
        );
  }
}

