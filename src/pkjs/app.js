var Clay = require('pebble-clay');
var clayConfig = require('./config');
var clay = new Clay(clayConfig);

var messageKeys = require('message_keys');

Pebble.addEventListener('webviewclosed', function(e) {
  // Get the keys and values from each config item
  var claySettings = clay.getSettings(e.response);

  // In this example messageKeys.NAME is equal to 10001
  console.log('Name is ' + claySettings[messageKeys.forecastio]); // Logs: "Name is Jane Doe"
});