const express = require('express');
const bodyParser = require('body-parser');
const gpiop = require('rpi-gpio').promise;
const app = express();

app.use(bodyParser.json());
app.post('/switchLed', function(req,res) {
    const statusExists = rq && req.body && req.body.status != null;
    const status = statusExists ? req.body.status.toString() == "true":false;

    gpiop.setup(8,gpiop.DIR_OUT).then(()=>{
        return gpiop.write(8, status);
    }).catch((err)=>{
        console.log(`error:${err.toString}`)
    });

    res.send('led was switched');
});

app.listen(3000);
console.log('server 3000');
