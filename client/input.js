function Controller (model, render)
{
	this.model = model;
	this.render = render;
};

addEventListener("keypress", handleKeyPress);

function handleKeyPress(event)
{
	var model = getModel();//new Model(8, 6, 0);
	var code = event.which || event.keyCode;
	var vector = new Vector(0, 0);
	if (code == 37)
	{
	 	vector.setX(-1);
		//ControllerChangeDirection(0, vector);
		ControllerChangeDirection(vector);
		console.log("left");
		
	}
	else if (code == 38)
	{
		vector.setY(-1);
		//ControllerChangeDirection(0, vector);
		ControllerChangeDirection(vector);
		console.log("up");
	}
	else if (code == 39)
	{
		vector.setX(1);
		//ControllerChangeDirection(0, vector);
		ControllerChangeDirection(vector);
		console.log("right");
	}
	else if (code == 40)
	{
		vector.setY(1);
		//ControllerChangeDirection(0, vector);
		ControllerChangeDirection(vector);
		console.log("down");
	}
	/*if (code == 97)
	{
	 	vector.setX(-1);
		ControllerChangeDirection(1, vector);
		console.log("left");
		
	}
	else if (code == 119)
	{
		vector.setY(-1);
		ControllerChangeDirection(1, vector);
		console.log("up");
	}
	else if (code == 100)
	{
		vector.setX(1);
		ControllerChangeDirection(1, vector);
		console.log("right");
	}
	else if (code == 115)
	{
		vector.setY(1);
		ControllerChangeDirection(1, vector);
		console.log("down");
	}*/
}