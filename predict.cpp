
#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

// Override base class with your custom functionality
struct object{
	int xPos;
	uint32_t yPos;
	int id;
	int ySize;
	int xSize;
};

struct dynObj
{
	float xSpeed;
	float xPos;
	uint32_t yPos;
	int health;
	int ySize;
	int xSize;
};

class Example : public olc::PixelGameEngine
{
public:
	Example()
	{
		// Name your application
		sAppName = "Example";
	}

private:
	uint8_t gameState = 1;
	int gameOverFade = 0;
	const uint8_t gameOverFadeMax = 150;
	
	std::string textStrings[7] = {"", "Start with Enter", "Avoid spikes with A and D", "Collect Hearts", "Teleport with W, S and", "-replace by Score-", "Space for using it"};

	//Player
	olc::Sprite* playerSprite = nullptr;
	dynObj player;


	//Global Y movment
	uint32_t globalY = 0;
	bool newYScore = false;
	float tempGlobalY = 0;
	float globalYSpeed;
	float globalYSpeedOff;
	const uint8_t camYPan = 75;
	float camYPanTemp = 0;

	//Spawn
	int spawnTimer = 0;
	const int ySpriteSize = 32;
	const int xSpriteSize = 32;

	//World
	float damageCooldown = 0;
	const uint8_t damageCooldownSpeed = 50;
	float portalSelectSlowMo = 0;
	const uint8_t slowMoCooldown = 110;

	olc::Sprite* spikesSprite = nullptr;
	olc::Sprite* heartSprite = nullptr;

	olc::Sprite* portalSprite = nullptr;
	olc::Sprite* aktivPortalSprite = nullptr;
	int selectedPortal = 0;// like size() -1
	int numberOfPortals = 0;
	bool usePortal = false;

	std::vector<object> world;

	// Fix "all" bugs X

	// "physiks" X

	// launch ! 

public:
	bool OnUserCreate() override
	{
		// Called once at the start, so create things here

		playerSprite = new olc::Sprite("player.png");
		spikesSprite = new olc::Sprite("spikes.png");
		heartSprite = new olc::Sprite("heart.png");
		portalSprite = new olc::Sprite("timePortel.png");
		aktivPortalSprite = new olc::Sprite("portalAktiv.png");

		newWorld();
		
		return true;
	}

	void newWorld()
	{
		std::srand(time(0));
		gameState = 1;
		globalY = 0;
		globalYSpeed = 50;
		globalYSpeedOff = 0.5;
		player = {300, float(ScreenWidth() / 2), 40, 2, 16, 16};
		world = {{100, 5, -1}, {20, 130, 0, 16, 32}, {20, 95, 2}, {220, 130, 0, 16, 32}, {40, 80, -2}, {150, 140, 1, 32, 32}, {10, 170, -3}, {200,210,2}, {10, 210, -4},  {10, 230, -6}};
		for (int i = 0; i < ScreenWidth() / xSpriteSize; i++)
		{
			world.push_back({i * xSpriteSize, 185, 0, 16, 32});
		}
		selectedPortal = 1;
		damageCooldown = 0;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		switch (gameState)
		{
			case 0: pause(); break;
			case 1: start(); //Menu
			case 2: gameArea(fElapsedTime);break; //Game
			case 3: gameOver(); break; //GameOver
			case 6: won(); break;
		}
		return true;
	}

	void won()
	{
		if (gameOverFade < gameOverFadeMax * 10)
		{
			
			gameOverFade++;
		}
		else
		{
			
			gameOverFade = 0;
			FillRect(0,0, ScreenWidth(), ScreenHeight(), olc::Pixel(std::rand()%255, std::rand()%255, std::rand()%255, 255));
		}
		DrawString( 35, 50, "!!!You won!!!", olc::WHITE, 2);
		DrawString( 30, 80, "Your score is", olc::WHITE, 2);
		DrawString( 50, 120, "!UINT32_MAX!", olc::WHITE, 2);
		DrawString(35, 150, "Press Enter", olc::WHITE, 2);
		DrawString( 30, 200, "I hope you cheeted", olc::WHITE, 1);
		if(GetKey(olc::Key::ENTER).bReleased)
		{
			newWorld();
		}
	}

	void pause()
	{
		darkFaid();
		gameOverFade++;
		DrawString(30, 50, "Esc to resume", olc::WHITE, 2);
		DrawString(30, 80, "score : " + std::to_string(globalY), olc::WHITE, 2);
		if(GetKey(olc::Key::ESCAPE).bReleased){gameState = 2;}
	}

	void start()
	{
		if(GetKey(olc::Key::ENTER).bReleased){gameState = 2;}
	}

	void gameArea(float fElapsedTime)
	{
		if(GetKey(olc::Key::ESCAPE).bReleased){gameState = 0;};
		gameOverFade = 0;

		Clear(olc::WHITE);
		//X Axis
		if(GetKey(olc::Key::A).bHeld){ player.xPos -= player.xSpeed * fElapsedTime;}
		if(GetKey(olc::Key::D).bHeld){ player.xPos += player.xSpeed * fElapsedTime; }
		if(GetKey(olc::Key::W).bReleased){ selectedPortal--; portalSelectSlowMo = slowMoCooldown;}
		if(GetKey(olc::Key::S).bReleased){ selectedPortal++; portalSelectSlowMo = slowMoCooldown;}
		if(GetKey(olc::Key::SPACE).bPressed){ usePortal = true;}
		//X Bounds
		if ( player.xPos  <= 0) { player.xPos  = 0;}
		if ( player.xPos  >= ScreenWidth() - player.xSize) { player.xPos  = ScreenWidth() - player.xSize;}
		//Portal Bounds
		if ( selectedPortal <= 0) { selectedPortal  = 1;}
		if ( selectedPortal > numberOfPortals) { selectedPortal = numberOfPortals;}

		SetPixelMode(olc::Pixel::ALPHA);

		
		if(portalSelectSlowMo > 0)
		{
			portalSelectSlowMo -= slowMoCooldown * fElapsedTime;
			fElapsedTime *= (slowMoCooldown - portalSelectSlowMo) / slowMoCooldown;
		}
		globalYSpeed += globalYSpeedOff * fElapsedTime;
		tempGlobalY += globalYSpeed * fElapsedTime;

		if(player.yPos > ScreenHeight() / 4)
		{
			camYPanTemp += camYPan * fElapsedTime;
			if(camYPanTemp > 1)
			{
				player.yPos--;
				globalY++;
				camYPanTemp = 0;
			}
		}

		if(tempGlobalY >= 1 && gameState == 2)
		{
			tempGlobalY = 0;
			globalY++;
			spawnTimer++;
			phyiscsCheck(player);
			if(globalY == UINT32_MAX)
			{
				gameState = 6;
			}
		}

		if ((globalY + ScreenHeight()) % 1000 == 0)
		{
			newYScore = true;
			textStrings[5] = std::to_string(globalY + ScreenHeight());
		}
		if(spawnTimer == ySpriteSize)
		{
			spawnTimer = 0;
			updateObjs();
		}

		buildTrain();

		SetPixelBlend((64 - damageCooldown)/64);
		DrawSprite( player.xPos , player.yPos, playerSprite, 1, 1);
		SetPixelBlend(1.0);

		if(damageCooldown > 0)
		{
			damageCooldown -= damageCooldownSpeed * fElapsedTime;
		}

		if(player.health < 1)
		{
			gameState++;
			return;
		}

		for(int i = 0; i < player.health; i++)
		{
			DrawSprite( 5 + i * ySpriteSize, 5, heartSprite, 1, 1);
		}

		

		//DrawString(100, 40, "d :" + std::to_string(globalY) +";"+ std::to_string(int(000)), olc::GREEN, 2);
		// Called once per frame, draws random coloured pixels
		/*for (int x = 0; x < ScreenWidth(); x++)
			for (int y = 0; y < ScreenHeight(); y++)
				Draw(x, y, olc::Pixel(rand() % 256, rand() % 256, rand() % 256));*/
	}

	void gameOver()
	{
		darkFaid(); 
		
		DrawString( 30, 50, "score : " + std::to_string(globalY), olc::WHITE, 2);
		DrawString( 35, 80, "Play again ? ", olc::WHITE, 2);
		DrawString(35, 120, "Press Enter", olc::WHITE, 2);
		if(GetKey(olc::Key::ENTER).bReleased)
		{
			newWorld();
		}
	}

	void darkFaid()
	{
		if (gameOverFade < gameOverFadeMax)
		{
			FillRect(0,0, ScreenWidth(), ScreenHeight(), olc::Pixel(0, 0, 0, 1));
		}
		gameOverFade++;
	}

	int phyiscsCheck(dynObj dynObj)
	{
		int i = 0;
		for(object worldObj : world)
		{
			//DrawRect(worldObj.xPos, worldObj.yPos - globalY, worldObj.xSize, worldObj.ySize, olc::GREEN);
			if(worldObj.yPos + worldObj.ySize - globalY > dynObj.yPos && worldObj.yPos - globalY < dynObj.yPos + dynObj.ySize)
			{
				if(worldObj.xPos + worldObj.xSize > dynObj.xPos && worldObj.xPos < dynObj.xPos + dynObj.xSize)
				{
					int phyState = worldObj.id;
					if(damageCooldown > 0)
					{
						if(phyState == 0)
						{
							phyState = -1;
						}
					}

					switch(phyState)
					{
						case 0: player.health--; damageCooldown = ySpriteSize * 1;break;
						case 1: player.health++; world.erase(world.begin() + i);break;
					}
					if(player.health > 8)
					{
						player.health = 8;
					}
				}
			}
			i++;
		}
		return -1;
	}

	void buildTrain()
	{
		int curPortalIdx = 0;
		int curPortal = 0;
		numberOfPortals = 0;
		for(object obj : world)
		{
			switch (obj.id)
			{
				case 0: DrawSprite( obj.xPos, obj.yPos - globalY, spikesSprite, 1, 1);break;
				case 1: DrawSprite( obj.xPos, obj.yPos - globalY, heartSprite, 1, 1);break;
				case 2: drawPortals(curPortalIdx, &curPortal); break;
				default: DrawString(obj.xPos, obj.yPos - globalY, textStrings[obj.id * -1], olc::BLACK, 1); break;
			}
			//DrawString(obj.xPos, obj.yPos - globalY, ":" + std::to_string(curPortalIdx), olc::GREEN, 1);
			curPortalIdx++;
		}
		if(usePortal == true && numberOfPortals > 0)
		{
			std::cout << curPortal;
			object portal = world.at(curPortal);
			player.xPos = portal.xPos + player.xSize / 2; player.yPos = portal.yPos - globalY + player.ySize / 2;
			world.erase(world.begin() + curPortal);
			usePortal = false;
		}
		//DrawString(100, 100, "::" + std::to_string(curPortal), olc::GREEN, 1);
	}

	void drawPortals(int curPortalIdx, int* curPortal)
	{
		object obj = world.at(curPortalIdx);
		numberOfPortals++;
		if(numberOfPortals == selectedPortal)
		{
			DrawSprite( obj.xPos, obj.yPos - globalY, aktivPortalSprite, 1, 1);
			*curPortal = curPortalIdx;
		}
		else
		{
			DrawSprite( obj.xPos, obj.yPos - globalY, portalSprite, 1, 1);
		}
	}

	void updateObjs()
	{
		bool deleting = true;
		while(deleting)
		{
			if(world.front().yPos - globalY < 0)
			{
				world.erase(world.begin());
			}
			else
			{
				deleting = false;
			}
		}
		for(int i = 0; i < world.size(); i++)
		{
			if(world[i].id == 2)
			{
				if(world[i].yPos - globalY < player.yPos)
				{
					world.erase(world.begin() + i);
					i--;
				}
			}
		}
		if(newYScore == true)
		{
			newYScore = false;
			world.push_back({ 4*xSpriteSize, ScreenHeight() + globalY, -5});
			return ;
		}
		for(int i = 0; i < xSpriteSize; i++)
		{
			int id = std::rand() % 50 -30;
			int ySize;
			int xSize;
			int ySpawnOff;

			if( id >= 0)
			{
				switch(id)
				{
					case 0:
					case 1: id = 1; ySpawnOff = 0; ySize = 32; xSize = 32;break;
					case 2: id = 2; ySpawnOff = 0; ySize = 32; xSize = 32;break;
					default: id = 0; ySpawnOff = 0; ySize = 16; xSize = 32;break;
				}

				world.push_back({ i*xSpriteSize, ScreenHeight() + globalY - ySpawnOff, id, ySize, xSize});
			}
		}
	}
};

int main()
{
	Example demo;
	if (demo.Construct(256, 256, 4, 4, false, false))
		demo.Start();
	return 0;
}
