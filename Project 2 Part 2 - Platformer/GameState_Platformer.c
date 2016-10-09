// ---------------------------------------------------------------------------
// Project Name		:	Platformer
// File Name		:	GameState_Platformer.cpp
// Author			:	Antoine Abi Chakra
// Creation Date	:	2008/03/04
// Purpose			:	Implementation of the platform game state
// History			:
//	- 09/28/2015	:	Turned into a step by step project.
// ---------------------------------------------------------------------------

/*
-------------------------------------------------------------------------- - 
Project Title : Platformer File Name : Platformer
Author : Sean Higgins
Creation Date : 2015/09/30 
Purpose : Basic platformer game
History - (10/07/2015) : Finishing touches and particle effects
*/

// ---------------------------------------------------------------------------


#include "AEEngine.h"
#include "GameStateMgr.h"
#include <sys/stat.h>
#include <string.h>
#include "Vector2D.h"
#include "Matrix2D.h"
#include "Math2D.h"
// ---------------------------------------------------------------------------

#define GAME_OBJ_NUM_MAX			32
#define GAME_OBJ_INST_NUM_MAX		2048

//Gameplay related variables and values
#define GRAVITY -20.0f
#define JUMP_VELOCITY 11.0f
#define MOVE_VELOCITY_HERO 4.0f
#define MOVE_VELOCITY_ENEMY 7.5f
#define ENEMY_IDLE_TIME 2.0
#define HERO_LIVES 3
#define HERO_SCALE 1
#define ENEMY_SCALE 1
#define COIN_SCALE 1
#define OTHER_SCALE 1
#define MAP_TRANSFORM_SCALE_X 20
#define MAP_TRANSFORM_SCALE_Y 20
#define STARTING_NUMBER_OF_COINS 10
#define PARTICLE_SCALE   0.1f
#define PARTICLE_LIFESPAN 1.5f

static int colorToggle;
static int HeroLives;
static int Hero_Initial_X;
static int Hero_Initial_Y;
static int TotalCoins;

//Flags
#define FLAG_ACTIVE			0x00000001




enum TYPE_OBJECT
{
	TYPE_OBJECT_EMPTY,			//0
	TYPE_OBJECT_COLLISION,		//1
	TYPE_OBJECT_HERO,			//2
	TYPE_OBJECT_ENEMY1,			//3
	TYPE_OBJECT_COIN,			//4
	PARTICLE_RED_CIRCLE, 
	PARTICLE_GOLD_CIRCLE, 
	PARTICLE_BLUE_SQUARE 

	
};

//State machine states
enum STATE
{
	STATE_NONE,
	STATE_GOING_LEFT,
	STATE_GOING_RIGHT
};

//State machine inner states
enum INNER_STATE
{
	INNER_STATE_ON_ENTER,
	INNER_STATE_ON_UPDATE,
	INNER_STATE_ON_EXIT
};


typedef struct
{
	unsigned int		type;		// object type
	AEGfxVertexList*	pMesh;		// pbject
}GameObj;


typedef struct
{
	GameObj*		pObject;	// pointer to the 'original'
	unsigned int	flag;		// bit flag or-ed together
	float			scale;
	Vector2D			posCurr;	// object current position
	Vector2D			velCurr;	// object current velocity
	float			dirCurr;	// object current direction

	Matrix2D			transform;	// object drawing matrix

	//Used to hold the current 
	int				gridCollisionFlag;

	// pointer to custom data specific for each object type
	void*			pUserData;

	//State of the object instance
	enum			STATE state;
	enum			INNER_STATE innerState;

	//General purpose counter (This variable will be used for the enemy state machine)
	double			counter;
}GameObjInst;


// list of original objects
static GameObj			*sGameObjList;
static unsigned int		sGameObjNum;

// list of object instances
static GameObjInst		*sGameObjInstList;
static unsigned int		sGameObjInstNum;




/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
// TO DO 1:
// -- Import these functions from part 1
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
//Binary map data
static int **MapData;
static int **BinaryCollisionArray;
static int BINARY_MAP_WIDTH;
static int BINARY_MAP_HEIGHT;
int GetCellValue(int X, int Y);
int CheckInstanceBinaryMapCollision(float PosX, float PosY, float scaleX, float scaleY);
float SnapToCell(float Coordinate);
//void SnapToCell(float *Coordinate);
int ImportMapDataFromFile(char *FileName);
void FreeMapData(void);


static Matrix2D MapTransform;


//Collision flags
#define	COLLISION_LEFT		0x00000001	//0001
#define	COLLISION_RIGHT		0x00000002	//0010
#define	COLLISION_TOP		0x00000004	//0100
#define	COLLISION_BOTTOM	0x00000008	//1000





int GetCellValue(unsigned int X, unsigned int Y)
{
	if (X >= (BINARY_MAP_HEIGHT) || Y >= BINARY_MAP_WIDTH || X < 0 || Y < 0)
	{
		return 0;
	}

	else
	{

		return BinaryCollisionArray[X][Y];
	}
}

int CheckInstanceBinaryMapCollision(float PosX, float PosY, float scaleX, float scaleY)
{
	int FLAG = 0;
	//return 0;

	//Top right, right side
	int xTRR = PosX + .5*scaleX;
	int yTRR = PosY + .25*scaleY;

	//Top right, top side
	int xTRT = PosX + .25*scaleX;
	int yTRT = PosY + .5*scaleY;

	//Bottom right, right side
	int xBRR = PosX + .5*scaleX;
	int yBRR = PosY - .25*scaleY;

	//Bottom right, bottom side
	int xBRB = PosX + .25*scaleX;
	int yBRB = PosY - .5*scaleY;

	//Bottom left, bottom side
	int xBLB = PosX - .25*scaleX;
	int yBLB = PosY - .5*scaleY;

	//Bottom left, left side
	int xBLL = PosX - .5*scaleX;
	int yBLL = PosY - .25*scaleY;

	//Top left, left side
	int xTLL = PosX - .5*scaleX;
	int yTLL = PosY + .25*scaleY;

	//Top left, top side
	int xTLT = PosX - .25*scaleX;
	int yTLT = PosY + .5*scaleY;


	if (1 == GetCellValue(xTRR, yTRR) || (1 == GetCellValue(xBRR, yBRR)))
	{
		FLAG = FLAG | COLLISION_RIGHT;
	}

	if (1 == GetCellValue(xTRT, yTRT) || (1 == GetCellValue(xTLT, yTLT)))
	{
		FLAG = FLAG | COLLISION_TOP;
	}
	if (1 == GetCellValue(xBRB, yBRB) || (1 == GetCellValue(xBLB, yBLB)))
	{
		FLAG = FLAG | COLLISION_BOTTOM;
	}
	if (1 == GetCellValue(xTLL, yTLL) || (1 == GetCellValue(xBLL, yBLL)))
	{
		FLAG = FLAG | COLLISION_LEFT;
	}

	return FLAG;



}



off_t fsize(const char *filename) {
	struct stat st;

	if (stat(filename, &st) == 0)
		return st.st_size;

	return -1;
}
/*void SnapToCell(float *Coordinate)
{
	float temp = *Coordinate;
	float t = (((int)(temp)) + 0.5f);

	*Coordinate = t;
}
*/

float SnapToCell(float Coordinate)
{
	float temp = Coordinate;
	float t = (((int)(temp)) + 0.5f);

	return t;
}



int ImportMapDataFromFile(char *FileName)
{
	char * buffer = ""; 
	char * heightString = "";
	char * widthString = "";
	char * trash = "";

	int breaker = 0;
	//int trash = 0;
	int i = 6;


	printf("Opening file %s \n", FileName);

	FILE * input = fopen(FileName, "r");
	//Get to the 6th character of the 1st line by reading in 6 chars via fread.

	//fread(trash, 1, 6, input);



	int fileSize = fsize(FileName);
	printf("file size:  %i \n", fileSize);
	buffer = (char *)malloc(fileSize);

	int temp = 0;
	temp = fread(buffer, 1, fileSize, input);

	printf("num chars read from file:  %i\n", temp);
	printf("my string:  %s \n", buffer);

	//fwrite(trash, 1, temp, buffer);

	//printf("%c \n", buffer[6]);

	trash = strtok(buffer, " \n");

	for (int i = 1; i < 4 && trash != NULL; i++)
	{
		trash = strtok(NULL, " \n");
		printf("trash val:  %s\n", trash);
		if (i == 1)
		{
			BINARY_MAP_WIDTH = strtol(trash, NULL, 10);
		}

		if (i == 3)
		{
			BINARY_MAP_HEIGHT = strtol(trash, NULL, 10);
		}

	}

	printf("Width, Height:  (%i, %i)\n", BINARY_MAP_WIDTH, BINARY_MAP_HEIGHT);





	MapData = (int **)malloc(BINARY_MAP_HEIGHT * sizeof(int*));
	BinaryCollisionArray = (int **)malloc(BINARY_MAP_HEIGHT * sizeof(int*));
	for (i = 0; i<BINARY_MAP_HEIGHT; i++)
	{

		MapData[i] = (int *)(malloc(BINARY_MAP_WIDTH*sizeof(int)));
		BinaryCollisionArray[i] = (int *)(malloc(BINARY_MAP_WIDTH*sizeof(int)));

	}

	int holdThis;
	for (int h = BINARY_MAP_WIDTH - 1; h >= 0; h--)
	{
		for (int w = 0; w< BINARY_MAP_HEIGHT; w++)
		{
			trash = strtok(NULL, " \n");
			holdThis = strtol(trash, NULL, 10);

			//printf("Error checking:  value of holdThis:  %i\n", holdThis);


			MapData[w][h] = holdThis;
			if (holdThis == 1)
			{
				BinaryCollisionArray[w][h] = 1;
			}
			else
			{
				BinaryCollisionArray[w][h] = 0;
			}



			//printf("Error checking:  value of Binary matrix at (%i, %i) = %i\n", w, h, BinaryCollisionArray[w][h]);

		}
	}


	/*
	for (i = 0; i < BINARY_MAP_WIDTH; i++)
	{
	for (int j = 0; j < BINARY_MAP_HEIGHT; j++)
	{
	if (0 == MapData[i][j])
	{
	BinaryCollisionArray[i][j] = 0;
	}
	else
	{
	BinaryCollisionArray[i][j] = 1;
	}

	}
	}
	*/



	return 1;
}

void FreeMapData(void)
{
	for (int i = 0; i < BINARY_MAP_WIDTH; i++)
	{
		free(MapData[i]);
	}
	free(MapData);
}

void PrintRetrievedInformation(void)
{
	int i, j;

	printf("Width: %i\n", BINARY_MAP_WIDTH);
	printf("Height: %i\n", BINARY_MAP_HEIGHT);
	/*
	printf("Map Data:\n");
	for (i = 0; i < BINARY_MAP_WIDTH; i++)
	{
	for (j = 0; j < BINARY_MAP_HEIGHT; j++)
	{
	printf("%i ", MapData[i][j]);
	}
	printf("\n");
	}

	printf("\n\nBinary Collision Data:\n");
	for (i = 0; i < BINARY_MAP_WIDTH; i++)
	{
	for (j = 0; j < BINARY_MAP_HEIGHT; j++)
	{
	printf("%i ",BinaryCollisionArray[i][j]);
	}
	printf("\n");
	}*/

	for (j = BINARY_MAP_HEIGHT - 1; j >= 0; --j)
	{
		for (i = 0; i < BINARY_MAP_WIDTH; ++i)
		{
			printf("%i ", MapData[i][j]);
		}

		printf("\n");
	}

	printf("\n\nBinary Collision Data:\n");
	for (j = BINARY_MAP_HEIGHT - 1; j >= 0; --j)
	{
		for (i = 0; i < BINARY_MAP_WIDTH; ++i)
		{
			printf("%i ", BinaryCollisionArray[i][j]);
		}

		printf("\n");
	}


}




// function to create/destroy a game object instance
static GameObjInst*	gameObjInstCreate (unsigned int type, float scale, Vector2D* pPos, Vector2D* pVel, float dir, enum STATE startState);
static void			gameObjInstDestroy(GameObjInst* pInst);

//We need a pointer to the hero's instance for input purposes
static GameObjInst *pHero;

//State machine functions
void EnemyStateMachine(GameObjInst *pInst);


void GameStatePlatformLoad(void)
{
	// USed for the cicle shape
	float CircleAngleStep, i;
	int Parts;
	GameObj* pObj;


	sGameObjList = (GameObj *)calloc(GAME_OBJ_NUM_MAX, sizeof(GameObj));
	sGameObjInstList = (GameObjInst *)calloc(GAME_OBJ_INST_NUM_MAX, sizeof(GameObjInst));
	sGameObjNum = 0;



	//Creating the black object
	pObj		= sGameObjList + sGameObjNum++;
	pObj->type	= TYPE_OBJECT_EMPTY;

	AEGfxMeshStart();

	//1st argument: X
	//2nd argument: Y
	//3rd argument: ARGB
	AEGfxTriAdd(
		-0.5f, -0.5f, 0xFF000000, 0.0f, 0.0f,
		 0.5f,  -0.5f, 0xFF000000, 0.0f, 0.0f, 
		-0.5f,  0.5f, 0xFF000000, 0.0f, 0.0f);
	
	AEGfxTriAdd(
		-0.5f, 0.5f, 0xFF000000, 0.0f, 0.0f, 
		 0.5f,  -0.5f, 0xFF000000, 0.0f, 0.0f, 
		0.5f,  0.5f, 0xFF000000, 0.0f, 0.0f);

	pObj->pMesh = AEGfxMeshEnd();

		
	
	//Creating the white object
	pObj		= sGameObjList + sGameObjNum++;
	pObj->type	= TYPE_OBJECT_COLLISION;

	AEGfxMeshStart();

	//1st argument: X
	//2nd argument: Y
	//3rd argument: ARGB
	AEGfxTriAdd(
		-0.5f, -0.5f, 0xFFFFFFFF, 0.0f, 0.0f, 
		 0.5f,  -0.5f, 0xFFFFFFFF, 0.0f, 0.0f, 
		-0.5f,  0.5f, 0xFFFFFFFF, 0.0f, 0.0f);
	
	AEGfxTriAdd(
		-0.5f, 0.5f, 0xFFFFFFFF, 0.0f, 0.0f, 
		 0.5f,  -0.5f, 0xFFFFFFFF, 0.0f, 0.0f, 
		0.5f,  0.5f, 0xFFFFFFFF, 0.0f, 0.0f);

	pObj->pMesh = AEGfxMeshEnd();


	//Creating the hero object
	pObj		= sGameObjList + sGameObjNum++;
	pObj->type	= TYPE_OBJECT_HERO;

	AEGfxMeshStart();

	//1st argument: X
	//2nd argument: Y
	//3rd argument: ARGB
	AEGfxTriAdd(
		-0.5f, -0.5f, 0xFF0000FF, 0.0f, 0.0f, 
		 0.5f,  -0.5f, 0xFF0000FF, 0.0f, 0.0f, 
		-0.5f,  0.5f, 0xFF0000FF, 0.0f, 0.0f);
	
	AEGfxTriAdd(
		-0.5f, 0.5f, 0xFF0000FF, 0.0f, 0.0f, 
		 0.5f,  -0.5f, 0xFF0000FF, 0.0f, 0.0f, 
		0.5f,  0.5f, 0xFF0000FF, 0.0f, 0.0f);

	pObj->pMesh = AEGfxMeshEnd();


	//Creating the enemey1 object
	pObj		= sGameObjList + sGameObjNum++;
	pObj->type	= TYPE_OBJECT_ENEMY1;

	AEGfxMeshStart();

	//1st argument: X
	//2nd argument: Y
	//3rd argument: ARGB
	AEGfxTriAdd(
		-0.5f, -0.5f, 0xFFFF0000, 0.0f, 0.0f, 
		 0.5f,  -0.5f, 0xFFFF0000, 0.0f, 0.0f, 
		-0.5f,  0.5f, 0xFFFF0000, 0.0f, 0.0f);
	
	AEGfxTriAdd(
		-0.5f, 0.5f, 0xFFFF0000, 0.0f, 0.0f, 
		 0.5f,  -0.5f, 0xFFFF0000, 0.0f, 0.0f, 
		0.5f,  0.5f, 0xFFFF0000, 0.0f, 0.0f);

	pObj->pMesh = AEGfxMeshEnd();


	//Creating the Coin object
	pObj		= sGameObjList + sGameObjNum++;
	pObj->type	= TYPE_OBJECT_COIN;

	AEGfxMeshStart();

	//1st argument: X
	//2nd argument: Y
	//3rd argument: ARGB

	//Creating the circle shape
	CircleAngleStep = PI/12.0f;
	Parts = 12;
	for(i = 0; i < Parts; ++i)
	{
		AEGfxTriAdd(
		0.0f, 0.0f, 0xFFFFFF00, 0.0f, 0.0f, 
		cosf(i*2*PI/Parts)*0.5f,  sinf(i*2*PI/Parts)*0.5f, 0xFFFFFF00, 0.0f, 0.0f, 
		cosf((i+1)*2*PI/Parts)*0.5f,  sinf((i+1)*2*PI/Parts)*0.5f, 0xFFFFFF00, 0.0f, 0.0f);
	}

	pObj->pMesh = AEGfxMeshEnd();




	//Creating Particle Effect Objects -- SH

	//Red Circle

	pObj = sGameObjList + sGameObjNum++;
	pObj->type = PARTICLE_RED_CIRCLE;

	AEGfxMeshStart();

	//1st argument: X
	//2nd argument: Y
	//3rd argument: ARGB


	CircleAngleStep = PI / 12.0f;
	Parts = 12;
	for (i = 0; i < Parts; ++i)
	{
		AEGfxTriAdd(
			0.0f, 0.0f, 0xFFFF0000, 0.0f, 0.0f,
			cosf(i * 2 * PI / Parts)*0.5f, sinf(i * 2 * PI / Parts)*0.5f, 0xFFFFFF00, 0.0f, 0.0f,
			cosf((i + 1) * 2 * PI / Parts)*0.5f, sinf((i + 1) * 2 * PI / Parts)*0.5f, 0xFFFFFF00, 0.0f, 0.0f);
	}

	pObj->pMesh = AEGfxMeshEnd();


	//Gold circle

	pObj = sGameObjList + sGameObjNum++;
	pObj->type = PARTICLE_GOLD_CIRCLE;

	CircleAngleStep = PI / 12.0f;
	Parts = 12;
	for (i = 0; i < Parts; ++i)
	{
		AEGfxTriAdd(
			0.0f, 0.0f, 0xFFFFFF00, 0.0f, 0.0f,
			cosf(i * 2 * PI / Parts)*0.5f, sinf(i * 2 * PI / Parts)*0.5f, 0xFFFFFF00, 0.0f, 0.0f,
			cosf((i + 1) * 2 * PI / Parts)*0.5f, sinf((i + 1) * 2 * PI / Parts)*0.5f, 0xFFFFFF00, 0.0f, 0.0f);
	}

	pObj->pMesh = AEGfxMeshEnd();


	pObj = sGameObjList + sGameObjNum++;
	pObj->type = PARTICLE_BLUE_SQUARE;
	AEGfxTriAdd(
		-0.5f, -0.5f, 0xFF0000FF, 0.0f, 0.0f,
		0.5f, -0.5f, 0xFF0000FF, 0.0f, 0.0f,
		-0.5f, 0.5f, 0xFF0000FF, 0.0f, 0.0f);

	AEGfxTriAdd(
		-0.5f, 0.5f, 0xFF0000FF, 0.0f, 0.0f,
		0.5f, -0.5f, 0xFF0000FF, 0.0f, 0.0f,
		0.5f, 0.5f, 0xFF0000FF, 0.0f, 0.0f);

	pObj->pMesh = AEGfxMeshEnd();



	//Setting intital binary map values
	MapData = 0;
	BinaryCollisionArray = 0;
	BINARY_MAP_WIDTH = 0;
	BINARY_MAP_HEIGHT = 0;

	//Importing Data
	if (!ImportMapDataFromFile("Exported.txt"))
		gGameStateNext = GS_QUIT;


	/////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////
	// TO DO 3:
	// -- Implement the map transform matrix
	//    A map translation and a scale are required. 
	// -- Store your computed map transformation in "MapTransform"
	/////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////
	Matrix2D scale, translate;
	
	
		Matrix2DScale(&scale, MAP_TRANSFORM_SCALE_X, MAP_TRANSFORM_SCALE_Y);
		Matrix2DTranslate(&translate, BINARY_MAP_WIDTH/(-2), BINARY_MAP_HEIGHT/(-2));
		Matrix2DConcat(&MapTransform, &scale, &translate);
		//Matrix2DConcat(&MapTransform, &translate, &scale);
}

void GameStatePlatformInit(void)
{
	int i, j;
	GameObjInst *pInst;
	Vector2D Pos;

	pHero = 0;
	TotalCoins = 0;

	//Setting the inital number of hero lives
	HeroLives = HERO_LIVES;

	/////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////
	// TO DO 2:
	// -- Create the map tiles.
	//    Loop through the loaded map array, and create a black instance (non collision) or white 
	//    instance (collision).
	// -- Create the objects instances (Hero, enemies & coins).
	// -- If implemented correctly, you should see the map and instances. (They'll be really small at
	//    this point
	/////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////
	// Creating the map, the main character, the enemies and the coins according to their initial positions in MapData

	/***********
	Loop through all the array elements of MapData (which was initialized in the "GameStatePlatformLoad" function
	from the .txt file
		
		 - Create a white or black cell

		 - if the element represents the hero
			Create a hero instance
			Set its position depending on its array indices in MapData
			Save its array indices in Hero_Initial_X and Hero_Initial_Y (Used when the hero dies and its position needs to be reset)

		 - if the element represents an enemy
			Create an enemy instance
			Set its position depending on its array indices in MapData
			
		 - if the element represents a coin
			Create a coin instance
			Set its position depending on its array indices in MapData



			
	***********/
	for(i = 0; i < BINARY_MAP_WIDTH; ++i)
		for (j = 0; j < BINARY_MAP_HEIGHT; ++j)
		{
			GameObjInst * t;
			Vector2D pos, posA, vel;
			//Vector2DSet(&pos, i, j);
			Vector2DSet(&pos, i + .5, j + .5);
			Vector2DSet(&vel, 0, 0);
			//	Vector2DSet(&posA, i + .5, j + .5);

			if (BinaryCollisionArray[i][j] == TYPE_OBJECT_EMPTY)
			{

				t = gameObjInstCreate(TYPE_OBJECT_EMPTY, OTHER_SCALE, &pos, &vel, 0, STATE_NONE);

			}

			else if (BinaryCollisionArray[i][j] == TYPE_OBJECT_COLLISION)
			{
				t = gameObjInstCreate(TYPE_OBJECT_COLLISION, OTHER_SCALE, &pos, &vel, 0, STATE_NONE);
			}


		}


	for (i = 0; i < BINARY_MAP_WIDTH; i++)
		for (j = 0; j < BINARY_MAP_WIDTH; j++)
		{ 
			
			GameObjInst * t;
			Vector2D pos, posA, vel;
			//Vector2DSet(&pos, i, j);
			Vector2DSet(&pos, i + .5, j + .5);
			Vector2DSet(&vel, 0, 0);
			//	Vector2DSet(&posA, i + .5, j + .5);
			
			
			if (MapData[i][j] == TYPE_OBJECT_ENEMY1)
			{
				//Vector2DSet(&vel, MOVE_VELOCITY_ENEMY, MOVE_VELOCITY_ENEMY);
				Vector2DSet(&vel, 0, 0);
				//t=gameObjInstCreate(TYPE_OBJECT_ENEMY1, ENEMY_SCALE, &posA, &vel, 0, STATE_GOING_RIGHT);
				t = gameObjInstCreate(TYPE_OBJECT_ENEMY1, ENEMY_SCALE, &pos, &vel, 0, STATE_NONE);
			}
			else if (MapData[i][j] == TYPE_OBJECT_COIN)
			{
				t=gameObjInstCreate(TYPE_OBJECT_COIN, COIN_SCALE, &pos, &vel, 0, STATE_NONE);
			}

			else if (MapData[i][j] == TYPE_OBJECT_HERO)
			{
				//Vector2DSet(&vel, MOVE_VELOCITY_HERO, MOVE_VELOCITY_HERO);
				Vector2DSet(&vel,0, 0);

				Hero_Initial_X = i;
				Hero_Initial_Y = j;
				pHero = gameObjInstCreate(TYPE_OBJECT_HERO, HERO_SCALE, &pos, &vel, 0, STATE_NONE);
				//printf("Starting position of hero is: (%i, %i)\n", i, j);
			}

		}
}

void GameStatePlatformUpdate(void)
{
	int i, j;
	float winMaxX, winMaxY, winMinX, winMinY;
	double frameTime;
	//Vector2D vel, accel, pos;
	// ==========================================================================================
	// Getting the window's world edges (These changes whenever the camera moves or zooms in/out)
	// ==========================================================================================
	winMaxX = AEGfxGetWinMaxX();
	winMaxY = AEGfxGetWinMaxY();
	winMinX = AEGfxGetWinMinX();
	winMinY = AEGfxGetWinMinY();


	// ======================
	// Getting the frame time
	// ======================

	frameTime = AEFrameRateControllerGetFrameTime();
	//printf("Value of frameTime:  %d\n", frameTime);

	/////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////
	// TO DO 8:
	// Control the hero's movement:
	// -- If Left/Right are pressed: Set the hero velocity's X coordinate to -/+ MOVE_VELOCITY_HERO.
	//    Set it to 0 when neither are pressed.
	// -- If SPACE is pressed AND the hero is on a platform: Jump. Use "JUMP_VELOCITY" as the upward
	//    jump velocity.
	// -- You should be able to move the main character and make it jump.
	/////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////
	int temp;



	temp = CheckInstanceBinaryMapCollision(pHero->posCurr.x, pHero->posCurr.y, 1, 1);
	//printf("Temp testing:  %i \n", temp & COLLISION_BOTTOM);
	if (AEInputCheckCurr(VK_LEFT))
	{
		Vector2DSet(&(pHero->velCurr), -1 * MOVE_VELOCITY_HERO, pHero->velCurr.y);
	}
	else
	if (AEInputCheckCurr(VK_RIGHT))
	{
		Vector2DSet(&(pHero->velCurr), MOVE_VELOCITY_HERO, pHero->velCurr.y);
	}
	else
	{
		Vector2DSet(&(pHero->velCurr), 0, pHero->velCurr.y);
	}
	
	if ((AEInputCheckTriggered(VK_SPACE)||AEInputCheckTriggered(VK_UP)) && !(temp & COLLISION_BOTTOM))
	{
		//printf("Velocity should be changing if this prints, jumping pls\n");
		//Can jump on enemies if you're fast enough.  Not a bug, a 'featrue'
		Vector2DSet(&(pHero->velCurr), pHero->velCurr.x, JUMP_VELOCITY);


		//Spawn 15-30 blue particles when the hero jumps, have them fall through everything
		GameObjInst * pInst;
		float yVel, xVel;
		float numParticles = AERandFloat() * 15 + 15;
		Vector2D vel, pos;
		Vector2DSet(&pos, pHero->posCurr.x, pHero->posCurr.y - .5); //Set the position vector to the bottom middle of the hero
		for (int i = 0; i < numParticles; i++)
		{
			 yVel = AERandFloat()*JUMP_VELOCITY;
			 xVel = AERandFloat()*pHero->velCurr.y;
			if (i % 2 == 0)
			{
				Vector2DSet(&vel,xVel, yVel);
			}
			else
			{
				Vector2DSet(&vel,-1* xVel, yVel);
			}
			pInst = gameObjInstCreate(PARTICLE_BLUE_SQUARE, PARTICLE_SCALE, &pos, &vel, (360 * AERandFloat()), STATE_NONE);
			pInst->counter = PARTICLE_LIFESPAN;
		}
	}



	/////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////
	// TO DO 6:
	// Update the velocity of each active game object instances, excluding the map tiles.
	// -- velocities are updated here (V1.y = GRAVITY*t + V0.y)
	/////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////
	//Update object instances physics and behavior
	for(i = 0; i < GAME_OBJ_INST_NUM_MAX; ++i)
	{
		GameObjInst *pInst = sGameObjInstList + i;

		// skip non-active object
		if (0 == (pInst->flag & FLAG_ACTIVE))
			continue;


		if (pInst->pObject->type != TYPE_OBJECT_EMPTY && pInst->pObject->type != TYPE_OBJECT_COLLISION  && pInst->pObject->type != PARTICLE_GOLD_CIRCLE && pInst->pObject->type != PARTICLE_RED_CIRCLE)
		{
			Vector2DSet(&(pInst->velCurr), pInst->velCurr.x, (pInst->velCurr.y + GRAVITY*frameTime));
			//Vector2DSet(&(pInst->velCurr), pInst->velCurr.x, (pInst->velCurr.y + GRAVITY));

			if (pInst->pObject->type == TYPE_OBJECT_ENEMY1)
			{
				EnemyStateMachine(pInst);
			}

		}


		

/*
		else if (pInst->pObject->type == TYPE_OBJECT_ENEMY1)
		{
			Vector2DSet(&vel, (pInst->velCurr.x), (pInst->velCurr.y));
			Vector2DScale(&vel, &vel, frameTime * MOVE_VELOCITY_ENEMY);
			Vector2DSet(&pos, (pInst->posCurr.x), (pInst->velCurr.y));
			Vector2DAdd(&(pInst->posCurr), &pos, &vel);

		}
		*/
		/////////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////////
		// TO DO 10:
		// If the object instance is an enemy, update its state machine by calling the "EnemyStateMachine"
		// function.
		/////////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////////
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////
	// TO DO 5:
	// Update the positions of all active game object instances
	// -- Positions are updated here (P1 = vt + P0)
	/////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////
	for(i = 0; i < GAME_OBJ_INST_NUM_MAX; ++i)
	{
		GameObjInst *pInst = sGameObjInstList + i;

		// skip non-active object
		if (0 == (pInst->flag & FLAG_ACTIVE))
			continue;
		Vector2D vel1, pos1;
		
		

		if (pInst->pObject->type == TYPE_OBJECT_HERO || pInst->pObject->type == TYPE_OBJECT_ENEMY1 || pInst->pObject->type == PARTICLE_BLUE_SQUARE)
		{
			Vector2DSet(&vel1, (pInst->velCurr.x), (pInst->velCurr.y));
			Vector2DScale(&vel1, &vel1, frameTime);
			Vector2DSet(&pos1, (pInst->posCurr.x), (pInst->posCurr.y));
			Vector2DAdd(&(pInst->posCurr), &pos1, &vel1);
			
		}

	/*	else if (pInst->pObject->type == TYPE_OBJECT_ENEMY1)
		{
			Vector2DSet(&vel1, (pInst->velCurr.x), (pInst->velCurr.y));
			Vector2DScale(&vel1, &vel1, frameTime);
			Vector2DSet(&pos1, (pInst->posCurr.x), (pInst->posCurr.y));
			Vector2DAdd(&(pInst->posCurr), &pos1, &vel1);

		}
		*/

	}

	/////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////
	// TO DO 7:
	// -- Check for collision between active game object instances (excluding the map tiles) and the 
	//    map, by calling the “CheckInstanceBinaryMapCollision”. Store the bit field in the instance's
	//    "gridCollisionFlag" member variable.
	// -- In case of an intersection with the map, set the respective velocity coordinate to 0 and snap the 
	//    respective position coordinate.
	// -- If implemented correctly, object instances should stop falling down through the platforms.
	/////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////
	//Check for grid collision
 	for (i = 0; i < GAME_OBJ_INST_NUM_MAX; ++i)
	{
		GameObjInst *pInst = sGameObjInstList + i;

		// skip non-active object instances
		if (0 == (pInst->flag & FLAG_ACTIVE))
			continue;

		
			if (pInst->pObject->type == TYPE_OBJECT_HERO || pInst->pObject->type == TYPE_OBJECT_ENEMY1)// || pInst->pObject->type == TYPE_OBJECT_COIN)
			{
				pInst->gridCollisionFlag = 0;
				//pInst->gridCollisionFlag = CheckInstanceBinaryMapCollision(pInst->posCurr.x, pInst->posCurr.y, pInst->scale, pInst->scale);
				pInst->gridCollisionFlag = CheckInstanceBinaryMapCollision(pInst->posCurr.x, pInst->posCurr.y, 1, 1);
				//pInst->gridCollisionFlag = CheckInstanceBinaryMapCollision(pInst->posCurr.x, pInst->posCurr.y, MAP_TRANSFORM_SCALE_X, MAP_TRANSFORM_SCALE_Y);

/*
				if (pInst->pObject->type == TYPE_OBJECT_HERO)
				{
					printf("Hero position pre-snap:  (%i, %i)\n", pInst->posCurr.x, pInst->posCurr.y);
				}
				*/


				if ((pInst->gridCollisionFlag & COLLISION_BOTTOM))
				{
					pInst->velCurr.y = 0;
					pInst->posCurr.y = SnapToCell(((pInst->posCurr.y)));
				//	SnapToCell((&pInst->posCurr.x));
				}

				if ((pInst->gridCollisionFlag & COLLISION_TOP))
				{
					pInst->velCurr.y = 0;
					pInst->posCurr.y = SnapToCell(((pInst->posCurr.y)));
					//SnapToCell((&(pInst->posCurr.y)));
					//SnapToCell((&pInst->posCurr.x));
				}

				if ((pInst->gridCollisionFlag & COLLISION_LEFT))
				{
					pInst->velCurr.x = 0;
					pInst->posCurr.x = SnapToCell(((pInst->posCurr.x)));
					//SnapToCell((&(pInst->posCurr.x)));
					//SnapToCell((&pInst->posCurr.y));
				}

				if ((pInst->gridCollisionFlag & COLLISION_RIGHT))
				{
					pInst->velCurr.x = 0;
					pInst->posCurr.x = SnapToCell(((pInst->posCurr.x)));
					//SnapToCell(((pInst->posCurr.x)));
					//SnapToCell((&pInst->posCurr.y));
					//printf("");
				}
				

				/*
				if (pInst->pObject->type == TYPE_OBJECT_HERO)
				{
					printf("Current hero position:  (%i, %i)\n", pInst->posCurr.x, pInst->posCurr.y);
				}
				*/
			}

		
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////
	// TO DO 11:
	// -- Check for collision among objects instances.
	//    Hero-Enemy intersection: Rectangle-Rectangle: The hero's position should be reset to its
	//		value
	//    Hero-Coin intersection: Rectangle-Circle: The coin should be deleted.
	/////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////
	for(i = 0; i < GAME_OBJ_INST_NUM_MAX; ++i)
	{
		GameObjInst *pInst = sGameObjInstList + i;

		// skip non-active object instances
		if (0 == (pInst->flag & FLAG_ACTIVE))
			continue;

		if (pInst->pObject->type == TYPE_OBJECT_ENEMY1)
		{
			
			if (StaticRectToStaticRect(&(pInst->posCurr), pInst->scale / 2, pInst->scale / 2, &(pHero->posCurr), pHero->scale / 2, pHero->scale / 2))
			{
				printf("Hero has lost a life!  Git gud.\n");
				Vector2DSet(&(pHero->posCurr), Hero_Initial_X, Hero_Initial_Y);

			}

		}

		else if (pInst->pObject->type == TYPE_OBJECT_COIN)
		{
			if (StaticCircleToStaticRectangle(&(pInst->posCurr), pInst->scale / 2, &(pHero->posCurr), pHero->scale / 2, pHero->scale / 2))
			{
				printf("Hero has gotten a coin!\n");
				TotalCoins++;
				/*	
				CircleAngleStep = PI / 12.0f;
				Parts = 12;
				for (i = 0; i < Parts; ++i)
				{
				AEGfxTriAdd(
			0.0f, 0.0f, 0xFFFFFF00, 0.0f, 0.0f,
			cosf(i * 2 * PI / Parts)*0.5f, sinf(i * 2 * PI / Parts)*0.5f, 0xFFFFFF00, 0.0f, 0.0f,
			cosf((i + 1) * 2 * PI / Parts)*0.5f, sinf((i + 1) * 2 * PI / Parts)*0.5f, 0xFFFFFF00, 0.0f, 0.0f);
				}
	*/

				//Create a circle of objects of alternating colors around where the coin was
				//Initially intended to make a circle around where the coin was and fade away, but currently makes a semi-random circle around where it was and a point at the center
				//I like it
				float startingDegrees = AERandFloat()*15;
				GameObjInst * temp;
				Vector2D pos, tempPos;
				for (int i = 0; i < 13; i++)
				{
					Vector2DSet(&tempPos, cosf(startingDegrees), sinf(startingDegrees));
					Vector2DAdd(&pos, &tempPos, &(pInst->posCurr));
					if (colorToggle == 0)
					{
						if (i % 2 == 0)
						{
							temp=gameObjInstCreate(PARTICLE_GOLD_CIRCLE, PARTICLE_SCALE, &pos, &(pInst->velCurr), startingDegrees, STATE_NONE);
							

						}
						else
						{
							temp=gameObjInstCreate(PARTICLE_RED_CIRCLE, PARTICLE_SCALE, &pos, &(pInst->velCurr), startingDegrees, STATE_NONE);
						}
						colorToggle = 1;
					}

					else
					{
						if (i % 2 == 0)
						{
							temp=gameObjInstCreate(PARTICLE_RED_CIRCLE, PARTICLE_SCALE, &(pInst->posCurr), &(pInst->velCurr), startingDegrees, STATE_NONE);
						}
						else
						{
						temp =	gameObjInstCreate(PARTICLE_GOLD_CIRCLE, PARTICLE_SCALE, &(pInst->posCurr), &(pInst->velCurr), startingDegrees, STATE_NONE);
						}
						colorToggle = 0;
					}
					startingDegrees += 30;
					temp->counter =PARTICLE_LIFESPAN;
					

				}



				gameObjInstDestroy(pInst);




			}



		}



		else if (pInst->pObject->type == PARTICLE_GOLD_CIRCLE || pInst->pObject->type == PARTICLE_RED_CIRCLE || pInst->pObject->type == PARTICLE_BLUE_SQUARE)
		{
			if (pInst->counter > 0)
			{
				pInst->counter -= frameTime;
			}

			else
			{
				gameObjInstDestroy(pInst);
			}
		}
	}

	
	//Computing the transformation matrices of the game object instances
	for(i = 0; i < GAME_OBJ_INST_NUM_MAX; ++i)
	{
		Matrix2D scale, rot, trans;
		GameObjInst *pInst = sGameObjInstList + i;

		// skip non-active object
		if (0 == (pInst->flag & FLAG_ACTIVE))
			continue;

		Matrix2DScale(&scale, pInst->scale, pInst->scale);
		Matrix2DRotDeg(&rot, pInst->dirCurr);
		Matrix2DTranslate(&trans, pInst->posCurr.x, pInst->posCurr.y);

		Matrix2DConcat(&pInst->transform, &trans, &rot);
		Matrix2DConcat(&pInst->transform, &pInst->transform, &scale);
	}
}

void GameStatePlatformDraw(void)
{
	//Drawing the tile map (the grid)
	int i, j;
	Matrix2D cellTranslation, cellFinalTransformation;
	double frameTime;
	char strBuffer[100];


	// ======================
	// Getting the frame time
	// ======================

	frameTime = AEFrameRateControllerGetFrameTime();
	//printf("The value of frameTime is:  %d \n", frameTime);
	
	AEGfxSetRenderMode(AE_GFX_RM_COLOR);
	AEGfxTextureSet(NULL, 0, 0);
	AEGfxSetTintColor(1.0f, 1.0f, 1.0f, 1.0f);


	/////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////
	// TO DO 4:
	// -- Draw the map and all game object instances.
	// -- Concatenate "MapTransform" with the transformation matrix of each tile and game object
	//    instance.
	/////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////

	for (i = 0; i < GAME_OBJ_INST_NUM_MAX; i++)
	{
		GameObjInst* pInst = sGameObjInstList + i;

		// skip non-active object
		if (0 == (pInst->flag & FLAG_ACTIVE))
			continue;
		

		Matrix2DConcat(&(pInst->transform), &MapTransform, &(pInst->transform));
		//Matrix2DConcat(&(pInst->transform), &(pInst->transform), &MapTransform);
		AEGfxSetTransform(pInst->transform.m);
		AEGfxMeshDraw(pInst->pObject->pMesh, AE_GFX_MDM_TRIANGLES);
	}
}


void GameStatePlatformFree(void)
{
	unsigned int i;
	// kill all object in the list
	for (i = 0; i < GAME_OBJ_INST_NUM_MAX; i++)
		gameObjInstDestroy(sGameObjInstList + i);
}

void GameStatePlatformUnload(void)
{
	unsigned int i;
	// free all created mesh
	for (i = 0; i < sGameObjNum; i++)
		AEGfxMeshFree(sGameObjList[i].pMesh);

	/////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////
	// TO DO 12:
	// -- Free the map data
	/////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////

	FreeMapData();
}


GameObjInst* gameObjInstCreate(unsigned int type, float scale, Vector2D* pPos, Vector2D* pVel, float dir, enum STATE startState)
{
	unsigned int i;
	Vector2D zero = { 0.0f, 0.0f };

	AE_ASSERT_PARM(type < sGameObjNum);
	
	// loop through the object instance list to find a non-used object instance
	for (i = 0; i < GAME_OBJ_INST_NUM_MAX; i++)
	{
		GameObjInst* pInst = sGameObjInstList + i;
		 
		// check if current instance is not used
		if (pInst->flag == 0)
		{
			// it is not used => use it to create the new instance
			pInst->pObject	 = sGameObjList + type;
			pInst->flag		 = FLAG_ACTIVE;
			pInst->scale	 = scale;
			pInst->posCurr	 = pPos ? *pPos : zero;
			pInst->velCurr	 = pVel ? *pVel : zero;
			pInst->dirCurr	 = dir;
			pInst->pUserData = 0;
			pInst->gridCollisionFlag = 0;
			pInst->state = startState;
			pInst->innerState = INNER_STATE_ON_ENTER;
			pInst->counter = 0;
			
			// return the newly created instance
			return pInst;
		}
	}
}

void gameObjInstDestroy(GameObjInst* pInst)
{
	// if instance is destroyed before, just return
	if (pInst->flag == 0)
		return;

	// zero out the flag
	pInst->flag = 0;
}



void EnemyStateMachine(GameObjInst *pInst)
{
	double frameTime;
	frameTime = AEFrameRateControllerGetFrameTime();
	/////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////
	// TO DO 9:
	// -- Implement the enemy’s state machine.
	// -- Each enemy’s current movement status is controlled by its “state”, “innerState” and 
	//    “counter” member variables.
	//    Refer to the provided enemy movement flowchart.
	/////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////
	if (pInst->pObject->type == TYPE_OBJECT_ENEMY1)
	{
		if (pInst->state == STATE_NONE)
		{
			pInst->state = STATE_GOING_LEFT;
		}

		if (pInst->state == STATE_GOING_LEFT)
		{

			if (pInst->innerState == INNER_STATE_ON_ENTER)
			{

				Vector2DSet(&(pInst->velCurr), MOVE_VELOCITY_ENEMY * -1, pInst->velCurr.y);
				pInst->innerState = INNER_STATE_ON_UPDATE;
			}

			if (pInst->innerState == INNER_STATE_ON_UPDATE)
			{

				if ((pInst->gridCollisionFlag & COLLISION_LEFT) || GetCellValue(pInst->posCurr.x - 1, pInst->posCurr.y - 1) == 0)
				{
					Vector2DSet(&(pInst->velCurr), 0, pInst->velCurr.y);
					pInst->counter = ENEMY_IDLE_TIME;  //Set it to 2 to have it wait for 2s?
					pInst->innerState = INNER_STATE_ON_EXIT;
				}
			}

			if (pInst->innerState == INNER_STATE_ON_EXIT)
			{
				pInst->counter -= frameTime;
				if (pInst->counter <= 0)
				{
					pInst->state = STATE_GOING_RIGHT;
					pInst->innerState = INNER_STATE_ON_ENTER;
				}
			}

		}


		if (pInst->state == STATE_GOING_RIGHT)
		{



			if (pInst->innerState == INNER_STATE_ON_ENTER)
			{

				Vector2DSet(&(pInst->velCurr), MOVE_VELOCITY_ENEMY, pInst->velCurr.y);
				pInst->innerState = INNER_STATE_ON_UPDATE;
			}

			if (pInst->innerState == INNER_STATE_ON_UPDATE)
			{

				if ((pInst->gridCollisionFlag & COLLISION_RIGHT) || GetCellValue(pInst->posCurr.x + 1, pInst->posCurr.y - 1) == 0)
				{
					Vector2DSet(&(pInst->velCurr), 0, pInst->velCurr.y);
					pInst->counter = ENEMY_IDLE_TIME;  //Set it to 2 to have it wait for 2s?
					pInst->innerState = INNER_STATE_ON_EXIT;
				}
			}

			if (pInst->innerState == INNER_STATE_ON_EXIT)
			{
				pInst->counter -= frameTime;
				if (pInst->counter <= 0)
				{
					pInst->state = STATE_GOING_LEFT;
					pInst->innerState = INNER_STATE_ON_ENTER;
				}
			}

		}

		}

	//printf("Enemy x velocity:  %f\n", pInst->velCurr.x);



	}







