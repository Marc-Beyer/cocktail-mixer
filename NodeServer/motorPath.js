const X_DIS_1 = 145;
const X_DIS_2 = 520;

const Y_DIS_1 = 600;
const Y_DIS_2 = 700;


function getPathFromCocktail(cocktail){

    let positions = [];
    for (const liquid of cocktail.liquids) {
        for (let index = 0; index < liquid.amount; index++) {
            positions.push(stations.indexOf(liquid.name));
        }
    }

    let movingDistance = [(positions[0] - 1) * X_DIS_2 + X_DIS_1];
    for (let index = 1; index < positions.length; index++) {
        movingDistance.push((positions[i] - positions[i - 1]) * X_DIS_2);
    }

    let path = "";
    for (let index = 0; index < movingDistance.length; index++) {
        pa
    }

    return path;
}