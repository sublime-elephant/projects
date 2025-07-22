
let ciphertext;
let plaintext;
let key;

function encrypt(){
    plaintext = document.getElementById("inpLeft").value;
    split_plaintext_bytes = [];
    let sCipherHex = [];
    let sPlainHex = [];
    for (let i = 0; i < plaintext.length; i++){
        sPlainHex[i]=(plaintext.charCodeAt(i).toString("16").padStart(2, '0'));
        split_plaintext_bytes[i] = plaintext.charCodeAt(i);
    }
    sPlainHex.splice(0,0,""); // needed so that there is a leading \x for the 
    //join statement. 
    document.getElementById("inpLeftHex").value = sPlainHex.join("\\x");


    key = document.getElementById("txtKey").value;
    key_bytes = [];

    for (let i = 0; i < key.length; i++){
        key_bytes[i]=(key.charCodeAt(i));
    }

    ciphertext = []; 

    let S = [];
    for (let i = 0; i < 256; i++){
        S[i]=i;
    }

    let j = 0;

    for (let i = 0; i < 256; i++){
        j = (j + S[i] + key_bytes[i % key_bytes.length]) % 256;
        //XOR-trick
        temp = S[i];
        S[i]=S[j];
        S[j]=temp;
    }

    console.log(S);

    i = 0;
    j = 0;

    let t = 0;
    let K = 0; 
    let c = [];

    for (let x = 0; x < split_plaintext_bytes.length; x++) {
        i = (i + 1) % 256;
        j = (j + S[i]) % 256;
        temp = S[i];
        S[i]=S[j];
        S[j]=temp;
        t = (S[i] + S[j]) % 256;
        K = S[t];

        sCipherHex[x]=((K^split_plaintext_bytes[x]).toString("16").padStart(2, '0'));
        c[x]=(String.fromCharCode(K^split_plaintext_bytes[x]));
        console.log("stats");
        console.log(K);
        console.log(split_plaintext_bytes[x])
        console.log(K^split_plaintext_bytes[x]);
    }
    sCipherHex.splice(0,0,"");
    document.getElementById("inpRightHex").style.visibility = "visible";
    document.getElementById("inpLeftHex").style.visibility = "visible";
    document.getElementById("inpRightHex").value = sCipherHex.join("\\x");

    let a=[];
    for (let x = 0; x<S.length;x++){
        a[x]=S[x].toString("16").padStart("2","0");
    }
    document.getElementById("txtKeyStream").value = a.join(" ");


    c = c.join("");
    console.log(c);
    document.getElementById("inpRight").value = c;
}

function decrypt(){
    ciphertext = document.getElementById("inpRight").value;
    split_ciphertext_bytes = [];
    let sPlainHex = [];
    let sCipherHex=[];
    for (let i = 0; i < ciphertext.length; i++){
        sCipherHex[i]=(ciphertext.charCodeAt(i).toString("16").padStart(2,"0"));
        split_ciphertext_bytes[i] = ciphertext.charCodeAt(i);
    }
    sCipherHex.splice(0,0,"");
    document.getElementById("inpRightHex").value = sCipherHex.join("\\x");
    key = document.getElementById("txtKey").value;
    key_bytes = [];

    for (let i = 0; i < key.length; i++){
        key_bytes[i]=(key.charCodeAt(i));
    }
    // key_expand = key.repeat(Math.floor((ciphertext.length / key.length)))
    // key_expand = key_expand.concat(key.slice(0,(ciphertext.length % key.length)));
    // split_key = key_expand.split("");

    plaintext = []; 

    let S = [];
    for (let i = 0; i < 256; i++){
        S[i]=i;
    }
    console.log(S);

    let j = 0;

    for (let i = 0; i < 256; i++){
        j = (j + S[i] + key_bytes[i % key_bytes.length]) % 256;
        temp = S[i];
        S[i]=S[j];
        S[j]=temp;
    }
    i = 0;
    j = 0;

    let t = 0;
    let K = 0;
    let c = [];
    for (let x = 0; x < split_ciphertext_bytes.length; x++) {
        i = (i + 1) % 256;
        j = (j + S[i]) % 256;
        temp = S[i];
        S[i]=S[j];
        S[j]=temp;
        t = (S[i] + S[j]) % 256;
        K = S[t];
        sPlainHex[x]=((K^split_ciphertext_bytes[x]).toString("16").padStart(2, '0'));
        c[x]=(String.fromCharCode(K^split_ciphertext_bytes[x]));
        console.log("stats");
        console.log(K);
        console.log(split_ciphertext_bytes[x])
        console.log(K^split_ciphertext_bytes[x]);
    }
    sPlainHex.splice(0,0,"");
    document.getElementById("inpLeftHex").value = sPlainHex.join("\\x");
    document.getElementById("inpRightHex").style.visibility = "visible";
    document.getElementById("inpLeftHex").style.visibility = "visible";
    let a=[];
    for (let x = 0; x<S.length;x++){
        a[x]=S[x].toString("16").padStart("2","0");
    }
    document.getElementById("txtKeyStream").value = a.join(" ");
    c = c.join("");
    console.log(c);
    document.getElementById("inpLeft").value = c;
    check_answer(c);
}

function init(){
    let collection = document.getElementsByClassName("inpArea");
    let collection_length = collection.length;
    for (let i = 0; i < collection_length; i++){
        collection[i].value = "";
    }
    document.getElementById("txtKey").value="";
    document.getElementById("txtKeyStream").value="";
}

function check_answer(answer) {
    f = document.getElementById("inpRight");
    if (answer = "3b99") {
        f.style.boxshadow = "red";
    }
}


document.getElementById("butDecrypt").addEventListener("click",function(){decrypt()});
document.getElementById("butEncrypt").addEventListener("click",function(){encrypt()});

document.body.onload = init();


