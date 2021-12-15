path = require('path');

exports.setEndpoints = function (app) {
    app.get("/style/main.css", (req, res) => {
        res.sendFile(path.join(__dirname, "../web/style/main.css"));
    });

    app.get("/scripts/configuration.js", (req, res) => {
        res.sendFile(path.join(__dirname, "../web/scripts/configuration.js"));
    });

    app.get("/scripts/main.js", (req, res) => {
        res.sendFile(path.join(__dirname, "../web/scripts/main.js"));
    });
    
    app.get("/scripts/addCocktail.js", (req, res) => {
        res.sendFile(path.join(__dirname, "../web/scripts/addCocktail.js"));
    });
};
