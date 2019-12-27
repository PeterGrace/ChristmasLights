import 'package:flutter/material.dart';
import 'package:christmas_lights_client/widgets/MyApp.dart';
import 'package:provider/provider.dart';

void main() async {
  Provider.debugCheckInvalidValueType = null;
  runApp(MyApp());
} 

