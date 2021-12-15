exports.setEndpoints = function(app){
    app.get("/configuration", (req, res) => {
        res.send(`
            <script type="text/javascript">
                window.location.href = "/";
            </script>
            `);
    });
}