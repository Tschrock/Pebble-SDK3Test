Pebble.addEventListener('ready',
  function(e) {
    console.log('JavaScript app ready and running!');
    if(Pebble.getActiveWatchInfo) {
      var watch = Pebble.getActiveWatchInfo();
      console.log('platform: ' + watch.platform);
      console.log('model: ' + watch.model);
      console.log('language: ' + watch.language);
      console.log('firmware: ' + watch.firmware.major + "." + watch.firmware.minor + "." + watch.firmware.patch + " " + watch.firmware.suffix);
    } else {
      console.log('Can\'t get Watch Info!');
    }
  }
);

Pebble.addEventListener('appmessage',
  function(e) {
    console.log('Received message: ' + JSON.stringify(e.payload));
  }
);

Pebble.addEventListener('showConfiguration', function(e) {
  // Show config page
  Pebble.openURL('http://tschrock.net/Pebble/api/1f6ed4e1-176f-4b2b-acc5-3cbe54e4c55b/config.php');
});

Pebble.addEventListener('webviewclosed',
  function(e) {
    console.log('Configuration window returned: ' + e.response);
  }
);