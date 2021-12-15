const buttonList = document.querySelector(".buttons");
const requestString = requestHTTP("/get-cocktails");

console.log(requestString);

let drinkArray = JSON.parse(requestString);

for (const drink of drinkArray) {
    let li = document.createElement("li");
    let button = document.createElement("button");
    button.textContent = drink.name;

    button.addEventListener("click", (e) => {
        document.location = "./request-cocktail?name=" + drink.name;
    });

    li.appendChild(button);
    buttonList.appendChild(li);
}


buttonList.appendChild(document.createElement("hr"));

// Config Button
let li = document.createElement("li");
let button = document.createElement("button");
button.textContent = "Change Configuration";

button.addEventListener("click", (e) => {
    document.location = "./configuration";
});

li.appendChild(button);
buttonList.appendChild(li);

// Add Button
li = document.createElement("li");
button = document.createElement("button");
button.textContent = "Add New Cocktail";

button.addEventListener("click", (e) => {
    document.location = "./add-cocktail";
});

li.appendChild(button);
buttonList.appendChild(li);

function requestHTTP(url) {
    var xmlHttp = new XMLHttpRequest();
    xmlHttp.open("GET", url, false);
    xmlHttp.send(null);
    return xmlHttp.responseText;
}
