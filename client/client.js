var clientAddress = document.getElementById("address").value;
var clientPort = document.getElementById("port").value;

function Socket(model){
	this.model = model;
	this.connection = new WebSocket('ws://'+clientAddress+':'+clientPort, ['soap', 'xmpp']);
	


// Log errors
	this.scoreArray = [0,0];
	this.count = 1;
	this.connection.onerror = function (error) {
		console.log('WebSocket Error ' + error);
	};

// Log messages from the server
	this.connection.onmessage = (e)=> {
		//this is in scope?
		var array = e.data.split(":");
		if(this.count)
		{
		//assumes client 0 based ids
			var ids = this.model.getIds();
			
			console.log("init:"+ids[0]+":"+ids[1]);
			this.connection.send("init:"+ids[0]+":"+ids[1]);
		}
		console.log(e.data)
		this.scoreArray[0] = array[0];
		this.scoreArray[1] = array[1];
		this.model.setScore(this.scoreArray);
		this.count =0;
		ViewRefresh();
	}
	this.sendMessage = function(inc)
	{
		this.connection.send(inc);
	}

	this.done = ()=>{this.connection.send("DONE")}
};