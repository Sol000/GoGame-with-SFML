#pragma once
#include "Header.h"

struct free_field_group {
	int owner = 0;
	std::vector<sf::Vector2i> members;
};

struct group {
	std::vector<sf::Vector2i> members;
	int id;
	std::vector<sf::Vector2i> freedoms;
	bool is_closed = false;
};

struct stone {
	int id = 0;
	sf::Sprite body;
};



class Game
{
private:
	//print debug info
	const bool draw_field_points = true;
	const bool print_debug = false;

	const float GAMESTONE_RADIUS		= 17.f;
	const int BOARD_OFFSET_WINDOW		= 25;
	const int TITLEBAR_HIGHT			= 35;
	const float FIELD_OFFSET			= 43.f;
	const int FIELDPOINT_OFFSET			= 5;
	const int DELETE_OFFSET				= 20;

	const int PLAYER_BLACK_ID			= 1;
	const int PLAYER_WHITE_ID			= 2;

	const int FONT_SIZE					= 20;

	//declarations of some core variables 
	sf::Clock clock;
	float dt = 0;
	sf::RenderWindow* window;
	sf::RenderWindow* window_fin;
	sf::Vector2i last_mouse_action;
	sf::Vector2i last_array_action;

	//for drawing
	sf::Texture texture;
	sf::Sprite background;
	sf::Texture t_black_stone;
	sf::Texture t_white_stone;
	sf::Sprite black_stone;
	sf::Sprite white_stone;
	sf::Texture t_fieldpoint_white;
	sf::Texture t_fieldpoint_black;
	sf::Sprite fieldpoint_white;
	sf::Sprite fieldpoint_black;
	sf::Font font;
	sf::Text text_counter_1;
	sf::Text text_counter_2;
	sf::Text info;
	sf::Texture t_delete_stone;
	sf::Sprite delete_stone;
	sf::Texture t_summary;
	sf::Sprite scoreboard;
	sf::Text t_score_b;
	sf::Text t_score_w;
	sf::Text t_pass_hint;

	int player_id;
	int pass_count;
	bool finished;
	int gamestate;

	int score_black;
	int killed_by_black = 0;
	int score_white;
	int killed_by_white = 0;


	std::vector<group*> groups;
	std::vector<sf::Vector2i> stones_to_clear;
	std::vector<free_field_group*> fieldpoints;

	std::array<std::array<int, 19>, 19> board;
	std::array<std::array<stone, 19>, 19> drawing_board;


	//functions
	sf::Vector2i mouseToArray(sf::Vector2i& mousePos);
	std::string toString(const char* s, int i);

	void debug();
	void clearGroups();
	void clearFieldPoints();
	void update();
	void checkKill();
	void updateDrawingBoard();
	bool take_turn(sf::Vector2i& mouse_pos);
	void createGroups();
	void drawBoard();
	void drawFieldPoints();
	void drawText(sf::RenderWindow* window, int& i);
	void drawStonesToClear(sf::RenderWindow* window);
	void calculateFieldPoints();
	void calculateScore();
	void drawSummary(sf::RenderWindow* window);

public:
	Game();
	~Game();

	void run();
};

