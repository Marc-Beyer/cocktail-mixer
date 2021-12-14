const motorPath = require("./motorPath");

const express = require("express");
const SerialPort = require("serialport");
const Readline = require("@serialport/parser-readline");
const path = require("path");
const fs = require("fs");
const bodyParser = require("body-parser");

const app = express();
const port = 4242;

const arduinoSerielPort = new SerialPort("COM3", { baudRate: 115200 });
const parser = arduinoSerielPort.pipe(new Readline({ delimiter: "\n" }));

let curPathPos = 0;
let curPath = [];

app.use(bodyParser.json());
app.use(
    bodyParser.urlencoded({
        extended: true,
    })
);

try {
    arduinoSerielPort.on("open", () => {
        console.log("serial port open");
    });

    parser.on("data", (data) => {
        console.log("Data from Arduino:", data);

        curPathPos++;

        if (curPath?.length > curPathPos) {
            console.log("Send to Arduino:", curPath[curPathPos]);
            arduinoSerielPort.write(curPath[curPathPos], function (err) {
                if (err) {
                    console.log(err.message);
                }
            });
        }
    });
} catch (error) {
    console.log("TEST");
}

let liquids = fs.readFileSync("./files/liquids.json", "utf8");
let cocktails = [];
let station = []; // ["", "", "Wasser", ""];

console.log(liquids);

/**
 * ENDPOINTS
 */
app.get("/", (req, res) => {
    if (station && station.length > 0) {
        res.sendFile(path.join(__dirname, "/web/index.html"));
    } else {
        res.sendFile(path.join(__dirname, "/web/configuration.html"));
    }
});

app.post("/configuration", (req, res) => {
    station[0] = req.body?.s1;
    station[1] = req.body?.s2;
    station[2] = req.body?.s3;
    station[3] = req.body?.s4;

    console.log(req.body);

    let cocktailSrcFile = fs.readFileSync("./files/cocktails.json", "utf8");
    cocktails = JSON.parse(cocktailSrcFile).filter((elem) => {
        return elem.liquids.every((element) => {
            console.log(element.name, station);
            return station.includes(element.name);
        });
    });

    console.log(cocktails);

    res.sendFile(path.join(__dirname, "/web/index.html"));
});

app.get("/request-cocktail", (req, res) => {
    let name = req.query?.name;
    console.log("Cocktail:", name);

    let cocktail = cocktails.find((element) => {
        return element.name == name;
    });

    if (cocktail) {
        // Send data to arduino
        let path = motorPath.getPathFromCocktail(station, cocktail);
        console.log("path", path);
        curPath = path;
        curPathPos = 0;

        if (path?.length > 0) {
            console.log("Send to Arduino:", path[curPathPos]);
            arduinoSerielPort.write(path[0], function (err) {
                if (err) {
                    console.log(err.message);
                }
            });
        }

        res.send(`
        <script type="text/javascript">
            function sleep(ms) {
                return new Promise(resolve => setTimeout(resolve, ms));
            }

            (async function demo() {
                await sleep(5000);
                window.location.href = "/";
            })();
        </script>
        <h1>Cocktail wird vorbereitet...</h1>
        `);
    } else {
        res.send(`
        <h1>Cocktail "${name}" nicht gefunden!</h1>
        <a href="/">zurück</a>
        `);
    }
});

/**
 * Redirects
 */
app.get("/configuration", (req, res) => {
    res.send(`
        <script type="text/javascript">
            window.location.href = "/";
        </script>
        `);
});

/**
 * Data endpoints
 */
app.get("/get-cocktails", (req, res) => {
    res.send(JSON.stringify(cocktails));
});

app.get("/get-liquids", (req, res) => {
    res.send(JSON.stringify(liquids));
});

/**
 * Resources
 */
app.get("/style/main.css", (req, res) => {
    res.sendFile(path.join(__dirname, "/web/style/main.css"));
});

app.get("/scripts/configuration.js", (req, res) => {
    res.sendFile(path.join(__dirname, "/web/scripts/configuration.js"));
});

app.get("/scripts/main.js", (req, res) => {
    res.sendFile(path.join(__dirname, "/web/scripts/main.js"));
});

/**
 * Start webserver
 */
app.listen(port, () => {
    console.log(`Listening at http://localhost:${port}`);
});
