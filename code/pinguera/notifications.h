#pragma once 

#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <UrlEncode.h>

void send_wsp(String message, const String& phone, const String& key);