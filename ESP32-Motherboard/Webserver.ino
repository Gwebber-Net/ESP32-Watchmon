#include <ArduinoJson.h>

char webpage[] PROGMEM = R"=====(
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<html>
<head>
<script type="text/javascript" src="/js/nrg.js"></script>
</head>
<body>
<p> Voltage <span id="voltage">__</span> </p>
<button onclick="GetVoltage()"> Get Voltage </button>
<br>
<br>
<p> Result  <span id="result"></span> </p>
<button onclick="PostSettings()"> Send Settings</button>
</body>
</html>
)=====";


char www_js_nrg[] PROGMEM = R"=====(

function ajax(method, url, ctype="", on_success, on_failure, post_data="")
{
  var xhr = new XMLHttpRequest();
  xhr.onload = function()
  {
    if(xhr.status == 200)
    {
        on_success(xhr);
    } else 
    {
        on_fail(xhr);
    }
  };
    xhr.open(method, url);
    
    if ( method == "POST" ) 
    {
        xhr.setRequestHeader('Content-Type', ctype);
        xhr.send(JSON.stringify(post_data));
        console.log(post_data);
    }
    else
    {
      xhr.send();
    }
};
function GetVoltage()
{
  ajax("GET",
       "/voltage", 
       "application/json", 
       // success callback
       function(result) 
       {
            var obj = JSON.parse(result.responseText);
            document.getElementById("voltage").innerHTML = obj.voltage;
            console.log(result.status);
            console.log(result.responseText);
       },
       // error callback
       function(result) 
       {
            console.log(result.status);
            console.log(result.responseText);            
        }
        
  );
};

function PostSettings()
{
    ajax("POST",
       "/settings", 
       "application/json", 
       // success callback
       function(result) 
       {
            //var obj = JSON.parse(result.responseText);
            //document.getElementById("voltage").innerHTML = obj.voltage;
            console.log(result.status);
            console.log(result.responseText);
       },
       // error callback
       function(result) 
       {
            console.log(result.status);
            console.log(result.responseText);            
        },
        {setting: "btime"}
    );
};
)=====";

void ReceiveSettings()
{
  String input = server.arg("plain");
  Serial.println(input);
  
  StaticJsonDocument<200> doc;
  DeserializationError err = deserializeJson(doc, input);
  if (err) 
  {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(err.c_str());
  }
  else
  {
    String s = doc["setting"];
    Serial.println(s);
  }  
}

void SendVoltage()
{

StaticJsonDocument<100> doc;

// create an object
//JsonObject object = doc.to<JsonObject>();
//object["voltage"] = 4.2;
doc["voltage"] = 4.2;
// serialize the object and send the result to Serial
String output;
serializeJson(doc, output);
server.send(200,"application/json",output);
}

void SendPackInfo()
{
  String output = PackInfo();
  server.send(200,"application/json",output);

  
}

void SendSummary()
{
  byte lowestcell = GetLowestCell();
  byte highestcell = GetHighestCell();
  float voltage = (float)24.1;
  float current = (float)3.33;
  StaticJsonDocument<100> doc;
  doc["voltage"] = voltage;
  doc["current"] = current;
  doc["lowestcell"] = lowestcell;
  doc["highestcell"] = highestcell;
  String output;
  serializeJson(doc, output);
  server.send(200,"application/json",output);
}

void InitialiseServer()
{
  server.on("/",[](){
    Serial.println("Service: /index.html");
    server.send_P(200, "text/html", webpage);}
  );
  server.on("/js/nrg.js",[](){
    Serial.println("Service: /js/nrg.js");
    server.send_P(200, "application/x-javascript", www_js_nrg);}
  );
  server.on("/voltage", SendVoltage);
  server.on("/settings", ReceiveSettings);
  server.on("/api/packinfo", SendPackInfo);
  server.on("/api/summary", SendSummary);
  
  server.begin();
}
