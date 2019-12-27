library clfuncs;

import 'dart:convert';

import 'package:christmas_lights_client/values/values.dart';
import 'package:flushbar/flushbar.dart';
import 'package:flutter/material.dart';
import 'package:provider/provider.dart';


void setMode(BuildContext context, int modenum) async {
    var url = Provider.of<AppState>(context).url + "lights";
    var body = json.encode(
          {
      'mode': modenum,
      'brightness': 128
    });
    final response = await www.post(url,data:body);
    if (response.statusCode == 200) {
        Flushbar(
          message: "Sent mode change",
          icon: Icon(
            Icons.check,
            size: 28,
            color: Colors.green),
          duration: Duration(seconds: 3)
        ).show(context);
    } else {
      Flushbar(
        message: "Error when sending code",
          icon: Icon(
            Icons.error,
            size: 28,
            color: Colors.red),
          duration: Duration(seconds: 3)
        ).show(context);
    }
}