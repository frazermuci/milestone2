var controllerInterval;
var socket;

function ControllerNewGame()
{
    getModel().newGame();
    getModel().isRunning = 1;
	ViewRefresh();
	socket = new Socket(getModel());
}

function ControllerStopGame()
{
    getModel().isRunning = 0;
	ViewRefresh();
	socket.done();
}

function ControllerTie()
{

	console.log("TIE");
	getModel().lastWinner = 0;
	ControllerStopGame();
}

function ControllerWin(id)
{
	console.log("WIN " + id);
	getModel().lastWinner = id+1;
	ControllerStopGame();
}

function ControllerTick()
{
	socket.sendMessage(socket.serialize(getModel()));
	ViewRefresh();
	return;
    var m = getModel();

    // Grow both snakes
    m.growSnake(0);
    m.growSnake(1);

    var snake1 = m.getSnake(0);
    var snake2 = m.getSnake(1);

    var head1 = snake1.getHead();
    var head2 = snake2.getHead();

    var body1 = snake1.getBody();
    var body2 = snake2.getBody();

    // Check collision for snakes
    var lose1 = false;
    var lose2 = false;

    // Heads colliding
    /*if(head1.equals(head2))
    {
        lose1 = true;
        lose2 = true;
    }*/

    // Out of the board
    if(!(head1.x >= 0 && head1.x < getModel().boardWidth && head1.y >= 0 && head1.y < getModel().boardHeight))
        lose1 = true;
    if(!(head2.x >= 0 && head2.x < getModel().boardWidth && head2.y >= 0 && head2.y < getModel().boardHeight))
        lose2 = true;

    // Colliding with other snake
    for(var i = 0; i < body1.length; i++)
    {
        if(head2.equals(body1[i]))
            lose2=true;
    }
    for(var i = 0; i < body2.length; i++)
    {
        if(head1.equals(body2[i]))
            lose1=true;
    }

    // Colliding with yourself
    for(var i = 1; i < body1.length; i++)
    {
        if(head1.equals(body1[i]))
            lose1=true;
    }
    for(var i = 1; i < body2.length; i++)
    {
        if(head2.equals(body2[i]))
            lose2=true;
    }

    // Check victory condition (+ tie)
    if(lose1 && lose2)
        ControllerTie();
    else if(lose1)
        ControllerWin(1);
    else if(lose2)
        ControllerWin(0);
	
    // Check bonus (head at bonus position)
	var bonuses = getModel().getBonuses();
    for(var i = 0; i < bonuses.length; i++)
	{
		if(head1.equals(bonuses[i]))
		{
			snake1.eatBonus();
			getModel().makeBonus(i);

			socket.sendMessage(0)
		}
		if(head2.equals(bonuses[i]))
		{
			snake2.eatBonus();
			getModel().makeBonus(i);

			socket.sendMessage(1);

		}
	}
	// Increment Clock
    ViewRefresh();
	
}

/*function ControllerChangeDirection(id, vector)
{
    var m = getModel();
    m.changeDirection(id, vector);
}*/
function ControllerChangeDirection(vector)
{
    var m = getModel();
    m.changeDirection(m.snakeIndex, vector);
}

function ControllerMainLoop()
{
    if (getModel().isRunning == 1)
    {
        ControllerTick();
    }
}

//controllerInterval = window.setInterval(ControllerMainLoop, 750);
//ControllerNewGame();
