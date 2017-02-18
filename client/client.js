var clientAddress = "127.0.0.1";
var clientPort = "21234";

function Socket(model){
	console.log("hello");
	this.model = model;
	this.connection = new WebSocket('ws://'+clientAddress+':'+clientPort, ['soap', 'xmpp']);
	


// Log errors
	//this.scoreArray = [0,0];
	//this.count = 1;
	this.connection.onerror = function (error) {
		console.log('WebSocket Error ' + error);
	};
	
	this.sendMessage = function(inc)
	{
		this.connection.send(inc);
	}
// Log messages from the server
	this.connection.onmessage = (e)=> {
		//this is in scope?
		var array = e.data.split(":");
/* 		if(this.count)
		{
		//assumes client 0 based ids
			var ids = this.model.getIds();
			
			console.log("init:"+ids[0]+":"+ids[1]);
			this.connection.send("init:"+ids[0]+":"+ids[1]);
		} */
		if (array[0] == "init")
		{
			console.log("init");
			//array[1] = model.snakeID;
			this.sendMessage("init:" + model.snakeID);
		}
		else if(array[0] == "start")
		{
			parseInt(array[1]);
			getModel().snakeIndex = parseInt(array[2]);
			window.setTimeout(ControllerTick, 750);
		}
		else 
		{
			deserialize(array[0]);
			ViewRefresh();
			window.setTimeout(ControllerTick, 750);
		}
		console.log(e.data)
		//{
			//deserialize(array);
			//sendMessage(serialize(model));
		//}
		//this.scoreArray[0] = array[0];
		//this.scoreArray[1] = array[1];
		//this.model.setScore(this.scoreArray);
		this.count =0;
		ViewRefresh();
	}
	
	this.deserialize = function(s)
	{
    var a = s[0];
    var s1Dir = Vector(1,0);
    var s1Bonus = false;
    var s1Loss = false;
    var s2Dir=Vector(1,0);
    var s2Bonus=false;
    var s2Loss=false;
    var s1BonusX = 0;
    var s1BonusY = 0;
    var s2BonusX = 0;
    var s2BonusY = 0;
    
    // s1Dir s1Dir s1Bonus s1Loss s2Dir s2Dir s2Bonus s2Loss
    if(a > 127)
    {
        a -= 128;
        if(a > 63) // Up
        {
            a-=64;
            s1Dir = Vector(0,-1);
        }
        else // Down
            s1Dir = Vector(0, 1);
    }
    else
    {
        if(a > 63) // Right
        {
            a-=64;
            s1Dir = Vector(1,0);
        }
        else // Left
            s1Dir = Vector(-1, 0);
    }
    
    if(a > 31)
    {
        a-= 32;
        s1Bonus = true;
    }
    
    if(a > 15)
    {
        a-= 16;
        s1Loss = true;
    }
    
    if(a > 7)
    {
        a -= 8;
        if(a > 3) // Up
        {
            a-=4;
            s2Dir = Vector(0,-1);
        }
        else // Down
            s2Dir = Vector(0, 1);
    }
    else
    {
        if(a > 3) // Right
        {
            a-=4;
            s2Dir = Vector(1,0);
        }
        else // Left
            s2Dir = Vector(-1, 0);
    }
    
    if(a > 1)
    {
        a-= 2;
        s2Bonus = true;
    }
    
    if(a > 0)
    {
        a-= 1;
        s2Loss = true;
    }
    
    var i = 1;
    
    if(s1Bonus)
    {
        var b = s[i];
        s1BonusY = b%16;
        s1BonusX = (b-s1BonusY)/16;
        i++;
    }
    if(s2Bonus)
    {
        var c = s[i];
        s2BonusY = c%16;
        s2BonusX = (c-s2BonusY)/16;
        i++;
    }
    
    // SET TO MODEL
    getModel().changeDirection(0, s1Dir);
    getModel().changeDirection(1, s2Dir);
    getModel().growSnake(0);
    getModel().growSnake(1);
    if(s1Bonus)
    {
        getModel().getSnake(0).eatBonus();
        
        var bonusToChange = 1;
        var newBonusPos = Vector(s1BonusX, s1BonusY);
        var snake1Head = getModel().getSnake(0).getHead();
        
        if(snake1Head.equals(getModel.getBonusPos(0)))
            bonusToChange = 0;
        
        getModel().setBonus(bonusToChange,newBonusPos);
    }
    if(s2Bonus)
    {
        getModel().getSnake(1).eatBonus();
        
        var bonusToChange = 1;
        var newBonusPos = Vector(s2BonusX, s2BonusY);
        var snake1Head = getModel().getSnake(0).getHead();
        
        if(snake1Head.equals(getModel.getBonusPos(0)))
            bonusToChange = 0;
        
        getModel().setBonus(bonusToChange,newBonusPos);
    }
    
    if(lose1 && lose2)
        ControllerTie();
    else if(lose1)
        ControllerWin(2);
    else if(lose2)
        ControllerWin(1);
	}
	
	this.serialize = function(model)
	{
		
    var snake = getModel().getSnake(model.snakeIndex); // TODO NEEDS TO KNOW WHAT SNAKE
    var dir = snake.direction;
    
    if(dir.equals(new Vector(1,0))) // Right
        return 64+16+4+1; // 01 01 01 01
    if(dir.equals(new Vector(0,-1))) // Up
        return 128+64+32+16+8+4+2+1; // 11 11 11 11
    if(dir.equals(new Vector(-1,0))) // Left
        return 0; // 00 00 00 00
    if(dir.equals(new Vector(0,1))) // Down
        return 128+32+8+2; // 01 01 01 01
}


	this.done = ()=>{this.connection.send("DONE")}
};