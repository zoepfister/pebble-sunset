var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};

function locationSuccess(pos) {
  var url = 'https://api.open-meteo.com/v1/forecast?' +
      'latitude=' + pos.coords.latitude +
      '&longitude=' + pos.coords.longitude +
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
      function(e) { console.log('Sun data sent successfully'); },
      function(e) { console.log('Error sending sun data'); }
    );
  });
}

function locationError(err) {
  console.log('Error requesting location: ' + err.message);
}

function getSunData() {
  navigator.geolocation.getCurrentPosition(
    locationSuccess,
    locationError,
    { timeout: 15000, maximumAge: 300000 }
  );
}

Pebble.addEventListener('ready', function(e) {
  console.log('PebbleKit JS ready');
  getSunData();
});

Pebble.addEventListener('appmessage', function(e) {
  console.log('AppMessage received');
  if (e.payload['REQUEST_SUN_DATA']) {
    getSunData();
  }
});
