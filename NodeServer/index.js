const express = require("express");
const SerialPort = require("serialport");
const Readline = require("@serialport/parser-readline");
const path = require("path");

const app = express();
const port = 4242;

const arduinoSerielPort = new SerialPort("COM3", { baudRate: 115200 });
const parser = arduinoSerielPort.pipe(new Readline({ delimiter: "\n" }));

arduinoSerielPort.on("open", () => {
    console.log("serial port open");
});
    
parser.on("data", (data) => {
    console.log("got word from arduino:", data);
});

/*
    ENDPOINTS
*/

app.get("/configuration", (req, res) => {
    const station1 = req.query.station1;
    const station2 = req.query.station2;
    const station3 = req.query.station3;
    const station4 = req.query.station4;


});

app.get("/", (req, res) => {
    res.sendFile(path.join(__dirname, "/web/index.html"));
});

// Start webserver
app.listen(port, () => {
    console.log(`Listening at http://localhost:${port}`);
});
