var mqtt = require('mqtt')
var writeFile = require('write');
var client  = mqtt.connect({host:'localhost', port:1883})

var out = "count,delReceiver,delListener\n";
client.on('connect', function () {
  client.subscribe('report', function (err) {
  })
  client.subscribe('ping', function (err) {
  })
})
var pingTime, pingName;
client.on('message', function (topic, message) {
  // message is Buffer
  if(topic == "ping"){
    console.log("ping received");
    pingTime = process.hrtime.bigint();
    pingName = message.toString().split(':')[1];
  }
  if(topic == "report"){
    if(pingName == message.toString().split(':')[1].split('-')[0]){
      pingNum = message.toString().split('-')[1];
      diffNum = (process.hrtime.bigint() - pingTime).toString();
      diffNum = (parseInt(diffNum, 10)/1000).toString();
      out += pingName + "," + numberWithCommas(pingNum) + "," +  numberWithCommas(diffNum.split('.')[0]) + "\n";
      console.log(out);
      writeFile.sync('foo.csv', out);
    }
  }
  //client.end()
})
function numberWithCommas(x) {
  return x.replace(/\B(?=(\d{3})+(?!\d))/g, ",");
}