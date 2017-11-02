var request = require('request');
var LineByLineReader = require('line-by-line');
var lr = new LineByLineReader(process.argv[2] || 'apertusvr.log');

lr.on('error', function (err) {
	console.log('line-reader: error: ', err);
});

lr.on('line', function (line) {
	lr.pause();

	var dataObj = JSON.parse(line);
	console.log('line-reader: ', JSON.stringify(dataObj));

	var options = {
		uri: 'http://localhost:3000/api/v1/setproperties',
		method: 'POST',
		json: dataObj
	};

	request.post(options, function (error, response, body) {
			if (error) {
				console.log('Error: ', error);
			}
			console.log();
			console.log('request: body: ', body);

			lr.resume();
		}
	);
});

lr.on('end', function () {
	console.log('line-reader: all lines are read.');
});
