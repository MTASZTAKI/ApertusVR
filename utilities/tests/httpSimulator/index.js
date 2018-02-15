var request = require('request');
var LineByLineReader = require('line-by-line');
var fileName = process.argv[2] || 'apertusvr.log';
var delay = process.argv[3] || 20;
var lr = new LineByLineReader(fileName);
var lineNumber = 0;

lr.on('error', function (err) {
    console.log('line-reader: error: ', err);
});

lr.on('line', function (line) {
    lr.pause();

    var dataObj = JSON.parse(line);
    lineNumber++;
    console.log('line-reader: line> ', lineNumber);

    var options = {
        uri: 'http://localhost:3000/api/v1/setproperties',
        method: 'POST',
        json: dataObj
    };

    request.post(options, function (error, response, body) {
            if (error) {
                console.log('Error: ', error);
            }
            setTimeout(function(){
                lr.resume();
            }, delay);
        }
    );
});

lr.on('end', function () {
    console.log('line-reader: all lines are read.');
    process.exit(0);
});
