#include <iostream>

#include <SDL.h>
#include <SDL_image.h>

#include <ctime>
#include <vector>
#include <utility>

enum GameStatus
{
	EXIT, RUNNING
};

SDL_Texture
	*TILEMAP_IMAGE,
	*CLOUDS_IMAGE,
	*BACKGROUND_IMAGE;
float player_vel_y = 0.f, player_acc_y = 0.f;
std::vector<bool> tiles_is_collision; // Lazy mod
const float
	TILE_WIDTH = 500.f / 16.f,
	TILE_HEIGHT = 500.f / 16.f;
const int
	MAP_TILE_WIDTH = 32,
	MAP_TILE_HEIGHT = 16,
	MAP_TILE[MAP_TILE_HEIGHT][MAP_TILE_WIDTH] =
	{
		{0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 },
		{0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 },
		{0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 },
		{0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 },
		{0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 },
		{0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 },
		{0 ,25,25,0 ,0, 0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 },
		{1 ,2 ,2 ,2 ,3 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 },
		{10,10,10,10,11,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,27,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 },
		{10,10,10,10,11,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,1 ,2 ,0 ,0 ,0 ,0 ,0 ,0 ,26,1 },
		{10,10,10,10,11,0 ,26,0 ,27,0 ,0 ,0 ,0 ,26,0 ,27,0 ,0 ,0 ,26,0 ,1 ,10,11,0 ,27,0 ,0 ,0 ,27,1 ,10},
		{10,10,10,10,10,2 ,2 ,2 ,2 ,2 ,3 ,0 ,1 ,2 ,2 ,2 ,3 ,0 ,1 ,2 ,2 ,10,10,10,2 ,3 ,0 ,27,0 ,1 ,10,10},
		{10,10,10,10,10,10,10,10,10,10,11,0 ,9 ,10,10,10,11,0 ,10,10,10,10,10,10,10,10,2 ,2 ,2 ,10,10,10},
		{10,10,10,10,10,10,10,10,10,10,11,0 ,9 ,10,10,10,11,0 ,10,10,10,10,10,10,10,10,10,10,10,10,10,10},
		{10,10,10,10,10,10,10,10,10,10,11,0 ,9 ,10,10,10,11,0 ,10,10,10,10,10,10,10,10,10,10,10,10,10,10},
		{13,13,13,13,13,13,13,13,13,13,13,0 ,13,13,13,13,13,0 ,13,13,13,13,13,13,13,13,13,13,13,13,13,13}
	};

void load_textures(SDL_Renderer *renderer) noexcept
{
	if(!(TILEMAP_IMAGE = IMG_LoadTexture(renderer, "assets/tileset.png")))
		std::cerr << "IMG_LoadTexture: " << IMG_GetError() << "\n";
	if(!(CLOUDS_IMAGE = IMG_LoadTexture(renderer, "assets/clouds.png")))
		std::cerr << "IMG_LoadTexture: " << IMG_GetError() << "\n";
	if(!(BACKGROUND_IMAGE = IMG_LoadTexture(renderer, "assets/background.png")))
		std::cerr << "IMG_LoadTexture: " << IMG_GetError() << "\n";
}
void destroy_textures() noexcept
{
	SDL_DestroyTexture(TILEMAP_IMAGE);
}

std::vector<SDL_FRect> getTilesHit(SDL_FRect &player, std::vector<std::pair<SDL_FRect, SDL_Rect>> tiles) noexcept
{
	std::vector<SDL_FRect> tiles_hit;
	int i = 0;
	for(auto tile : tiles)
	{
		if(SDL_HasIntersectionF(&tile.first, &player) && tiles_is_collision[i])
			tiles_hit.push_back(tile.first);
		i++;
	}
	return tiles_hit;
}
void collision(SDL_FRect &player, std::vector<std::pair<SDL_FRect, SDL_Rect>> tiles, float movement[2]) noexcept
{
	player.x += movement[0];

	auto tiles_hit = getTilesHit(player, tiles);
	for(auto &tile : tiles_hit)
  {
    if(movement[0] > 0)
    {
      player.x = tile.x - player.w;
    }
    if(movement[0] < 0)
    {
      player.x = tile.x + tile.w;
    }
  }
	player.y += movement[1];
	tiles_hit = getTilesHit(player, tiles);
	for(auto &tile : tiles_hit)
  {
    if(movement[1] > 0)
    {
      player.y = tile.y - player.h;
      player_vel_y = 0;
      player_acc_y = 0;
    }
    if(movement[1] < 0)
    {
      player.y = tile.y + tile.h;
    }
  }
}

int main(void)
{
	std::srand(std::time(NULL));
	if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		std::cerr << "Error<SDL_Init>: " << SDL_GetError() << "\n";
		return EXIT_FAILURE;
	}
	if(IMG_Init(IMG_INIT_PNG) < 0)
	{
		std::cerr << "Error<IMG_Init>: " << IMG_GetError() << "\n";
		return EXIT_FAILURE;
	}

	SDL_Window *window = SDL_CreateWindow("SDL2 - Planet Explorer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 500, 500, SDL_WINDOW_SHOWN);
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
	SDL_Event event;
	GameStatus game_status = window && renderer ? GameStatus::RUNNING : GameStatus::EXIT;

	std::vector<std::pair<SDL_FRect, SDL_Rect>>
		tiles, clouds;

	SDL_FRect background = {0.f, 0.f, 550.f, 550.f};
	for(int i = 0; i < 20; i++)
	{
		int scale = std::rand() % 5 + 1;
		clouds.push_back(std::make_pair(
			SDL_FRect {(float) (std::rand() % 800 + (-100)), (float) (std::rand() % 100), 32.f * (float) scale, 16.f * (float) scale},
			(std::rand() % 2) == 0 ?
				SDL_Rect {0, 0, 32, 16}
			:
				SDL_Rect {0, 16, 32, 16}
		));
	}

	load_textures(renderer);

	SDL_FRect player = {0.f, 0.f, 8.f * 3, 24.f * 3};

	for(int y = 0; y < MAP_TILE_HEIGHT; y++)
	{
		for(int x = 0; x < MAP_TILE_WIDTH; x++)
		{
			int id = MAP_TILE[y][x];
			if(id-- != 0)
			{
				tiles.push_back(std::make_pair(
					SDL_FRect {x * TILE_WIDTH, y * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT},
					SDL_Rect {id % 8 * 4, (int) std::floor(id / 8) * 4, 4, 4}
				));
				if(id >= 24 && id <= 28)
					tiles_is_collision.push_back(false);
				else
					tiles_is_collision.push_back(true);
			}
		}
	}
	int mouse_position[2];
	float scroll[2] = {0.f, 0.f};
	bool player_left = false, player_right = false;

	while(game_status == GameStatus::RUNNING)
	{
		SDL_GetMouseState(&mouse_position[0], &mouse_position[1]);

		scroll[0] += (500.f / 2.f - player.x - scroll[0]) / 10.f;
		scroll[1] += (500.f / 2.f - player.y - scroll[1]) / 10.f;

		if(scroll[0] >= 0) scroll[0] = 0;
		if(scroll[1] <= 0) scroll[1] = 0;

		while(SDL_PollEvent(&event))
		{
			if(event.type == SDL_QUIT)
				game_status = GameStatus::EXIT;
			if(event.type == SDL_KEYDOWN)
			{
				if(event.key.keysym.sym == SDLK_a)
					player_left = true;
				if(event.key.keysym.sym == SDLK_d)
					player_right = true;
				if(event.key.keysym.sym == SDLK_SPACE)
				{
					player_acc_y = 0.f;
					player_vel_y = -10.f;
				}
			}
			if(event.type == SDL_KEYUP)
			{
				if(event.key.keysym.sym == SDLK_a)
					player_left = false;
				if(event.key.keysym.sym == SDLK_d)
					player_right = false;
			}
			
		}
		SDL_SetRenderDrawColor(renderer, 140, 255, 230, 255);
		SDL_RenderClear(renderer);

		player_vel_y += player_vel_y > 10.f ? 0 : (player_acc_y += 0.08f);

		float player_movement[2] = {
			player_left ? -4.f : player_right ? 4.f : 0.f,
			player_vel_y
		};	
		collision(player, tiles, player_movement);


		SDL_FRect b = background;
		
		b.x += scroll[0] / 50.f;
		b.y += scroll[1] / 50.f;

		SDL_RenderCopyF(renderer, BACKGROUND_IMAGE, NULL, &b);
		int c_i = 0;
		for(auto cloud : clouds)
		{
			cloud.first.x += scroll[0] / 10.f;
			cloud.first.y += scroll[1] / 10.f;

			clouds[c_i].first.x -= (std::rand() % 100) == 3 ? 1.f : 0.5f;
			if(clouds[c_i].first.x < -clouds[c_i].first.w * 2)
				clouds[c_i].first.x = std::rand() % 700 + 500;

			SDL_RenderCopyF(renderer, CLOUDS_IMAGE, &cloud.second, &cloud.first);
			c_i++;
		}
		for(auto tile : tiles)
		{
			tile.first.x += scroll[0];
			tile.first.y += scroll[1];

			SDL_RenderCopyF(renderer, TILEMAP_IMAGE, &tile.second, &tile.first);
		}

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

		SDL_FRect player_scrolled = player;

		player_scrolled.x += scroll[0];
		player_scrolled.y += scroll[1];

		SDL_RenderFillRectF(renderer, &player_scrolled);

		SDL_RenderPresent(renderer);
	}

	destroy_textures();

	IMG_Quit();
	SDL_Quit();

  return EXIT_SUCCESS;
}
