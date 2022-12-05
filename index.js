const express = require('express');
const bodyParser = require('body-parser');
const gpiop = require('rpi-gpio').promise;
const app = express();

app.use(bodyParser.json());
app.post('/switchLed', function(req,res) {
    const statusExists = rq && req.body && req.body.status != null;
});
