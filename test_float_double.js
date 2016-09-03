    
    function testDouble(f) {
        var size = 200000000;
        for (var i = 0; i < size; ++i) {
            var t = Math.cos(f);
            if (Math.pow(t, 3.0) < 0.5) {
                t += 5 * t;
                t -= 2 * Math.sin(t);
            }
            f += t;
            if (f > 0.1) {
                f /= 10.0;
                f += Math.random();
            }
            f *= 196.0;
        }
        return f;
    }
    
  var t0 = new Date();
  var f = 0.0;
  testDouble(f);
  var t1 = new Date();
  console.log(t1.getTime() - t0.getTime());