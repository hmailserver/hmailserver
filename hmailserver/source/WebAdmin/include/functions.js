function getHTTPObject(){
   if (window.ActiveXObject) 
      return new ActiveXObject("Microsoft.XMLHTTP");
   else if (window.XMLHttpRequest) 
      return new XMLHttpRequest();
   else
      alert("This action requires WebBrowser Ajax-support.");
}
 
