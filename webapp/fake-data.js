'use strict';

function generateRssi() {
  return -(Math.random() * 70 + 30) | 0;
}
function generateTemp() {
  return (Math.random() * 1000 + 2000) | 0;
}
function generateFakeEndpoint(endpoint, deviceList, f) {
  var resources = Object.keys(deviceList).reduce((curr, mac, ix) => {
    if (f.indexOf(ix % 3) === -1) return curr;

    curr[mac + '/rssi'] = generateRssi();
    curr[mac + '/temperature'] = generateTemp();
    return curr;
  }, {});
  resources.endpoint = endpoint;
  return resources;
}

module.exports = function(deviceList) {
  // so first we need to create some base values I guess...
  // let's start with single gateway scenario
  // all devices will be present at the start

  var data = [
    generateFakeEndpoint('78f44e29-b690-4cad-8b45-b841c85c6c45', deviceList, [ 0, 1 ]),
    generateFakeEndpoint('dd190f5d-99f7-4fae-8d2b-5d1d491da682', deviceList, [ 2 ]),
  ];

  var startSendingEvents = connector => {
    // so every 5 seconds we'll dump some random data...
    setInterval(() => {
      for (let device of data) {
        for (let resource of Object.keys(device)) {
          if (resource === 'endpoint') continue;

          var mac = resource.split('/')[0];
          if (resource.indexOf('rssi') > -1) {
            connector.emit('notification', {
              ep: device.endpoint,
              path: `/${mac}/0/rssi`,
              payload: generateRssi()
            });
          }
          else if (resource.indexOf('temperature') > -1) {
            connector.emit('notification', {
              ep: device.endpoint,
              path: `/${mac}/0/temperature`,
              payload: generateTemp()
            });
          }
        }
      }
    }, 2000);
  };

  return {
    data: data,
    startSendingEvents: startSendingEvents
  };
};
