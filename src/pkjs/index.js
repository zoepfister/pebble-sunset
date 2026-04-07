var Clay = require('@rebble/clay');
var clayConfig = require('./config');
var clay = new Clay(clayConfig, null, { autoHandleEvents: false });

var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};

function getManualCoords() {
  try {
    var settings = JSON.parse(localStorage.getItem('clay-settings') || '{}');
    var lat = parseFloat(settings.LAT);
    var lon = parseFloat(settings.LON);
    if (!isNaN(lat) && !isNaN(lon)) {
      return { latitude: lat, longitude: lon };
    }
  } catch (e) {}
  return null;
}

function fetchSunData(lat, lon) {
  var url = 'https://api.open-meteo.com/v1/forecast?' +
      'latitude=' + lat +
      '&longitude=' + lon +
      '&daily=sunrise,sunset' +
      '&timezone=auto' +
      '&forecast_days=1';

  xhrRequest(url, 'GET', function(responseText) {
    var json = JSON.parse(responseText);
    var sunriseStr = json.daily.sunrise[0];
    var sunsetStr = json.daily.sunset[0];

    var riseParts = sunriseStr.split('T')[1].split(':');
    var setParts = sunsetStr.split('T')[1].split(':');

    var dictionary = {
      'SUNRISE_HOUR': parseInt(riseParts[0], 10),
      'SUNRISE_MIN':  parseInt(riseParts[1], 10),
      'SUNSET_HOUR':  parseInt(setParts[0], 10),
      'SUNSET_MIN':   parseInt(setParts[1], 10)
    };

    Pebble.sendAppMessage(dictionary,
      function() { console.log('Sun data sent successfully'); },
      function() { console.log('Error sending sun data'); }
    );
  });
}

function locationSuccess(pos) {
  fetchSunData(pos.coords.latitude, pos.coords.longitude);
}

function locationError(err) {
  console.log('Error requesting location: ' + err.message);
}

function getSunData() {
  var coords = getManualCoords();
  if (coords) {
    console.log('Using manual coordinates: ' + coords.latitude + ', ' + coords.longitude);
    fetchSunData(coords.latitude, coords.longitude);
  } else {
    navigator.geolocation.getCurrentPosition(
      locationSuccess,
      locationError,
      { timeout: 15000, maximumAge: 300000 }
    );
  }
}

Pebble.addEventListener('showConfiguration', function() {
  Pebble.openURL(clay.generateUrl());
});

Pebble.addEventListener('webviewclosed', function(e) {
  if (e && !e.response) return;
  var dict = clay.getSettings(e.response);

  Pebble.sendAppMessage(dict,
    function() {
      console.log('Settings saved');
      getSunData();
    },
    function() {
      console.log('Error sending settings');
    }
  );
});

Pebble.addEventListener('ready', function() {
  console.log('PebbleKit JS ready');
  getSunData();
});

Pebble.addEventListener('appmessage', function(e) {
  console.log('AppMessage received');
  if (e.payload['REQUEST_SUN_DATA']) {
    getSunData();
  }
});
