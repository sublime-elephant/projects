let square = document.getElementById("dSquare");
let box = document.getElementById("dBoundingBox");
let squareStyle = getComputedStyle(square);
let boxStyle = getComputedStyle(box);
let hardlimitX1 = Number(boxStyle.left.slice(0,-2));
let hardlimitX2 = Number(boxStyle.width.slice(0,-2))+Number(boxStyle.left.slice(0,-2));
let hardlimitY1 = Number(boxStyle.top.slice(0,-2));
let hardlimitY2 = Number(boxStyle.height.slice(0,-2))+Number(boxStyle.top.slice(0,-2));
let mX = 50;
let mY = -7;
let gravity = 3;
let pos = [];
let nextpos = [];

function startTimer(){
    interval = setInterval(tick, 1);
}

function tick(){
    squareStyle = getComputedStyle(square);
    boxStyle = getComputedStyle(box);
    position();
    nextPosition();
    draw(nextpos[3], nextpos[0]);
    mX *= 0.99;
}

function draw(x2,y2){
    square.style.left = x2;
    square.style.top = y2;
}

function checkCollision() {
 
}

function nextPosition() {
    pos = position();
    //top
    nextpos[0] = pos[0] + mY;
    //right
    nextpos[1] = pos[1] + mX;
    //bottom
    nextpos[2] = pos[2] + mY;
    //left
    nextpos[3] = pos[3] + mX;

    if (nextpos[2] >= hardlimitY2) { //if the bottom of the square touches the bottom
        nextpos[0] = hardlimitY2;
        mY = mY * -0.99 ;

    }
    
    if (nextpos[0] <= hardlimitY1) {
        nextpos[0] = hardlimitY1;
        console.log("bounce top");
        mY *= -1;
    }
    
    if (nextpos[1] >= hardlimitX2) { // if the left of the square touches the right side
        nextpos[3] = hardlimitX2-Number(squareStyle.width.slice(0,-2));
        mX = mX * -1;
    }
    else{
        mX -= 0.2;
    }
    if (nextpos[3] <= hardlimitX1) { //if the left of the square touches the left side
        nextpos[3] = hardlimitX1;
        mX = mX * -1;
    }
    else {
        mX+=0.2;
    }
    document.getElementById("mY").innerHTML = mY;
    mY += gravity;
    if (Math.abs(mY) < 0.04) {mY = 0;}
    return nextpos;
}

function position(){
    //top,right,bottom,left
    pos[0] = Number(squareStyle.top.slice(0,-2));
    pos[1] = Number(squareStyle.left.slice(0,-2))+Number(squareStyle.width.slice(0,-2));
    pos[2] = Number(squareStyle.top.slice(0,-2))+Number(squareStyle.height.slice(0,-2));
    pos[3] = Number(squareStyle.left.slice(0,-2));
    // document.getElementById("pX").innerHTML = pos[3]+(Number(squareStyle.width.slice(0,-2))/2);
    // document.getElementById("pY").innerHTML = pos[0]+(Number(squareStyle.height.slice(0,-2))/2);
    return pos;
}

let button = document.getElementById("goButton");
button.addEventListener("click", startTimer);