function pointerAliasSameType(a, b, res, size) {
        size = size - 1;
        for (var i = 1; i < size; ++i) {
            a[i] += res[i];
            b[i] += res[i];
            
            a[i - 1] *= res[i - 1];
            b[i - 1] *= res[i - 1];
            
            a[i + 1] += res[i + 1];
            b[i + 1] += res[i + 1];
        }
    }
 var POINTER_ALIAS_TEST_SIZE = 65000000;//300000000;
 var a = new Array();
 var b = new Array();
 var res = new Array();
 
 console.log("Preparing test ...");
 a.length = b.length = res.length = POINTER_ALIAS_TEST_SIZE;
 
 for (var i = 0; i < POINTER_ALIAS_TEST_SIZE; ++i) {
    a[i] = i; b[i] = i * 2; res[i] = i * 3;
 }

//pre-heat
pointerAliasSameType(a, b, res, POINTER_ALIAS_TEST_SIZE);

console.log("Starting test...");

var begin = new Date();

pointerAliasSameType(a, b, res, POINTER_ALIAS_TEST_SIZE);

var end = new Date();
var seconds = end.getTime() - begin.getTime();

console.log("time = " + seconds/1000);// + " " + ms);