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

function createSmartSelect() {
    let input = document.createElement("input");
    let optionsBtn = document.createElement("button");

    let options = document.createElement("div");

    for (const option of ["Test1", "Test2", "Test3"]) {
        let elem = document.createElement("button");
        elem.innerText = option;
    }
}

fetch("/get-liquids")
    .then((response) => response.json())
    .then((data) => {
        const liquids = JSON.parse(data).sort();
        for (const station of stations) {
            let option = document.createElement("option");
            option.textContent = "";
            //option.value = "leer";
            station.append(option);

            for (const liquid of liquids) {
                let option = document.createElement("option");
                option.textContent = liquid;
                station.append(option);
            }
        }
    });
