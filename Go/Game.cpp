#include "Game.h"

//prints Informations
void Game::debug()
{
	dt += clock.getElapsedTime().asSeconds();
	
	std::stringstream ss = std::stringstream();
	ss << "GO GO Powerrangers  -- Framerate: " << 1.f / clock.getElapsedTime().asSeconds();
	window->setTitle(ss.str());
	
	clock.restart();

	if (dt > 0.5) {
		
		dt = 0;


		system("cls");

		dt = 0;
		//print Mouse Position in Window
		std::cout << sf::Mouse::getPosition(*window).x << " " << sf::Mouse::getPosition(*window).y << " --- ";

		//print LAST Mouse Position in Window
		//std::cout << last_mouse_action.x << " " << last_mouse_action.y << " --- ";

		//print LAST Array action
		//std::cout << last_array_action.x << " " << last_array_action.y << std::endl;

		//print board
		/*for (int y = 0; y < board.size(); y++) {

			for (int x = 0; x < board.size(); x++)
				std::cout << board[x][y];

			std::cout << std::endl;
		}*/

		//print groups
		/*std::cout << "Group Counter: " << groups.size() << std::endl;
		std::cout << "-----------------------" << std::endl;
		for (auto group : groups) {
			std::cout << "Owner: " << group->id << std::endl;
			std::cout << "Closed: " << group->is_closed << std::endl;
			std::cout << "freedoms[" << group->freedoms.size() << "]: ";
			for (auto freedom : group->freedoms)
				std::cout << freedom.x << "," << freedom.y << " ";
			std::cout << std::endl;
			std::cout << "members[" << group->members.size() << "]: ";
			for (auto member : group->members)
				std::cout << member.x << "," << member.y << " ";

			std::cout << std::endl << "- - - - - - - - - -" << std::endl;
		}*/

		//print score and fieldpoints

		/*std::cout << "Fieldpoint Fields: " << fieldpoints.size() << std::endl;
		std::cout << "-----------------------" << std::endl;
		for (auto fieldpoint_group : fieldpoints) {
			std::cout << "Owner: " << fieldpoint_group->owner << std::endl;
			std::cout << "Members[" << fieldpoint_group->members.size() << "]: ";
			for (auto member : fieldpoint_group->members)
				std::cout << member.x << "," << member.y << " ";
			std::cout << std::endl << "- - - - - - - - - -" << std::endl;
		}*/
	}

	
}

//translates the mouseposition to the arrayposition
sf::Vector2i Game::mouseToArray(sf::Vector2i& mousePos)
{
	sf::Vector2i array_pos;

	array_pos.x = round(float(mousePos.x) / FIELD_OFFSET);
	array_pos.y = round(float(mousePos.y)  / FIELD_OFFSET);

	return array_pos;
}

// returns a text including variables
std::string Game::toString(const char* s, int i)
{
	std::stringstream ss;

	ss << s << " " << i;

	return ss.str();
}

//core update function
void Game::update()
{
	score_black = 0;
	score_white = 0;

	//1. check the board for points beginning by the last placed stone
	createGroups();

	//2. update boardp
	checkKill();

	//calculating and setting fieldpoints
	if (groups.size() > 1)
		calculateFieldPoints();

	calculateScore();

	//3. update drawing_board
	updateDrawingBoard();

}

//clears all the groups
void Game::clearGroups()
{
	for (int i = 0; i < groups.size(); i++)
		delete groups[i];

}

//resets and clears field Points
void Game::clearFieldPoints() 
{
	for (int i = 0; i < fieldpoints.size(); i++)
		delete fieldpoints[i];
}

//takes a turn
bool Game::take_turn(sf::Vector2i& mouse_pos)
{
	//get the position in the 2D array from the mouse pressed event
	last_array_action = mouseToArray(mouse_pos);

	if (last_array_action.y > 18 || last_array_action.x > 18)
		return false;

	//check if field is empty
	//cancel function execution
	try {
		if (board[last_array_action.x][last_array_action.y] != 0)
			return false;

		//if it is empty
		//setting player id to field
		board[last_array_action.x][last_array_action.y] = player_id;

		//Swap Player ID for next turn
		if (player_id == PLAYER_BLACK_ID) player_id = PLAYER_WHITE_ID;
		else player_id = PLAYER_BLACK_ID;

		pass_count = 0;

		return true;
	}
	catch (const std::out_of_range e) {
		return false;
	}
	
}

//Check board for groups and creates an std::vector of it
void Game::createGroups()
{


	//clear groups vector
	clearGroups();

	groups.clear(); // std::vector<group*>

	//needed variables/objectives
	std::vector<sf::Vector2i> stones_checked;

	//Create Groups
	for (int y = 0; y<19; y++)
		for(int x = 0; x<19;x++)
			if (board[x][y] != 0) {
				bool is_checked = false;
				
				for (int i = 0; i < stones_checked.size(); i++)
					if (stones_checked[i] == sf::Vector2i(x, y))
						is_checked = true;

				if (!is_checked) {
					int id = board[x][y];

					group* g = new group;
					g->id = id;

					std::queue<sf::Vector2i> to_check;
					std::vector<sf::Vector2i> checked_in_group;
					std::vector < sf::Vector2i> checked_freedoms;

					to_check.push(sf::Vector2i(x, y));

					while (!to_check.empty() && !is_checked) {
						is_checked = false;
						sf::Vector2i s = to_check.front();

						//if this stone is already checked
						for (int i = 0; i < checked_in_group.size(); i++)
							if (s == checked_in_group[i]) {
								is_checked = true;
								break;
							}
								
						for (int i = 0; i < stones_checked.size(); i++)
							if (s == stones_checked[i]) {
								is_checked = true;
								break;
							}

						if (!is_checked) {
							//check if the next stone is also from same id
							if(!(s.y - 1 < 0))
								if (board[s.x][s.y - 1] == id) { to_check.push(sf::Vector2i(s.x, s.y - 1)); } //up
							if (!(s.x + 1 > 18))
								if (board[s.x + 1][s.y] == id) { to_check.push(sf::Vector2i(s.x + 1, s.y)); } //right 
							if (!(s.y + 1 > 18))
								if (board[s.x][s.y + 1] == id) { to_check.push(sf::Vector2i(s.x, s.y + 1)); } //down
							if (!(s.x - 1 < 0))
								if (board[s.x - 1][s.y] == id) { to_check.push(sf::Vector2i(s.x - 1, s.y)); } //left

							//check if neighbor is freespace and set counter
							bool skip_up = false;
							bool skip_right = false;
							bool skip_down = false;
							bool skip_left = false;
							for (int i = 0; i < g->freedoms.size(); i++) {
								if (g->freedoms[i] == sf::Vector2i(s.x, s.y - 1))
									skip_up = true;
								if (g->freedoms[i] == sf::Vector2i(s.x + 1, s.y))
									skip_right = true;
								if (g->freedoms[i] == sf::Vector2i(s.x, s.y + 1))
									skip_down = true;
								if (g->freedoms[i] == sf::Vector2i(s.x - 1, s.y))
									skip_left = true;
							}

							if (!(s.y - 1 < 0) && !skip_up)
								if (board[s.x][s.y - 1] == 0) { g->freedoms.push_back(sf::Vector2i(s.x, s.y - 1)); } //up
							if (!(s.x + 1 > 18) && !skip_right)
								if (board[s.x + 1][s.y] == 0) { g->freedoms.push_back(sf::Vector2i(s.x + 1, s.y)); } //right 
							if (!(s.y + 1 > 18) && !skip_down)
								if (board[s.x][s.y + 1] == 0) { g->freedoms.push_back(sf::Vector2i(s.x, s.y + 1)); } //down
							if (!(s.x - 1 < 0) && !skip_left)
								if (board[s.x - 1][s.y] == 0) { g->freedoms.push_back(sf::Vector2i(s.x - 1, s.y)); } //left							

							g->members.push_back(s);
							stones_checked.push_back(s);
							checked_in_group.push_back(s);
						}
						
						
						to_check.pop();
						
						if (is_checked)
							is_checked = false;
					}

					groups.push_back(g);
				}
				
			}

}

//updates the board because of data from groups
void Game::checkKill()
{

	for (int i = 0; i < groups.size(); i++)
		if (groups[i]->freedoms.size() == 0) {
			if (groups[i]->id == PLAYER_BLACK_ID) 
				killed_by_white += groups[i]->members.size();				
			else 
				killed_by_black += groups[i]->members.size();

			for (auto member : groups[i]->members)
				board[member.x][member.y] = 0;
			createGroups();
		}
}

//updates the drawing_board
void Game::updateDrawingBoard()
{
	for (int y = 0; y < 19; y++)
		for (int x = 0; x < 19; x++)
			drawing_board[x][y] = stone();


	for (int y = 0; y < 19; y++) {
		for (int x = 0; x < 19; x++) {
			if (board[x][y] != 0) {
				drawing_board[x][y].id = board[x][y];
				if (drawing_board[x][y].id == PLAYER_BLACK_ID)
					drawing_board[x][y].body = black_stone;
				else
					drawing_board[x][y].body = white_stone;

				drawing_board[x][y].body.setPosition(x * FIELD_OFFSET + BOARD_OFFSET_WINDOW - GAMESTONE_RADIUS, y * FIELD_OFFSET + BOARD_OFFSET_WINDOW - GAMESTONE_RADIUS);
			}
		}
	}
}

//Draw the drawing_board
void Game::drawBoard()
{
	for (int y = 0; y < 19; y++) {
		for (int x = 0; x < 19; x++) {
			if (drawing_board[x][y].id == 0)
				continue;

			window->draw(drawing_board[x][y].body);
		}
	}
}

//Drawing fieldpoints
void Game::drawFieldPoints()
{

	for (auto g : fieldpoints)
		for (auto member : g->members) 
			if (g->owner == 1) {
				fieldpoint_black.setPosition(sf::Vector2f(member.x * FIELD_OFFSET - FIELDPOINT_OFFSET + BOARD_OFFSET_WINDOW, member.y * FIELD_OFFSET - FIELDPOINT_OFFSET + BOARD_OFFSET_WINDOW));
				window->draw(fieldpoint_black);
			}
			else {
				fieldpoint_white.setPosition(sf::Vector2f(member.x * FIELD_OFFSET - FIELDPOINT_OFFSET + BOARD_OFFSET_WINDOW, member.y * FIELD_OFFSET - FIELDPOINT_OFFSET + BOARD_OFFSET_WINDOW));
				window->draw(fieldpoint_white);
			}
				
}

//CONSTRUCTOR
Game::Game()
{
	//init window
	window = new sf::RenderWindow(sf::VideoMode(819, 819+TITLEBAR_HIGHT), "GO GO Powerrangers!", sf::Style::Titlebar | sf::Style::Close);
	
	//init clock
	clock = sf::Clock();

	//init Image/background
	if (!texture.loadFromFile("board.png")) {
		std::cout << "[ERROR] Couldn't load backroundimage!" << std::endl;
	}
	
	//init the sprite 
	background = sf::Sprite();
	background.setPosition(sf::Vector2f(0, 0));
	background.setTexture(texture);
	background.scale(sf::Vector2f(0.8, 0.8));

	//init board
	for (int i = 0; i < board.size(); i++)
		for (int j = 0; j < board.size(); j++) 
			board[i][j] = 0;

	//init stones
	if (!t_black_stone.loadFromFile("black_stone.png")) std::cout << "[ERROR] Couldn't load Image Black Stone" << std::endl;
	black_stone = sf::Sprite(t_black_stone);
	
	if (!t_white_stone.loadFromFile("white_stone.png")) std::cout << "[ERROR] Couldn't load Image White Stone" << std::endl;
	white_stone = sf::Sprite(t_white_stone);

	//init utils
	player_id = PLAYER_BLACK_ID;
	finished = false;
	gamestate = 0;
	
	//init score 
	score_black = 0;
	score_white = 0;

	//init fieldpoints
	t_fieldpoint_black.loadFromFile("field_point_black.png");
	fieldpoint_black = sf::Sprite(t_fieldpoint_black);

	t_fieldpoint_white.loadFromFile("field_point_white.png");
	fieldpoint_white = sf::Sprite(t_fieldpoint_white);
	

	//init Font and Text
	font.loadFromFile("Hakubo.ttf");
	text_counter_1.setFont(font);
	text_counter_1.setFillColor(sf::Color::White);
	text_counter_1.setCharacterSize(FONT_SIZE);
	text_counter_1.setPosition(sf::Vector2f(32, 819 + 5));
	text_counter_2.setFont(font);
	text_counter_2.setFillColor(sf::Color::White);
	text_counter_2.setCharacterSize(FONT_SIZE);
	text_counter_2.setPosition(sf::Vector2f(200, 819 + 5));
	info.setFont(font);
	info.setFillColor(sf::Color::White);
	info.setCharacterSize(FONT_SIZE);
	info.setPosition(sf::Vector2f(600, 819 + 5));
	t_pass_hint = sf::Text();
	t_pass_hint.setFont(font);
	t_pass_hint.setFillColor(sf::Color::White);
	t_pass_hint.setCharacterSize(FONT_SIZE);
	t_pass_hint.setPosition(sf::Vector2f(400, 819 + 5));

	
	//init delete_stone
	t_delete_stone = sf::Texture();
	t_delete_stone.loadFromFile("delete.png");
	delete_stone = sf::Sprite(t_delete_stone);

	//init summary
	t_summary = sf::Texture();
	t_summary.loadFromFile("scoreboard.png");
	scoreboard = sf::Sprite(t_summary);
	scoreboard.setPosition(sf::Vector2f(window->getSize().x / 2 - t_summary.getSize().x / 2, window->getSize().y / 2 - t_summary.getSize().y / 2));


	//create score texture
	t_score_w = sf::Text();
	t_score_b = sf::Text();


}

//DESTRUCTOR
Game::~Game()
{
	clearGroups();
	clearFieldPoints();

	delete window;
}

//drawing points and curent status of the game 
//param: gamephases -> 0 - playing, 1 - clearing deadstones
void Game::drawText(sf::RenderWindow* window, int& i)
{ 
	//print black current score
	text_counter_1.setString(toString("Score Black:" , score_black));
	window->draw(text_counter_1);
	

	//print white current score 
	text_counter_2.setString(toString("Score White:", score_white));
	window->draw(text_counter_2);

	if (i == 0) {
		//prints curent players turn 
		if (player_id == PLAYER_BLACK_ID)
			info.setString(">>>BLACK player's turn<<<");
		else
			info.setString(">>>WHITE player's turn<<<");

		t_pass_hint.setString("Pass [P]");
	}
	else if (i == 1) {
		info.setString(">>>Mark Dead Stones<<<");
		t_pass_hint.setString("Finished [F]");
	}
		

	window->draw(t_pass_hint);
	window->draw(info);
}

//calculates fieldpoints
void Game::calculateFieldPoints()
{
	//clear all 
	clearFieldPoints();

	fieldpoints.clear();

	std::vector<sf::Vector2i> fields_checked;
	

	for (int y = 0; y < 19; y++)
		for (int x = 0; x < 19; x++)
			if (board[x][y] == 0) {
				bool is_checked = false;

				for (int i = 0; i < fields_checked.size(); i++)
					if (fields_checked[i] == sf::Vector2i(x, y))
						is_checked = true;

				if (!is_checked) {
					free_field_group* g = new free_field_group;

					std::queue<sf::Vector2i> to_check;
					std::vector<sf::Vector2i> checked_in_group;

					to_check.push(sf::Vector2i(x, y));

					bool valid = true;

					while (!to_check.empty() && !is_checked) {
						is_checked = false;
						sf::Vector2i s = to_check.front();

						for (int i = 0; i < checked_in_group.size(); i++)
							if (s == checked_in_group[i]) {
								is_checked = true;
								break;
							}

						for (int i = 0; i < fields_checked.size(); i++)
							if (s == fields_checked[i]) {
								is_checked = true;
								break;
							}

						if (!is_checked) {
							//check if the next stone is also from same id
							if (!(s.y - 1 < 0)) {
								if (board[s.x][s.y - 1] == 0)
									to_check.push(sf::Vector2i(s.x, s.y - 1));

								else if (g->owner != board[s.x][s.y - 1])
									if (g->owner == 0)
										g->owner = board[s.x][s.y - 1];
									else if (g->owner != board[s.x][s.y - 1])
										valid = false;
							}

							if (!(s.x + 1 > 18)) {
								if (board[s.x + 1][s.y] == 0)
									to_check.push(sf::Vector2i(s.x + 1, s.y));

								//checking if owner is set and setts it if not 
								//sets valid to false if it finds bouth players in this area
								else if (g->owner != board[s.x + 1][s.y])
									if (g->owner == 0) 
										g->owner = board[s.x + 1][s.y];
									else if (g->owner != board[s.x + 1][s.y])
										valid = false;
							}

							if (!(s.y + 1 > 18)) {
								if (board[s.x][s.y + 1] == 0)
									to_check.push(sf::Vector2i(s.x, s.y + 1));

								else if (g->owner != board[s.x][s.y + 1])
									if (g->owner == 0)
										g->owner = board[s.x][s.y + 1];
									else if (g->owner != board[s.x][s.y + 1])
										valid = false;
							}

							if (!(s.x - 1 < 0)) {
								if (board[s.x - 1][s.y] == 0)
									to_check.push(sf::Vector2i(s.x - 1, s.y));

								else if (g->owner != board[s.x - 1][s.y])
									if (g->owner == 0)
										g->owner = board[s.x - 1][s.y];
									else if (g->owner != board[s.x - 1][s.y])
										valid = false;
							}

							g->members.push_back(s);
							checked_in_group.push_back(s);
							fields_checked.push_back(s);
							
							
						}

						to_check.pop();
						if (valid) {

							

						}

						if (is_checked)
							is_checked = false;

						
					}

					if (valid && g->owner != 0)
						fieldpoints.push_back(g);

					else
						delete g;

				}
			}


	
		
	
}

//calculates the score 
void Game::calculateScore()
{
	for (int i = 0; i < fieldpoints.size(); i++) {
		if (fieldpoints[i]->owner == 1)
			score_black += fieldpoints[i]->members.size();
		if (fieldpoints[i]->owner == 2)
			score_white += fieldpoints[i]->members.size();
	}

	score_white += killed_by_white;
	score_black += killed_by_black;

}

//draws all "to clear stones"
void Game::drawStonesToClear(sf::RenderWindow* window)
{
	for (auto m : stones_to_clear) {
		delete_stone.setPosition(sf::Vector2f(m.x * FIELD_OFFSET - DELETE_OFFSET + BOARD_OFFSET_WINDOW, m.y * FIELD_OFFSET - DELETE_OFFSET + BOARD_OFFSET_WINDOW));
		window->draw(delete_stone);
	}

}

//shows the gamesummary if finished
void Game::drawSummary(sf::RenderWindow* window) {

	//create background 
	t_score_b.setFont(font);
	t_score_w.setFont(font);
	t_score_w.setString(std::to_string(score_white));
	t_score_b.setString(std::to_string(score_black));
	t_score_w.setCharacterSize(80);
	t_score_b.setCharacterSize(80);
	t_score_w.setPosition(sf::Vector2f(scoreboard.getPosition().x + 320, scoreboard.getPosition().y + 180));
	t_score_b.setPosition(sf::Vector2f(scoreboard.getPosition().x + 90, scoreboard.getPosition().y + 180));
	t_score_w.setFillColor(sf::Color::Black);
	t_score_b.setFillColor(sf::Color::Black);

	window->draw(scoreboard);
	window->draw(t_score_w);
	window->draw(t_score_b);

}

//RUNS THE GAME
void Game::run()
{

	while (window->isOpen() && !finished)
	{
		sf::Event event;
		while (window->pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window->close();

			if (event.type == sf::Event::MouseButtonPressed && gamestate == 0) {
				
				last_mouse_action = sf::Vector2i(sf::Mouse::getPosition(*window).x - BOARD_OFFSET_WINDOW, sf::Mouse::getPosition(*window).y - BOARD_OFFSET_WINDOW);
				
				if (!take_turn(last_mouse_action)) 
					std::cout << "[INFO] Invalid move!" << std::endl;		

			}	

			if (event.type == sf::Event::MouseButtonPressed && gamestate == 1) {

				last_mouse_action = sf::Vector2i(sf::Mouse::getPosition(*window).x - BOARD_OFFSET_WINDOW, sf::Mouse::getPosition(*window).y - BOARD_OFFSET_WINDOW);
				last_array_action = mouseToArray(last_mouse_action);

				//check if the last klick is a stone and is in a group
				for (auto g : groups) 
					for (auto m : g->members)
						if (m == last_array_action)
							for (auto m : g->members)
								stones_to_clear.push_back(m);
			}

			if (event.type == sf::Event::KeyPressed && gamestate == 0) {
				/*if (event.key.code == sf::Keyboard::S)
					if (player_id == PLAYER_BLACK_ID) player_id = PLAYER_WHITE_ID;
					else player_id = PLAYER_BLACK_ID;*/

				if (event.key.code == sf::Keyboard::P)
					if (pass_count == 1) {
						gamestate = 1;
						pass_count = 0;
					}
					else
						pass_count++;

				if (event.key.code == sf::Keyboard::Space) {
					//clear intern board
					for (int y = 0; y < board.size(); y++)
						for (int x = 0; x < board.size(); x++)
							board[x][y] = 0;
				}
			}
			
			if (event.type == sf::Event::KeyPressed && gamestate == 1) {
				if (event.type == sf::Event::KeyPressed)
					if (event.key.code == sf::Keyboard::P)
						if (pass_count == 1) {
							for (auto m : stones_to_clear)
								board[m.x][m.y] = 0;
							gamestate = 2;
							pass_count = 0;
						}
						else
							pass_count++;
			}

			if (event.type == sf::Event::KeyPressed && gamestate == 2){
				if (event.key.code == sf::Keyboard::J) {
					//clear intern board
					for (int y = 0; y < board.size(); y++)
						for (int x = 0; x < board.size(); x++)
							board[x][y] = 0;

					if (event.key.code == sf::Keyboard::N)
						window->close();
					
					gamestate = 0;
				}
					
			}
				
		}

		window->clear();
		//update
		update();

		if (print_debug)
			debug();

		//draw
		window->draw(background);
		drawBoard();

		if (draw_field_points)
			drawFieldPoints();

		if (gamestate == 1) {
			drawStonesToClear(window);
		}
		if (gamestate == 2) {
			drawSummary(window);
		}
		

		drawText(window, gamestate);

		//display 
		window->display();
			
	
	}
	

}
