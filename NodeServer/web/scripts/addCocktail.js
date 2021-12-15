let liqList = document.querySelector("#liqList");
let addBtn = document.querySelector("#add-btn");

// Remove all old options
while (liqList.firstChild) {
    liqList.firstChild.remove();
}

/**
 <label>
    liquid 1
    <div>
        <input
            name="lname"
            type="text"
            placeholder="liquidname"
        />
        <input
            name="lamount"
            type="number"
            placeholder="amount"
        />
    </div>
</label>
 */

function addLiquidBtn(event) {
    event.preventDefault();

    let id = liqList.children.length;

    let label = document.createElement("label");
    label.textContent = "Liquid " + id;

    let div = document.createElement("div");
    div.className = "liquid";

    let inputName = document.createElement("input");
    inputName.name = "lname" + id;
    inputName.type = "text";
    inputName.placeholder = "liquidname";
    inputName.required = "required";

    let amountName = document.createElement("input");
    amountName.name = "lamount" + id;
    amountName.type = "number";
    amountName.placeholder = "amount";
    amountName.required = "required";

    div.append(inputName);
    div.append(amountName);
    label.append(div);
    liqList.append(label);
}

addBtn.addEventListener("click", addLiquidBtn);

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
