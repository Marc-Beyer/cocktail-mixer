const express = require("express");
const SerialPort = require("serialport");
const Readline = require("@serialport/parser-readline");
const path = require("path");
const fs = require("fs");
const { isGeneratorFunction } = require("util/types");

const app = express();
const port = 4242;

const arduinoSerielPort = new SerialPort("COM3", { baudRate: 115200 });
const parser = arduinoSerielPort.pipe(new Readline({ delimiter: "\n" }));

let cocktails = [];
let station = ["", "", "Wasser", ""];

arduinoSerielPort.on("open", () => {
    console.log("serial port open");
});

parser.on("data", (data) => {
    console.log("got word from arduino:", data);
});

/*
    ENDPOINTS
*/
app.get("/send", (req, res)=>{
    const cocktailName = req.query.cocktail;
    let cocktail = cocktails.find((element) => {
        return element.name == cocktailName;
    });

    if(cocktail){

    }
});

app.get("/configuration", (req, res) => {
    station[0] = req.query?.s1;
    station[1] = req.query?.s2;
    station[2] = req.query?.s3;
    station[3] = req.query?.s4;

    console.log(req.query);

    let cocktailSrcFile = fs.readFileSync("./files/cocktails.json");
    cocktails = JSON.parse(cocktailSrcFile).filter((elem)=>{
        return elem.liquids.every((element) => {
            console.log(element.name, station);
            return station.includes(element.name);
        });
    });

    let str = "<select>";
    for (const cocktail of cocktails) {
        str += `<option>${cocktail.name}</option>`;
    }
    str += "</select>";
    res.send(`<p>${cocktails.length}</p>${str}`);
});


app.get("/get-cocktails", (req, res) => {
    res.send(JSON.stringify(cocktails));
});

app.get("/", (req, res) => {
    if(station){
        res.sendFile(path.join(__dirname, "/web/index.html"));
    }else{
        res.sendFile(path.join(__dirname, "/web/index.html"));
    }
});

app.get("/", () => {
    let name = req.query?.name;
    isGeneratorFunction(name){
        arduinoSerielPort.write(name, function(err) {
            if (err) {
                console.log(err.message);
            }
        });
    }
});

// Start webserver
app.listen(port, () => {
    console.log(`Listening at http://localhost:${port}`);
});
