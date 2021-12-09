const express = require("express");
const SerialPort = require("serialport");
const Readline = require("@serialport/parser-readline");
const path = require("path");
const fs = require("fs");

const app = express();
const port = 4242;

const arduinoSerielPort = new SerialPort("COM3", { baudRate: 115200 });
const parser = arduinoSerielPort.pipe(new Readline({ delimiter: "\n" }));

let station1 = "";
let station2 = "";
let station3 = "";
let station4 = "";

let cockteils = [];

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
    station1 = req.query.station1;
    station2 = req.query.station2;
    station3 = req.query.station3;
    station4 = req.query.station4;

    let cockteilSrcFile = fs.readFileSync("./files/cocktails.json");
    let cockteils = JSON.parse(cockteilSrcFile);
});

app.get("/", (req, res) => {
    res.sendFile(path.join(__dirname, "/web/index.html"));
});

// Start webserver
app.listen(port, () => {
    console.log(`Listening at http://localhost:${port}`);
});
