#include "MyDirectX.h"

//initialize variables
const string APPTITLE = "final project platformer";
const int SCREENWIDTH = 1920;
const int SCREENHEIGHT = 1080;
const int cellwidth = 24;
const int cellheight = 24;
const int cellrows = SCREENHEIGHT / cellheight;
const int cellcolumns = SCREENWIDTH / cellwidth;
int stoneCellBlock[cellrows][cellcolumns];
int deathCellBlock[cellrows][cellcolumns];
int coinCellBlock[cellrows][cellcolumns];
int finishCellBlock[cellrows][cellcolumns];
bool jump = false;
int idle = 0;
int coinCount;

// initialize variables for sprites
LPDIRECT3DSURFACE9 back_img = NULL;
LPDIRECT3DTEXTURE9 player_img = NULL;
LPDIRECT3DTEXTURE9 stone_image = NULL;
LPDIRECT3DTEXTURE9 death_image = NULL;
LPDIRECT3DTEXTURE9 finish_image = NULL;
LPDIRECT3DTEXTURE9 coin_image = NULL;
CSound* jumpSound = NULL;
CSound* loseSound = NULL;
CSound* winSound = NULL;
CSound* coinSound = NULL;
LPD3DXSPRITE sprite_obj;

struct SPRITE	// define structure SPRITE at the beginning of MyGame.cpp
{
    int x, y, movex, movey;
    int frame, columns;
    int width, height;
    int startframe, endframe;
    int starttime, delay;
    int direction;
    SPRITE()
    {
        x = y = 0; 			movex = movey = 0;
        frame = 0; columns = 1; 	width = height = 0;
        startframe = endframe = 0;	starttime = delay = 0;
        direction = 1;
    }
};

SPRITE player;
SPRITE stonecell[cellrows][cellcolumns];
SPRITE deathcell[cellrows][cellcolumns];
SPRITE finishcell[cellrows][cellcolumns];
SPRITE coincell[cellrows][cellcolumns];
int frame = 0;
int starttime = 0;

// determines if player is colliding with stones
int CollisionStone(SPRITE player, SPRITE stonecell)
{
    RECT rect1;
    rect1.left = player.x + 1;
    rect1.top = player.y + 1;
    rect1.right = player.x + player.width - 1;
    rect1.bottom = player.y + player.height - 1;

    RECT rect2;
    rect2.left = stonecell.x + 1;
    rect2.top = stonecell.y + 1;
    rect2.right = stonecell.x + stonecell.width - 1;
    rect2.bottom = stonecell.y + stonecell.height - 1;

    RECT dest;
    return IntersectRect(&dest, &rect1, &rect2);  // if rect1 and rect2 has intersection 
}

// determines if player is colliding with death blocks
int CollisionDeath(SPRITE player, SPRITE deathcell)
{
    RECT rect1;
    rect1.left = player.x + 1;
    rect1.top = player.y + 1;
    rect1.right = player.x + player.width - 1;
    rect1.bottom = player.y + player.height - 1;

    RECT rect2;
    rect2.left = deathcell.x + 1;
    rect2.top = deathcell.y + 1;
    rect2.right = deathcell.x + deathcell.width - 1;
    rect2.bottom = deathcell.y + deathcell.height - 1;

    RECT dest;
    return IntersectRect(&dest, &rect1, &rect2);  // if rect1 and rect2 has intersection 
}

// determines if player is colliding with coins
int CollisionCoin(SPRITE player, SPRITE coincell)
{
    RECT rect1;
    rect1.left = player.x + 1;
    rect1.top = player.y + 1;
    rect1.right = player.x + player.width - 1;
    rect1.bottom = player.y + player.height - 1;

    RECT rect2;
    rect2.left = coincell.x + 1;
    rect2.top = coincell.y + 1;
    rect2.right = coincell.x + coincell.width - 1;
    rect2.bottom = coincell.y + coincell.height - 1;

    RECT dest;
    return IntersectRect(&dest, &rect1, &rect2);  // if rect1 and rect2 has intersection 
}

// determines if player is colliding with finish blocks
int CollisionFinish(SPRITE player, SPRITE finishcell)
{
    RECT rect1;
    rect1.left = player.x + 1;
    rect1.top = player.y + 1;
    rect1.right = player.x + player.width - 1;
    rect1.bottom = player.y + player.height - 1;

    RECT rect2;
    rect2.left = finishcell.x + 1;
    rect2.top = finishcell.y + 1;
    rect2.right = finishcell.x + finishcell.width - 1;
    rect2.bottom = finishcell.y + finishcell.height - 1;

    RECT dest;
    return IntersectRect(&dest, &rect1, &rect2);  // if rect1 and rect2 has intersection 
}

// will load player sprite and initialize its starting place
bool LoadPlayer()
{
    D3DXIMAGE_INFO info;
    HRESULT result;

    //load the tank sprite
    player_img = LoadTexture("caveman.bmp", D3DCOLOR_XRGB(255, 0, 255));
    if (player_img == NULL)
        return false;
    //set the tank's properties
    result = D3DXGetImageInfoFromFile("caveman.bmp", &info);
    if (result != D3D_OK)
        return false;

    player.width = info.Width / 8;
    player.height = info.Height / 2;
    player.x = 10;
    player.y = 1050;
    player.movex = 0; player.movey = -1;
    player.endframe = 0;
    idle = 0;

    return true;
}

// send a true value if player is colliding with stone
bool stoneCollisionCheck()
{
    for (int i = 0; i < cellrows; i++)
        for (int j = 0; j < cellcolumns; j++)
        {
            // if hit stone, take player back to previous position
            if (stoneCellBlock[i][j] == 1 && CollisionStone(player, stonecell[i][j]))
            {
                player.x = player.x - player.movex;
                player.y = player.y - player.movey - 1;
                return true;
            }
        }

    return false;
}

// send a true value if player is colliding with death block
bool deathCollisionCheck()
{
    for (int i = 0; i < cellrows; i++)
        for (int j = 0; j < cellcolumns; j++)
        {
            // if hit stone, take player back to previous position
            if (deathCellBlock[i][j] == 1 && CollisionDeath(player, deathcell[i][j]))
            {
                player.x = player.x - player.movex;
                player.y = player.y - player.movey;
                return true;
            }
        }

    return false;
}

// send a true value if player is colliding with finish block
bool finishCollisionCheck()
{
    for (int i = 0; i < cellrows; i++)
        for (int j = 0; j < cellcolumns; j++)
        {
            // if hit stone, take player back to previous position
            if (finishCellBlock[i][j] == 1 && CollisionFinish(player, finishcell[i][j]))
            {
                player.x = player.x - player.movex;
                player.y = player.y - player.movey;
                return true;
            }
        }

    return false;
}

// send a true value if player is colliding with coin
bool coinCollisionCheck()
{
    for (int i = 0; i < cellrows; i++)
        for (int j = 0; j < cellcolumns; j++)
        {
            // if hit stone, take player back to previous position
            if (coinCellBlock[i][j] == 1 && CollisionCoin(player, coincell[i][j]))
            {
                player.x = player.x - player.movex;
                player.y = player.y - player.movey;
                coinCellBlock[i][j] = 0;
                coinCount--;
                return true;
            }
        }

    return false;
}

// will load stones and their positions in the grid forming a portion of the level
bool LoadStones()
{
    D3DXIMAGE_INFO info;
    HRESULT result;

    //load the stone image
    stone_image = LoadTexture("stone.bmp", D3DCOLOR_XRGB(255, 0, 255));
    if (stone_image == NULL)
        return false;
    //set the stone's properties
    for (int i = 0; i < cellrows; i++)
        for (int j = 0; j < cellcolumns; j++)
        {
            stoneCellBlock[i][j] = 0;
            stonecell[i][j].width = cellwidth;
            stonecell[i][j].height = cellheight;
            stonecell[i][j].x = j * cellwidth;
            stonecell[i][j].y = i * cellheight;
            stonecell[i][j].movex = 0;
            stonecell[i][j].movey = 0;
        }

    // randomly build 100 stone cells
    //for (int i = 0; i < 100; i++)
    //    cell[rand() % cellrows][rand() % cellcolumns] = 1;
    for(int i = 0; i < 19; i++)
        stoneCellBlock[44][i] = 1;

    for (int i = 22; i < 39; i++)
        stoneCellBlock[44][i] = 1;

    for (int i = 42; i < 59; i++)
        stoneCellBlock[44][i] = 1;

    for (int i = 62; i < 80; i++)
        stoneCellBlock[44][i] = 1;

    for (int i = 39; i < 44; i++)
        stoneCellBlock[i][79] = 1;

    for (int i = 65; i < 73; i++)
        stoneCellBlock[39][i] = 1;

    stoneCellBlock[37][63] = 1;

    for (int i = 57; i < 59; i++)
        stoneCellBlock[37][i] = 1;

    stoneCellBlock[35][55] = 1;

    stoneCellBlock[33][53] = 1;

    for (int i = 46; i < 49; i++)
        stoneCellBlock[33][i] = 1;

    for (int i = 35; i < 38; i++)
        stoneCellBlock[35][i] = 1;

    for (int i = 28; i < 31; i++)
        stoneCellBlock[35][i] = 1;

    for (int i = 20; i < 23; i++)
        stoneCellBlock[35][i] = 1;

    for (int i = 0; i < 10; i++)
        stoneCellBlock[39][i] = 1;

    for (int i = 34; i < 40; i++)
        stoneCellBlock[i][0] = 1;

    for (int i = 28; i < 34; i++)
        stoneCellBlock[i][5] = 1;

    for (int i = 5; i < 80; i++)
        stoneCellBlock[28][i] = 1;

    for (int i = 25; i < 28; i++)
        stoneCellBlock[i][40] = 1;

    for (int i = 18; i < 28; i++)
        stoneCellBlock[i][70] = 1;

    for (int i = 0; i < 65; i++)
        stoneCellBlock[20][i] = 1;

    for (int i = 6; i < 71; i++)
        stoneCellBlock[11][i] = 1;

    for (int i = 17; i < 20; i++)
        stoneCellBlock[i][55] = 1;

    for (int i = 17; i < 20; i++)
        stoneCellBlock[i][40] = 1;

    for (int i = 17; i < 20; i++)
        stoneCellBlock[i][25] = 1;

    for (int i = 17; i < 20; i++)
        stoneCellBlock[i][24] = 1;

    for (int i = 17; i < 20; i++)
        stoneCellBlock[i][23] = 1;

    for (int i = 17; i < 20; i++)
        stoneCellBlock[i][22] = 1;

    for (int i = 13; i < 20; i++)
        stoneCellBlock[i][0] = 1;

    for (int i = 4; i < 12; i++)
        stoneCellBlock[i][5] = 1;

    for (int i = 4; i < 12; i++)
        stoneCellBlock[i][70] = 1;

    for (int i = 71; i < 76; i++)
        stoneCellBlock[4][i] = 1;

    return true;
}

// will load death blocks and their positions in the grid forming a portion of the level
bool LoadDeathBlock()
{
    death_image = LoadTexture("deathBlock.bmp", D3DCOLOR_XRGB(255, 0, 255));
    if (death_image == NULL)
        return false;
    //set the deathBlock properties
    for (int i = 0; i < cellrows; i++)
        for (int j = 0; j < cellcolumns; j++)
        {
            deathCellBlock[i][j] = 0;
            deathcell[i][j].width = cellwidth;
            deathcell[i][j].height = cellheight;
            deathcell[i][j].x = j * cellwidth;
            deathcell[i][j].y = i * cellheight;
            deathcell[i][j].movex = 0;
            deathcell[i][j].movey = 0;
        }

    for (int i = 19; i < 22; i++)
        deathCellBlock[44][i] = 1;

    for (int i = 39; i < 42; i++)
        deathCellBlock[44][i] = 1;

    for (int i = 59; i < 62; i++)
        deathCellBlock[44][i] = 1;

    for (int i = 10; i < 65; i++)
        deathCellBlock[39][i] = 1;

    deathCellBlock[27][10] = 1;
    deathCellBlock[27][20] = 1;
    deathCellBlock[27][30] = 1;
    deathCellBlock[25][41] = 1;
    deathCellBlock[26][42] = 1;
    deathCellBlock[27][43] = 1;
    deathCellBlock[21][41] = 1;
    deathCellBlock[22][42] = 1;
    deathCellBlock[23][43] = 1;
    deathCellBlock[24][44] = 1;
    deathCellBlock[25][45] = 1;
    deathCellBlock[16][21] = 1;
    deathCellBlock[17][21] = 1;
    deathCellBlock[18][21] = 1;
    deathCellBlock[19][20] = 1;
    deathCellBlock[12][20] = 1;
    deathCellBlock[13][19] = 1;
    deathCellBlock[14][18] = 1;
    deathCellBlock[15][17] = 1;
    deathCellBlock[16][16] = 1;
    deathCellBlock[17][16] = 1;
    deathCellBlock[18][16] = 1;

    for (int i = 12; i < 18; i++)
        deathCellBlock[i][70] = 1;

    for (int i = 4; i < 7; i++)
        deathCellBlock[i][8] = 1;

    for (int i = 8; i < 69; i++)
        deathCellBlock[7][i] = 1;

    for (int i = 4; i < 7; i++)
        deathCellBlock[i][68] = 1;

    for (int i = 0; i < 10; i++)
        deathCellBlock[i][79] = 1;

    deathCellBlock[10][78] = 1;
    deathCellBlock[11][77] = 1;
    deathCellBlock[12][76] = 1;
    deathCellBlock[13][75] = 1;
    deathCellBlock[14][74] = 1;
    deathCellBlock[15][74] = 1;
    deathCellBlock[16][75] = 1;
    deathCellBlock[17][76] = 1;
    deathCellBlock[18][77] = 1;
    deathCellBlock[19][78] = 1;
    deathCellBlock[20][79] = 1;

    deathCellBlock[17][54] = 1;
    deathCellBlock[18][53] = 1;
    deathCellBlock[19][52] = 1;

    deathCellBlock[12][54] = 1;
    deathCellBlock[13][54] = 1;
    deathCellBlock[14][53] = 1;
    deathCellBlock[15][52] = 1;
    deathCellBlock[16][51] = 1;
    deathCellBlock[17][50] = 1;

    deathCellBlock[17][39] = 1;
    deathCellBlock[18][38] = 1;
    deathCellBlock[19][37] = 1;

    deathCellBlock[12][39] = 1;
    deathCellBlock[13][39] = 1;
    deathCellBlock[14][38] = 1;
    deathCellBlock[15][37] = 1;
    deathCellBlock[16][36] = 1;
    deathCellBlock[17][35] = 1;

    for (int i = 20; i < 28; i++)
        deathCellBlock[i][79] = 1;

    for (int i = 22; i < 28; i++)
        deathCellBlock[i][75] = 1;

    deathCellBlock[21][74] = 1;
    deathCellBlock[20][73] = 1;
    deathCellBlock[19][72] = 1;
    deathCellBlock[18][71] = 1;

    deathCellBlock[11][71] = 1;
    deathCellBlock[10][72] = 1;
    deathCellBlock[9][73] = 1;
    deathCellBlock[8][74] = 1;
    deathCellBlock[7][75] = 1;
    deathCellBlock[6][75] = 1;
    deathCellBlock[5][75] = 1;

    return true;
}

// will load coins and their positions in the grid forming a portion of the level as well as initialize coinCount
bool LoadCoins()
{
    coin_image = LoadTexture("coin.bmp", D3DCOLOR_XRGB(255, 0, 255));
    if (coin_image == NULL)
        return false;
    //set the coin properties
    for (int i = 0; i < cellrows; i++)
        for (int j = 0; j < cellcolumns; j++)
        {
            coinCellBlock[i][j] = 0;
            coincell[i][j].width = cellwidth;
            coincell[i][j].height = cellheight;
            coincell[i][j].x = j * cellwidth;
            coincell[i][j].y = i * cellheight;
            coincell[i][j].movex = 0;
            coincell[i][j].movey = 0;
        }

    coinCount = 0;

    coinCellBlock[43][10] = 1;
    coinCount++;

    coinCellBlock[43][30] = 1;
    coinCount++;

    coinCellBlock[43][50] = 1;
    coinCount++;

    coinCellBlock[43][70] = 1;
    coinCount++;

    coinCellBlock[38][79] = 1; 
    coinCount++;

    coinCellBlock[38][68] = 1; 
    coinCount++;

    coinCellBlock[34][55] = 1; 
    coinCount++;

    coinCellBlock[32][47] = 1; 
    coinCount++;

    coinCellBlock[34][36] = 1; 
    coinCount++;

    coinCellBlock[34][29] = 1; 
    coinCount++;

    coinCellBlock[34][21] = 1; 
    coinCount++;

    coinCellBlock[38][5] = 1; 
    coinCount++;

    coinCellBlock[33][0] = 1; 
    coinCount++;

    coinCellBlock[27][7] = 1;
    coinCount++;

    coinCellBlock[27][15] = 1;
    coinCount++;

    coinCellBlock[27][25] = 1;
    coinCount++;

    coinCellBlock[27][35] = 1;
    coinCount++;

    coinCellBlock[25][43] = 1;
    coinCount++;

    coinCellBlock[27][57] = 1;
    coinCount++;

    coinCellBlock[18][69] = 1;
    coinCount++;

    coinCellBlock[17][52] = 1;
    coinCount++;

    coinCellBlock[17][37] = 1;
    coinCount++;

    coinCellBlock[19][16] = 1;
    coinCount++;

    coinCellBlock[12][0] = 1;
    coinCount++;

    coinCellBlock[3][5] = 1;
    coinCount++;

    coinCellBlock[10][12] = 1;
    coinCount++;

    coinCellBlock[10][22] = 1;
    coinCount++;

    coinCellBlock[10][32] = 1;
    coinCount++;

    coinCellBlock[10][42] = 1;
    coinCount++;

    coinCellBlock[10][52] = 1;
    coinCount++;

    coinCellBlock[10][62] = 1;
    coinCount++;

    coinCellBlock[3][70] = 1;
    coinCount++;

    coinCellBlock[6][77] = 1;
    coinCount++;

    coinCellBlock[14][72] = 1;
    coinCount++;

    coinCellBlock[22][77] = 1; 
    coinCount++;

    return true;
}

// will load finish blocks and their positions in the grid forming a portion of the level
bool LoadFinish()
{
    finish_image = LoadTexture("finish.bmp", D3DCOLOR_XRGB(255, 0, 255));
    if (finish_image == NULL)
        return false;
    //set the finish properties
    for (int i = 0; i < cellrows; i++)
        for (int j = 0; j < cellcolumns; j++)
        {
            finishCellBlock[i][j] = 0;
            finishcell[i][j].width = cellwidth;
            finishcell[i][j].height = cellheight;
            finishcell[i][j].x = j * cellwidth;
            finishcell[i][j].y = i * cellheight;
            finishcell[i][j].movex = 0;
            finishcell[i][j].movey = 0;
        }

    finishCellBlock[27][76] = 1;
    finishCellBlock[27][77] = 1;
    finishCellBlock[27][78] = 1;

    return true;
}
// the whole level is ready to be loaded and then needs to be displayed

// this will determine how the player sprite moves
// jumping is very strange as I could not figure out how to temporarily stop the player gravity when the player jumps 
// and then reactivate the gravity after a set amount of time while not colliding with anything
// instead the player will jump up very fast and fall down at a normal speed, creating a very strange jump
// the jump instead of being a smooth upside down U motion looks like a 45 degree angle motion
void UpdatePlayer(HWND hwnd)
{
    player.frame = idle;

    //forces player to go down
    if (!jump)
    {
        player.movex = 0;
        player.movey = 1;
        //player.frame = 0;
        player.x += player.movex;
        player.y += player.movey;
    }

    //check for right movement
    if (Key_Down(DIK_D))
    {
        //make player move faster
        if (Key_Down(DIK_K))
            player.movex = 2;
        else
            player.movex = 1;

        player.movey = 0;
        player.frame = 0;
        idle = player.frame;
        player.x += player.movex; // only move when key pressed
        player.y += player.movey;
    }

    //check for left movement
    if (Key_Down(DIK_A))
    {
        //make player move faster
        if (Key_Down(DIK_K))
            player.movex = -2;
        else
            player.movex = -1;
        player.movey = 0;
        player.frame = 1;
        idle = player.frame;
        player.x += player.movex;
        player.y += player.movey;
    }

    //check for jump key
    if (Key_Down(DIK_J))
    {
            //check if player is allowed to jump
            if(jump)
            {
                PlaySound(jumpSound);
                player.movex = 0;
                player.movey = -60;
                player.frame = idle;
                player.x += player.movex;
                player.y += player.movey;
            }
    }

    if (player.x < 0)
        player.x = 0;
    if (player.x > SCREENWIDTH - player.width)
        player.x = SCREENWIDTH - player.width;
    if (player.y < 0)
        player.y = 0;
    if (player.y > SCREENHEIGHT - player.height)
        player.y = SCREENHEIGHT - player.height;

    // will check if player is allowed to jump by checking collision
    jump = stoneCollisionCheck();

    // will check if player collides with death block, if so restart the level with lose message
    if (deathCollisionCheck())
    {
        PlaySound(loseSound);
        MessageBox(hwnd, "You lose... Press (Esc) to quit or press (Enter) to try again", "Lose Message", 0);
        LoadPlayer();
        LoadCoins();
    }

    // will check if player collides with finish block
    if (finishCollisionCheck())
    {
        // if player collected every coin the count will equal 0 and player will win the game
        if (coinCount == 0)
        {
            PlaySound(winSound);
            MessageBox(hwnd, ("You collected every coin & Won! Press (Enter) or click (ok) to quit out"), "Win Message", 0);
            gameover = true;
        }
        // if player did not collect every coin they will lose and level will be restarted
        else
        {
            PlaySound(loseSound);
            MessageBox(hwnd, ("You Lose? You did not collect every coin... Press (Esc) to quit or press (Enter) to try again"), "Lose Message", 0);
            LoadPlayer();
            LoadCoins();
        }
    }

    // if player collects a coin, coin sound is played as well as the collision check lowering the coin count
    if (coinCollisionCheck())
    {
        PlaySound(coinSound);
    }
}

bool Game_Init(HWND hwnd)
{
    Direct3D_Init(hwnd, SCREENWIDTH, SCREENHEIGHT, false);
    DirectInput_Init(hwnd);
    DirectSound_Init(hwnd);

    // load the background image
    back_img = LoadSurface("background.bmp");
    if (!back_img) {
        MessageBox(hwnd, "Error loading back_img", "Error", 0);
        return false;
    }

    // load the player sprite texture
    player_img = LoadTexture("caveman.bmp", D3DCOLOR_XRGB(255, 0, 255));
    if (!player_img) {
        MessageBox(hwnd, "Error loading caveman", "Error", 0);
        return false;
    }

    // load the stone sprite texture
    stone_image = LoadTexture("stone.bmp", D3DCOLOR_XRGB(255, 0, 255));
    if (!stone_image) {
        MessageBox(hwnd, "Error loading stone_image", "Error", 0);
        return false;
    }

    // load the death block texture
    death_image = LoadTexture("deathBlock.bmp", D3DCOLOR_XRGB(255, 0, 255));
    if (!death_image) {
        MessageBox(hwnd, "Error loading death_image", "Error", 0);
        return false;
    }

    // load the finish block texture
    finish_image = LoadTexture("finish.bmp", D3DCOLOR_XRGB(255, 0, 255));
    if (!finish_image) {
        MessageBox(hwnd, "Error loading finish_image", "Error", 0);
        return false;
    }

    // load the coin texture
    coin_image = LoadTexture("coin.bmp", D3DCOLOR_XRGB(255, 0, 255));
    if (!coin_image) {
        MessageBox(hwnd, "Error loading coin_image", "Error", 0);
        return false;
    }

    // load the jump sound
    jumpSound = LoadSound("jump.wav");
    if (jumpSound == NULL) {
        MessageBox(hwnd, "Error loading jumpSound", "Error", 0);
        return false;
    }

    // load the win sound
    winSound = LoadSound("win.wav");
    if (winSound == NULL) {
        MessageBox(hwnd, "Error loading winSound", "Error", 0);
        return false;
    }

    // load the lose sound
    loseSound = LoadSound("lose.wav");
    if (loseSound == NULL) {
        MessageBox(hwnd, "Error loading loseSound", "Error", 0);
        return false;
    }

    // load the coin collect sound
    coinSound = LoadSound("coinCollect.wav");
    if (coinSound == NULL) {
        MessageBox(hwnd, "Error loading coinSound", "Error", 0);
        return false;
    }

    // load the positions of the stones, death blocks, coins, & finish blocks to create the level as well as load player position
    LoadPlayer();
    LoadStones();
    LoadDeathBlock();
    LoadCoins();
    LoadFinish();

    return true;
}

void Game_Run(HWND hwnd)
{
    if (!d3ddev) return;
    DirectInput_Update();

    // allows to player to control the player sprite
    UpdatePlayer(hwnd);

    //start rendering and display everything that is loaded
    if (d3ddev->BeginScene())
    {
        //draw the background
        DrawSurface(backbuffer, 0, 0, back_img);

        //start sprite handler
        sprite_obj->Begin(D3DXSPRITE_ALPHABLEND);
        
        Sprite_Draw_Frame(player_img, player.x, player.y, player.frame, player.width, player.height, player.columns);

        for (int i = 0; i < cellrows; i++)
            for (int j = 0; j < cellcolumns; j++)
                if (stoneCellBlock[i][j] == 1)
                    Sprite_Draw_Frame(stone_image,
                        stonecell[i][j].x, stonecell[i][j].y,
                        stonecell[i][j].frame, stonecell[i][j].width,
                        stonecell[i][j].height, stonecell[i][j].columns);

        for (int i = 0; i < cellrows; i++)
            for (int j = 0; j < cellcolumns; j++)
                if (deathCellBlock[i][j] == 1)
                    Sprite_Draw_Frame(death_image,
                        deathcell[i][j].x, deathcell[i][j].y,
                        deathcell[i][j].frame, deathcell[i][j].width,
                        deathcell[i][j].height, deathcell[i][j].columns);

        for (int i = 0; i < cellrows; i++)
            for (int j = 0; j < cellcolumns; j++)
                if (finishCellBlock[i][j] == 1)
                    Sprite_Draw_Frame(finish_image,
                        finishcell[i][j].x, finishcell[i][j].y,
                        finishcell[i][j].frame, finishcell[i][j].width,
                        finishcell[i][j].height, finishcell[i][j].columns);

        for (int i = 0; i < cellrows; i++)
            for (int j = 0; j < cellcolumns; j++)
                if (coinCellBlock[i][j] == 1)
                    Sprite_Draw_Frame(coin_image,
                        coincell[i][j].x, coincell[i][j].y,
                        coincell[i][j].frame, coincell[i][j].width,
                        coincell[i][j].height, coincell[i][j].columns);

        //stop drawing
        sprite_obj->End();

        //stop rendering
        d3ddev->EndScene();
    }
    d3ddev->Present(NULL, NULL, NULL, NULL);

    if (Key_Down(DIK_ESCAPE)) 	//escape key exits
        gameover = true;
}

void Game_End()
{
    // delete all resources when program exits
    if (back_img) back_img->Release();
    if (player_img) player_img->Release();
    if (death_image) death_image->Release();
    if (coin_image) coin_image->Release();
    if (finish_image) finish_image->Release();
    if (jumpSound != NULL)	delete jumpSound;
    if (loseSound != NULL)	delete loseSound;
    if (winSound != NULL)	delete winSound;
    if (coinSound != NULL)	delete coinSound;

    DirectInput_Shutdown();
    Direct3D_Shutdown();
    DirectSound_Shutdown();
}