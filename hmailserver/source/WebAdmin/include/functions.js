function getHTTPObject(){
   if (window.ActiveXObject) 
      return new ActiveXObject("Microsoft.XMLHTTP");
   else if (window.XMLHttpRequest) 
      return new XMLHttpRequest();
   else
      alert("This action requires WebBrowser Ajax-support.");
}

// Sends the query string parameters in url to index.php as a POST, together with the CSRF token.
function PostLink(url) {
   var form = document.createElement("form");
   form.method = "post";
   form.action = "index.php";

   var parameters = url.substring(url.indexOf("?") + 1).split("&");
   for (var i = 0; i < parameters.length; i++) {
      if (parameters[i] == "")
         continue;

      var separator = parameters[i].indexOf("=");
      if (separator < 0)
         continue;

      AddHiddenField(form, decodeURIComponent(parameters[i].substring(0, separator)), decodeURIComponent(parameters[i].substring(separator + 1)));
   }

   AddHiddenField(form, "csrftoken", csrfToken);

   document.body.appendChild(form);
   form.submit();
}

function AddHiddenField(form, name, value) {
   var field = document.createElement("input");
   field.type = "hidden";
   field.name = name;
   field.value = value;
   form.appendChild(field);
}

// Posts parameters (name/value object) to index.php, together with the CSRF token.
function PostAjax(httpObject, parameters) {
   var body = "csrftoken=" + encodeURIComponent(csrfToken);
   for (var name in parameters)
      body += "&" + encodeURIComponent(name) + "=" + encodeURIComponent(parameters[name]);

   httpObject.open("POST", "index.php", true);
   httpObject.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
   httpObject.send(body);
}
 
