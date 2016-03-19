var MINI = require("minified");
var _=MINI._, $=MINI.$, $$=MINI.$$, EE=MINI.EE, HTML=MINI.HTML;


// Show or hide wait message and block user interface
function block_ui( block )
 {
  var status = $("#fade").get("$display")=="none"?0:1;
  if(status==block) return;
  if(block==0)
   {
    $("#fade").set({$$fade: 0.75}).animate({$$fade: 0}, 250).then(function(){$("#fade").hide()});
   } else {
    $("#fade").show().set({$$fade: 0}).animate({$$fade: 0.75}, 250);
   }
 }


// Show tooltip, hide after 5 seconds or if clicked
function tooltip_ui( tclass, message )
 {
  var tip = EE("div", {"@class": "tooltip grn-border"});
  tip.fill(EE("p", HTML(message)));
  tip.set("$", "+"+tclass);
  $("#tooltip").add(tip);
  tip.set({$$fade: 0}).animate({$$fade: 1}, 250);
  tip.onClick(function() {tip.hide();});
  setTimeout(function()
   {
    tip.set({$$fade: 1}).animate({$$fade: 0}, 500).then(function(){tip.remove();});
   }, 5000);
 }


// Show error as tooltip
function error_ui( message, xhr )
 {
  if(xhr!=null&&xhr.status!==undefined&&xhr.status!=0)
   {
    message = message + '<br>(Error ' + xhr.status;
    if(xhr.statusText!="") message = message + ' - ' + xhr.statusText;
    message = message + ')';
   }
  tooltip_ui("tooltip_error", message);
 }


// Show info message as tooltip
function message_ui( message )
 {
  tooltip_ui("tooltip_info", message);
 }
