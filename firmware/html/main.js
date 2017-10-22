var MINI = require("minified"); 
var _=MINI._, $=MINI.$, $$=MINI.$$, EE=MINI.EE, HTML=MINI.HTML;

// OnLoad
document.addEventListener("DOMContentLoaded", init_weather, false);


// Initialize data refresh
function init_weather()
 {
  refresh_weather();
  setInterval(refresh_weather, 15000);
 }


// AJAX request
function refresh_weather()
 {
  $.request("get", "sensors.cgi")
   .then(function(txt) {
    var json = $.parseJSON(txt);
    set_values(json.temperature, json.humidity, json.barometer, json.battery, json.rssi);
   })
   .error(function(status, responseText, xhr) {
    error_ui("Could not load sensor data!", xhr);
    set_values("", "", "", "");
   });
 }


// Set values
function set_values( temperature, humidity, barometer, battery, rssi )
 {
  if(temperature=="") temperature = "--.-";
  if(humidity=="") humidity = "--.-";
  if(barometer=="") barometer = "----";
  if(battery=="") battery = "-.---";
  if(rssi=="") rssi = "---";
  $("#temperature").fill(HTML(temperature +" &deg;C"));
  $("#humidity").fill(HTML(humidity +" %RH"));
  $("#barometer").fill(HTML(barometer +" hPa"));
  $("#battery").fill(HTML(battery +" V"));
  $("#rssi").fill(HTML(rssi +" dBm"));
 }
