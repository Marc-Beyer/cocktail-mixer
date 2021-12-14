const X_DIS_1 = 145;
const X_DIS_2 = 520;

const Y_DIS_1 = 600;
const Y_DIS_2 = 700;

const X_MOTOR_ID = 0;
const Y_MOTOR_ID = 1;

const X_MOTOR_SPEED = 10;
const Y_MOTOR_SPEED = 6;

const LONG_DELAY = 6000;
const MEDIUM_DELAY = 2000;
const SHORT_DELAY = 1000;

const RIGHT_BTN_ID = 11;
const BOTTOM_BTN_ID = 12;

exports.getPathFromCocktail = (station, cocktail) => {

    let positions = [];
    for (const liquid of cocktail.liquids) {
        for (let index = 0; index < liquid.amount; index++) {
            positions.push(station.indexOf(liquid.name));
        }
    }
    console.log("pos", positions);

    let movingDistance = [(positions[0] - 1) * X_DIS_2 + X_DIS_2];
    for (let index = 1; index < positions.length; index++) {
        movingDistance.push((positions[i] - positions[i - 1]) * X_DIS_2);
    }
    console.log("movingDistance", movingDistance);

    let path = [`${X_MOTOR_ID}:${X_DIS_1}:${X_MOTOR_SPEED}:${RIGHT_BTN_ID}:${MEDIUM_DELAY}`];
    for (let index = 0; index < movingDistance.length; index++) {
        path.push(`${X_MOTOR_ID}:${movingDistance[index]}:${X_MOTOR_SPEED}:${RIGHT_BTN_ID}:${MEDIUM_DELAY}`);
        path.push(`${Y_MOTOR_ID}:${Y_DIS_1}:${X_MOTOR_SPEED}:${RIGHT_BTN_ID}:${LONG_DELAY}`);
        path.push(`${Y_MOTOR_ID}:${-Y_DIS_1 * 2}:${Y_MOTOR_SPEED}:${BOTTOM_BTN_ID}:${MEDIUM_DELAY}`);
    }

    return path;
}