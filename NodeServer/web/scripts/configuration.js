let stations = document.querySelectorAll(".station");

for (const station of stations) {
    // Remove all old options
    while (station.firstChild) {
        station.firstChild.remove();
    }
}

/*
const Http = new XMLHttpRequest();
const url = "/get-liquids";
Http.open("GET", url);
Http.send();

Http.onreadystatechange = (e) => {
    console.log(JSON.parse(Http.responseText));
};
*/
fetch("/get-liquids")
    .then((response) => response.json())
    .then((data) => {
        const liquids = JSON.parse(data).sort();
        for (const station of stations) {
            let option = document.createElement("option");
            option.textContent = "";
            //option.value = "leer";
            station.append(option);

            for(const liquid of liquids){
                let option = document.createElement("option");
                option.textContent = liquid;
                station.append(option);
            }
        }
    });
