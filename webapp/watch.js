'use strict';

var TOKEN = 'YOUR_ACCESS_TOKEN';

var express = require('express');
var bodyParser = require('body-parser');
var konekuta = require('konekuta');
var app = express();
var server = require('http').Server(app);
var io = require('socket.io')(server);
var wrap = require('co-express');
var hbs = require('hbs');

hbs.registerPartials(__dirname + '/views/partials');
app.use(express.static(__dirname + '/public'));
app.set('view engine', 'html');
app.set('views', __dirname + '/views');
app.engine('html', hbs.__express);
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: false }));

function mapToView(d) {
  return d;
}

// Should be same as in gateway-firmware/source/main.cpp !!
var deviceList = {
  'db91f0f56813': 'Jan Jongboom',
  'e1f44f7e25f1': 'Phill Smith',
};

var resources = Object.keys(deviceList).reduce((curr, mac) => {
  curr[mac + '/rssi'] = `/${mac}/0/rssi`;
  curr[mac + '/temperature'] = `/${mac}/0/temperature`;
  return curr;
}, {});

var options = {
  endpointType: 'BLE_Watch_Gateway',
  token: TOKEN,
  io: io,
  subscribe: resources,
  retrieve: resources,
  updates: {},
  mapToView: mapToView,
  verbose: process.argv.indexOf('-v') > -1,
  dontUpdate: false,
  fakeData: null
};

konekuta(options, (err, gateways, ee) => {
  if (err) {
    throw err;
  }

  server.listen(process.env.PORT || 8219, process.env.HOST || '0.0.0.0', function() {
    console.log('Web server listening on port %s!', process.env.PORT || 8219);
  });

  app.get('/', wrap(function*(req, res) {
    var allTemp = gateways.reduce((curr, gw) => {
      Object.keys(gw).forEach(route => {
        if (route.indexOf('/temperature') === -1) return;
        curr[route.split('/')[0]] = gw[route]; // last gateway wins
      });
      return curr;
    }, {});

    // build the view model
    res.render('index', {
      tempJson: JSON.stringify(allTemp),
      namesJson: JSON.stringify(deviceList)
    });
  }));
});
