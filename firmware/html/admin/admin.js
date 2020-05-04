var MINI = require("minified");
var _=MINI._, $=MINI.$, $$=MINI.$$, EE=MINI.EE, HTML=MINI.HTML;
var wifi_mode = 0;
var services = null;
var wifi_dhcp = 1;
var http_enabled = 0;
var mqtt_enabled = 0;
var user_loaded = 0;
var wifi_loaded = 0;
var sensor_loaded = 0;

// OnLoad
document.addEventListener("DOMContentLoaded", init_admin, false);


// Initialize
function init_admin()
 {
  showtab(0);
  $("#btn-network").onClick(function() {showtab(0)});
  $("#btn-sensor").onClick(function() {showtab(1)});
  $("#btn-user").onClick(function() {showtab(2)});
  $("#btn-system").onClick(function() {showtab(3)});
  $("#networks-toggle").onClick(function() {toggle_wifi_manual()});
  $("#btn-save").onClick(function() {save_settings(0)});
  $("#wifi-dhcp").onChange(function() {change_wifi_settings()});
  $("#http-enabled").onChange(function() {change_sensor_settings()});
  $("#mqtt-enabled").onChange(function() {change_sensor_settings()});
 }


// Show div for menu 
function showtab( id )
 {
  if(id==0)
   {
    if($("#tab-network").get("$display")=="none")
     {
      load_wifi_settings(function(){
       if(id!=0) return;
       $("#tab-network").show().set({$$fade: 0}).animate({$$fade: 1}, 500);
       refresh_button(1);
       refresh_wifi();
      });
     }
   } else $("#tab-network").hide();
  if(id==1)
   {
    if($("#tab-sensor").get("$display")=="none")
     {
      load_sensor_services(function(){
       load_sensor_settings(function(){
        if(id!=1) return;
        $("#tab-sensor").show().set({$$fade: 0}).animate({$$fade: 1}, 500);
        refresh_button(1);
       });
      });
     }
   } else $("#tab-sensor").hide();
  if(id==2)
   {
    if($("#tab-user").get("$display")=="none")
     {
      load_user_settings(function(){
       if(id!=2) return;
       $("#tab-user").show().set({$$fade: 0}).animate({$$fade: 1}, 500);
       refresh_button(1);
      });
     }
   } else $("#tab-user").hide();
  if(id==3)
   {
    if($("#tab-system").get("$display")=="none")
     {
      load_about_page(function(){
       if(id!=3) return;
       $("#tab-system").show().set({$$fade: 0}).animate({$$fade: 1}, 500);
       refresh_button(0);
      });
     }
   } else $("#tab-system").hide();
 }


// Show or hide safe button
function refresh_button( visible )
 {
  var status = $("#main-buttons").get("$display")=="none"?0:1;
  if(status==visible) return;
  if(visible==0)
   {
    $("#main-buttons").hide();
   } else {
    $("#main-buttons").show().set({$$fade: 0}).animate({$$fade: 1}, 500);
   }
 }


// Fetch WIFI settings
function load_wifi_settings( callback )
 {
  block_ui(1);
  $.request("get", "wifisettings.cgi?action=load")
   .then(function(txt) {
    block_ui(0);
    var json = $.parseJSON(txt);
    wifi_mode = json.wifi_mode;
    if(wifi_mode!=3)
     {
      $("#networks-table").fill(EE("p", "Scanning disabled in station mode."));
     }
    $("#wifi-essid").set("value", json.wifi_essid);
    $("#wifi-pass").set("value", json.wifi_pass);
    wifi_dhcp = json.wifi_dhcp;
    $("#wifi-ip").set("value", json.wifi_ip);
    $("#wifi-mask").set("value", json.wifi_mask);
    $("#wifi-gw").set("value", json.wifi_gw);
    render_wifi_settings();
    wifi_loaded = 1;
    callback();
   })
   .error(function(status, responseText, xhr) {
    block_ui(0);
    $("#networks-table").fill(EE("p", "Could not load network settings."));
    error_ui("Could not load network settings!", xhr);
    callback();
   });
 }


// Show or hide ip settings
function render_wifi_settings()
 {
  $("#wifi-dhcp").set('selectedIndex', wifi_dhcp);
  if(wifi_dhcp==0)
   {
    $("#wifi-div").show();
   } else {
    $("#wifi-div").hide();
   }
 }


// DHCP dropdown - item changed
function change_wifi_settings()
 {
  wifi_dhcp = $("#wifi-dhcp").get("value");
  render_wifi_settings();
 }


// Toggle manual WiFi SSID settings
function toggle_wifi_manual()
 {
  if($("#networks").get("$display")=="none")
   {
    $("#networks").show();
    $("#networks-manual").hide();
   } else {
    $("#networks").hide();
    $("#networks-manual").show();
   }
 }


// Save modified WIFI settings
function save_wifi_settings( callback )
 {
  var essid = $("#wifi-essid").get("value");
  var pass = $("#wifi-pass").get("value");
  var dhcp = $("#wifi-dhcp").get("value");
  var ip = $("#wifi-ip").get("value");
  var mask = $("#wifi-mask").get("value");
  var gw = $("#wifi-gw").get("value");
  block_ui(1);
  $.request("post", "wifisettings.cgi?action=save", {"wifi_essid": essid, "wifi_pass": pass, "wifi_dhcp": dhcp, "wifi_ip": ip, "wifi_mask": mask, "wifi_gw": gw})
   .then(function(txt) {
    block_ui(0);
    var json = $.parseJSON(txt);
    if(json.status=="success")
     {
      callback();
     } else {
      error_ui("Could not save wifi settings!", null);
     }
   })
   .error(function(status, responseText, xhr) {
    block_ui(0);
    error_ui("Could not save wifi settings!", xhr);
   });
 }


// Scan for WIFI networks in range
function refresh_wifi()
 {
  if(wifi_mode!=3) return;
  $.request("get", "wifiscan.cgi")
   .then(function(txt) {
    var json = $.parseJSON(txt);
    if(json.inProgress=="0" && json.APs.length>0)
     {
      $("#networks-table").fill();
      json.APs.sort(function(a, b) {return b.rssi-a.rssi});
      for (var i=0; i<json.APs.length; i++)
       {
        if(json.APs[i].essid=="" && json.APs[i].rssi=="0") continue;
        ap_node(json.APs[i], i, $("#networks-table"));
       }
      $(".essid").on("change", function() {
       $("#wifi-essid").set("value", this.get("value"));
      });
      window.setTimeout(refresh_wifi, 30000);
     } else {
      window.setTimeout(refresh_wifi, 2000);
     }
   })
   .error(function(status, responseText, xhr) {
    $("#networks-table").fill(EE("p", "Could not get network list."));
   });
 }


// Render single AP node for WIFI list
function ap_node(ap_data, ap_id, networks_table)
 {
  var essid = $("#wifi-essid").get("value");
  var network_node = EE("div", {"@class": "network-node"});
  var ap = EE("div");
  var radio = EE("input", {"@name": "essid", "@class": "essid", "@id": "ap-"+ap_id, "@type": "radio", "@value": ap_data.essid});
  if(ap_data.essid==essid) radio.set("@checked", "true");
  ap.add(radio);
  ap.add(EE("label", {"@for": "ap-"+ap_id}, ap_data.essid));
  network_node.add(EE("div", ap));
  var rssi = rssi_to_signal(parseInt(ap_data.rssi, 10))*-32;
  var icon = EE("div", {"@class": "network-icon", "@style": "background-position: 0px "+rssi+"px"});
  var enc = -80; // WPA/WPA2
  if(ap_data.enc==1) enc = -48; // WEP 
  if(ap_data.enc==0) enc = -16; // Open
  icon.add(EE("div", {"@class": "enc-icon", "@style": "background-position: -32px "+enc+"px"}));
  network_node.add(EE("div", icon));
  networks_table.add(network_node);
 }


// Convert dBm to value of 0 (low) to 4 (high) signal
function rssi_to_signal(rssi)
 {
  if(rssi>=-50) rssi = -50; // -50dBm max
  if(rssi<=-100) rssi = -100; // -100dBm min
  return Math.floor((rssi+100)/50*4);
 }


// Fetch available push services
function load_sensor_services( callback )
 {
  if(services==null)
   {
    block_ui(1);
    $.request("get", "services.json")
     .then(function(txt) {
      block_ui(0);
      services = $.parseJSON(txt);
      callback();
     })
     .error(function(status, responseText, xhr) {
      block_ui(0);
      error_ui("Could not load services!", xhr);
      callback();
     });
   } else {
    callback();
   }
 }


// Show push services in UI
function render_sensor_services()
 {
  if(services==null) return;
  $("#http-enabled").fill();
  _.each(services.http_services, function(item, index) {
   var option = EE("option", {"@value": item.id}, item.name);
   if(item.id==http_enabled)
    {
     option.set("@selected", "true");
     if(item.key_label!="")
      {
       $("#http-key-label").fill(item.key_label);
       $("#http-key-div").show();
      } else $("#http-key-div").hide();
     if(item.grp_label!="")
      {
       $("#http-grp-label").fill(item.grp_label);
       $("#http-grp-div").show();
      } else $("#http-grp-div").hide();
     if(item.url_visible==true) $("#http-url-div").show(); else $("#http-url-div").hide();
    }
   $("#http-enabled").add(option);
  });
  $("#mqtt-enabled").fill();
  _.each(services.mqtt_services, function(item, index) {
   var option = EE("option", {"@value": item.id}, item.name);
   if(item.id==mqtt_enabled)
    {
     option.set("@selected", "true");
     if(services.mqtt_default_ports[index]!=""&&services.mqtt_default_ports.indexOf($("#mqtt-port").get("value"))>0)
      {
       $("#mqtt-port").set("value", services.mqtt_default_ports[index]);
      }
     if(item.settings_visible==true) $("#mqtt-div").show(); else $("#mqtt-div").hide();
    }
   $("#mqtt-enabled").add(option);
  });
 }


// Service dropdown - item changed
function change_sensor_settings()
 {
  http_enabled = $("#http-enabled").get("value");
  mqtt_enabled = $("#mqtt-enabled").get("value");
  render_sensor_services();
 }


// Fetch sensor settings
function load_sensor_settings( callback )
 {
  block_ui(1);
  $.request("get", "sensorconfig.cgi?action=load")
   .then(function(txt) {
    block_ui(0);
    var json = $.parseJSON(txt);
    $("#sensor-interval").set("value", json.sensor_interval);
    http_enabled = json.http_enabled;
    $("#http-key").set("value", json.http_key);
    $("#http-grp").set("value", json.http_grp);
    $("#http-url").set("value", json.http_url);
    mqtt_enabled = json.mqtt_enabled;
    $("#mqtt-host").set("value", json.mqtt_host);
    $("#mqtt-port").set("value", json.mqtt_port);
    $("#mqtt-user").set("value", json.mqtt_user);
    $("#mqtt-pass").set("value", json.mqtt_pass);
    $("#mqtt-topic").set("value", json.mqtt_topic);
    render_sensor_services();
    sensor_loaded = 1;
    callback();
   })
   .error(function(status, responseText, xhr) {
    block_ui(0);
    error_ui("Could not load sensor settings!", xhr);
    callback();
   });
 }


// Save modified sensor settings
function save_sensor_settings( callback )
 {
  var interval = $("#sensor-interval").get("value");
  var http = $("#http-enabled").get("value");
  var key = $("#http-key").get("value");
  var grp = $("#http-grp").get("value");
  var url = $("#http-url").get("value");
  var mqtt = $("#mqtt-enabled").get("value");
  var host = $("#mqtt-host").get("value");
  var port = $("#mqtt-port").get("value");
  var user = $("#mqtt-user").get("value");
  var pass = $("#mqtt-pass").get("value");
  var topic = $("#mqtt-topic").get("value");
  block_ui(1);
  $.request("post", "sensorconfig.cgi?action=save", {"sensor_interval": interval, "http_enabled": http, "http_key": key, "http_grp": grp, "http_url": url, "mqtt_enabled": mqtt, "mqtt_host": host, "mqtt_port": port, "mqtt_user": user, "mqtt_pass": pass, "mqtt_topic": topic})
   .then(function(txt) {
    block_ui(0);
    var json = $.parseJSON(txt);
    if(json.status=="success")
     {
      callback();
     } else {
      error_ui("Could not save sensor settings!", null);
     }
   })
   .error(function(status, responseText, xhr) {
    block_ui(0);
    error_ui("Could not save sensor settings!", xhr);
   });
 }


// Fetch user settings
function load_user_settings( callback )
 {
  block_ui(1);
  $.request("get", "userconfig.cgi?action=load")
   .then(function(txt) {
    block_ui(0);
    var json = $.parseJSON(txt);
    $("#admin-user").set("value", json.admin_user);
    $("#admin-pass").set("value", json.admin_pass);
    user_loaded = 1;
    callback();
   })
   .error(function(status, responseText, xhr) {
    block_ui(0);
        error_ui("Could not load user settings!", xhr);
    callback();
   });
 }


// Save modified user settings
function save_user_settings( callback )
 {
  var user = $("#admin-user").get("value");
  var pass = $("#admin-pass").get("value");
  block_ui(1);
  $.request("post", "userconfig.cgi?action=save", {"admin_user": user, "admin_pass": pass})
   .then(function(txt) {
    block_ui(0);
    var json = $.parseJSON(txt);
    if(json.status=="success")
     {
      callback();
     } else {
      error_ui("Could not save user settings!", null);
     }
   })
   .error(function(status, responseText, xhr) {
    block_ui(0);
    error_ui("Could not save user settings!", xhr);
   });
 }


// Show build info on about page
function load_about_page( callback )
 {
  block_ui(1);
  $.request("get", "buildinfo.cgi")
   .then(function(txt) {
    block_ui(0);
    $("#tab-system-inner").fill(EE("pre", txt));
    callback();
   })
   .error(function(status, responseText, xhr) {
    block_ui(0);
    $("#tab-system-inner").fill(EE("pre", "Could not load build information."));
    error_ui("Could not load build information!", xhr);
    callback();
   });
 }


// Save settings if item loaded before (if tab had been chosen)
function save_settings( item )
 {
  switch(item)
   {
    case 0:
     if(wifi_loaded!=0)
      {
       save_wifi_settings(function(){save_settings(1)});
       break;
      }
    case 1:
     if(sensor_loaded!=0)
      {
       save_sensor_settings(function(){save_settings(2)});
       break;
      }
    case 2:
     if(user_loaded!=0)
      {
       save_user_settings(function(){save_settings(3)});
       break;
      }
    default:
     message_ui("Settings saved!");
     break;
   }
 }
