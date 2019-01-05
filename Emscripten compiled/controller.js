var downloadBlob, downloadURL;

downloadBlob = function(data, fileName, mimeType) {
  var blob, url;
  blob = new Blob([data], {
    type: mimeType
  });
  url = window.URL.createObjectURL(blob);
  downloadURL(url, fileName);
  setTimeout(function() {
    return window.URL.revokeObjectURL(url);
  }, 1000);
};

downloadURL = function(data, fileName) {
  var a;
  a = document.createElement('a');
  a.href = data;
  a.download = fileName;
  document.body.appendChild(a);
  a.style = 'display: none';
  a.click();
  a.remove();
};
needsfocus = false;
function scanfile() {
  var file = document.getElementById("fileForUpload").files[0];
  value = document.getElementById("textcolor").value;
  value = value.match(/[A-Za-z0-9]{2}/g);
  value = value.map(function(v) { return parseInt(v, 16) });
  if (file) {
      var reader = new FileReader();
      reader.readAsText(file, "UTF-8");
      reader.onload = function (evt) {
          var enc = new TextEncoder("ascii");
          var data = enc.encode(evt.target.result);
          var stream = FS.open('file.lrc', 'w+');
          FS.write(stream, data, 0, data.length, 0);
          FS.close(stream);
          Module.ccall('convlrc2cdgjs','void',['string','string','number','number','number'],['file.lrc','file.cdg',value[0],value[1],value[2]]);
          var stream = FS.open('file.cdg', 'r');
          var size = FS.stat('file.cdg').size;
          var buf = new Uint8Array(size);
          FS.read(stream, buf, 0, size, 0);
          FS.close(stream);
          downloadBlob(buf, file.name.split('.')[0] + '.cdg', 'application/octet-stream');
          document.getElementById("fileForUpload").style.display = "none";
          document.getElementById("textcolor").style.display = "none";
          needsfocus = true;
      }
      reader.onerror = function (evt) {
          console.log("error reading file");
      }
  }
}
function reload() {
  window.location = window.location.href;
}
// Set up event handler to produce text for the window focus event
window.addEventListener("focus", function(event)
{
    if(needsfocus) {
      document.getElementsByTagName("button")[0].style.display = "inline";
      document.body.classList.add("done");
    }
}, false);
