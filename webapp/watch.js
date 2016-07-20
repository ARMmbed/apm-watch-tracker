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
var deviceList = require('./device-list');

var fakeData;
if (process.argv.indexOf('--fake-data') > -1) {
  console.log('Starting in fake-data mode');
  fakeData = require('./fake-data')(deviceList);
}

hbs.registerPartials(__dirname + '/views/partials');
app.use(express.static(__dirname + '/public'));
app.set('view engine', 'html');
app.set('views', __dirname + '/views');
app.engine('html', hbs.__express);
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: false }));

function mapToView(d) {
  var friendlyName;
  switch (d.endpoint) {
    case '78f44e29-b690-4cad-8b45-b841c85c6c45':
      friendlyName = 'Demo booth';
      break;
    case 'dd190f5d-99f7-4fae-8d2b-5d1d491da682':
      friendlyName = 'Meeting room #1';
      break;
    default:
      friendlyName = d.endpoint;
      break;
  }

  return {
    endpoint: d.endpoint,
    friendlyName: friendlyName
  };
}

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
  mapToView: d => d,
  verbose: process.argv.indexOf('-v') > -1,
  dontUpdate: false,
  fakeData: fakeData && fakeData.data
};

konekuta(options, (err, gateways, ee, connector) => {
  if (err) {
    throw err;
  }

  server.listen(process.env.PORT || 8219, process.env.HOST || '0.0.0.0', function() {
    console.log('Web server listening on port %s!', process.env.PORT || 8219);

    fakeData && fakeData.startSendingEvents(connector);
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
      namesJson: JSON.stringify(deviceList),
      gatewayJson: JSON.stringify(gateways.map(gw => gw.endpoint)),
      gateways: gateways.map(mapToView)
    });
  }));
});
