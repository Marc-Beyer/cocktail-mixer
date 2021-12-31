const motorPath = require("./modules/motorPath");
const resources = require("./modules/resources");
const redirect = require("./modules/redirect");

const express = require("express");
const SerialPort = require("serialport");
const Readline = require("@serialport/parser-readline");
const path = require("path");
const fs = require("fs");
const bodyParser = require("body-parser");

const app = express();
const port = 4242;


let curPathPos = 0;
let curPath = [];

app.use(bodyParser.json());
app.use(
    bodyParser.urlencoded({
        extended: true,
    })
);

const arduinoSerielPort = new SerialPort("COM6", { baudRate: 115200 });
const parser = arduinoSerielPort.pipe(new Readline({ delimiter: "\n" }));

try {
    arduinoSerielPort.on("open", () => {
        console.log("serial port open");
    });

    parser.on("data", (data) => {
        console.log("Data from Arduino:", data);

        if(data.startsWith("Choose a cocktail")){
            console.log("Send to Arduino: 42");
            arduinoSerielPort.write("42", function (err) {
                if (err) {
                    console.log(err.message);
                }
            });

            return;
        }

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

app.get("/configuration", (req, res) => {
    res.sendFile(path.join(__dirname, "/web/configuration.html"));
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

    res.send(`
        <script type="text/javascript">
            window.location.href = "/";
        </script>
    `);
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
// redirect.setEndpoints(app);

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
resources.setEndpoints(app);

app.get("/add-cocktail", (req, res) => {
    res.sendFile(path.join(__dirname, "/web/add-cocktail.html"));
});

app.post("/add-cocktail", (req, res) => {
    let cocktail = {};
    cocktail.name = req.body?.name;
    let liquidCount = (Object.keys(req.body).length - 1) / 2;

    console.log(req.body);
    console.log("liquidCount", liquidCount);

    cocktail.liquids = [];

    for (let index = 0; index < liquidCount; index++) {
        let liquid = {};
        liquid.name = req.body["lname" + index];
        liquid.amount = req.body["lamount" + index];
        cocktail.liquids.push(liquid);

        if(!liquids.includes(liquid.name)){
            liquids = liquids.replace("]", ',"' + liquid.name + '"]');
            fs.writeFileSync("./files/liquids.json", liquids);
        }
    }

    let cocktailSrcFile = fs.readFileSync("./files/cocktails.json", "utf8");
    let oldCocktails = JSON.parse(cocktailSrcFile);
    oldCocktails.push(cocktail);
    console.log(oldCocktails);

    fs.writeFileSync("./files/cocktails.json", JSON.stringify(oldCocktails));
    
    cocktails = oldCocktails.filter((elem) => {
        return elem.liquids.every((element) => {
            console.log(element.name, station);
            return station.includes(element.name);
        });
    });
    
    res.send(`
        <script type="text/javascript">
            window.location.href = "/";
        </script>
    `);
});

/**
 * Start webserver
 */
app.listen(port, () => {
    console.log(`Listening at http://localhost:${port}`);
});
