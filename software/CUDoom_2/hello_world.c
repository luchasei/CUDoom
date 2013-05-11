#include <math.h>
#include <io.h>
#include <system.h>
#include <stdio.h>
#include "sky.h"

//#define lookupLength 3480
//#define RAD 0.0018055130193045
//#define HALF_RAD 0.00090275650965225

#define lookupLength 4096
#define RAD 0.0015339807878856
#define HALF_RAD 0.0007669903939428

//#define lookupLength 3072
//#define RAD 0.0020453077171808
//#define HALF_RAD 0.0010226538585904


#define screenWidth 640
#define halfScreenWidth 320
#define screenHeight 480
#define texWidth 64
#define texHeight 64
#define mapWidth 32
#define mapHeight 32
#define extensionFactor 6
#define loopbackFactor 4

#define RED_8BIT 0xE0 //red
#define GREEN_8BIT 0x1C //green
#define BLUE_8BIT 0x03 //blue
#define YELLOW_8BIT 0xFC //yellow
#define WHITE_8BIT 0xFF //white
#define VIOLET_8BIT 0xE2 //violet

#define CTRL_WRITE_HIGH 0
#define CTRL_WRITE_LOW 0

#define posShift 22




#define IOWR_RAM_DATA(base, offset, data) \
IOWR_32DIRECT(base, (offset*4), data)

#define IORD_RAM_DATA(base, offset) \
IORD_32DIRECT(base, (offset*4) )

#define IOSKYWR_RAM_DATA(base, offset, data) \
  IOWR_16DIRECT(base, (offset) * 2, data)

char worldMap[mapWidth][mapHeight]=
{
 9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
 9,0,0,7,0,8,0,8,0,0,8,7,0,2,5,2,6,2,5,2,0,6,0,0,0,0,0,0,0,0,0,9,
 9,2,3,3,0,0,0,0,0,8,8,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,0,0,6,0,9,
 9,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,0,0,0,0,0,0,0,0,9,
 9,2,3,3,0,0,0,0,0,8,8,4,0,0,0,0,0,0,0,0,0,0,0,0,6,0,0,0,6,0,0,9,
 9,0,0,0,0,0,0,0,0,0,8,4,0,0,0,0,0,6,6,6,0,6,7,0,0,0,5,0,0,0,0,9,
 9,8,8,8,0,8,8,8,8,8,8,4,4,4,4,4,4,6,0,0,0,0,0,0,0,0,0,0,0,7,0,9,
 9,6,7,7,0,7,7,7,7,0,8,0,8,0,8,0,8,4,0,4,0,6,7,0,0,0,0,0,0,0,0,9,
 9,7,0,0,0,0,0,0,7,8,0,8,0,8,0,8,8,6,0,0,0,0,0,0,0,0,8,0,8,0,0,9,
 9,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,0,0,0,0,0,0,0,0,9,
 9,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,6,0,6,0,6,0,0,0,0,0,0,0,0,0,9,
 9,7,0,0,0,0,0,0,7,8,0,8,0,8,0,8,8,6,4,6,0,6,6,0,0,0,0,0,0,0,0,9,
 9,2,7,7,0,7,7,7,7,8,8,4,0,6,8,4,8,3,3,3,0,3,6,0,0,0,0,0,0,0,0,9,
 9,6,2,2,0,2,2,2,2,4,6,4,0,0,6,0,6,3,0,0,0,0,0,0,0,0,0,0,0,0,0,9,
 9,6,0,0,0,0,0,2,2,4,0,0,0,0,0,0,4,3,0,0,0,0,6,0,0,0,0,0,0,0,0,9,
 9,0,0,0,0,0,0,0,2,4,0,0,0,0,0,0,4,3,0,0,0,0,0,0,3,2,0,4,3,0,0,9,
 9,7,0,0,0,0,0,0,1,4,4,0,4,4,6,0,6,3,3,0,0,0,0,0,0,0,0,0,0,0,0,9,
 9,0,0,0,0,0,0,0,2,2,2,0,2,2,2,6,6,0,0,5,0,5,0,0,0,0,0,0,0,0,0,9,
 9,7,0,0,0,0,0,2,2,2,0,0,0,2,2,3,5,0,5,0,0,0,5,0,0,0,0,0,0,0,0,9,
 9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,0,0,0,0,0,0,0,0,0,9,
 9,7,0,0,0,0,0,0,3,4,2,1,1,2,0,4,4,3,1,2,2,0,0,0,0,0,0,0,0,0,0,9,
 9,0,0,0,0,0,0,0,2,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,
 9,2,0,2,7,2,7,2,2,2,0,7,0,8,8,0,5,0,4,0,7,0,0,0,0,0,0,0,0,0,0,9,
 9,2,0,2,7,2,7,2,2,2,0,7,0,8,8,0,5,0,4,0,7,0,0,0,0,0,0,0,0,0,0,9,
 9,2,0,2,0,0,0,0,2,2,0,7,0,8,8,0,5,0,4,0,7,0,0,0,0,0,0,0,0,0,0,9,
 9,2,0,2,0,2,0,0,0,0,0,7,0,8,8,0,3,0,4,0,7,0,0,0,0,0,0,0,0,0,0,9,
 9,2,0,2,0,2,0,2,2,2,0,2,0,8,8,0,5,0,4,0,7,0,0,0,0,0,0,0,0,0,0,9,
 9,2,0,2,0,2,0,0,0,1,0,2,0,3,3,0,5,0,4,0,7,0,0,0,0,0,0,0,0,0,0,9,
 9,2,0,2,0,2,7,2,0,2,0,1,0,8,8,0,3,0,4,0,7,0,0,0,0,0,0,0,0,0,0,9,
 9,2,0,2,0,2,7,1,0,1,0,1,0,8,8,0,5,0,4,0,7,0,0,0,0,0,0,0,0,0,0,9,
 9,2,0,0,0,2,7,2,1,2,0,7,0,0,0,0,5,0,0,0,7,0,0,0,0,0,0,0,0,0,0,9,
 9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9
};

//char worldMap[mapWidth][mapHeight]=
//{
// 1,1,1,1,1,1,1,1,1,1,1,4,4,2,4,4,2,4,2,4,4,4,2,4,
// 1,0,0,0,0,0,0,0,0,0,1,4,0,0,0,0,0,0,0,0,0,0,0,4,
// 1,0,3,3,0,0,0,0,0,1,1,4,0,0,0,0,0,0,0,0,0,0,0,2,
// 1,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,
// 1,0,3,3,0,0,0,0,0,1,1,4,0,0,0,0,0,0,0,0,0,0,0,4,
// 1,0,0,0,0,0,0,0,0,0,1,4,0,0,0,0,0,2,2,2,0,2,4,2,
// 1,1,1,1,0,1,1,1,1,1,1,4,4,4,4,4,4,2,0,0,0,0,0,2,
// 1,1,1,1,0,1,1,1,1,0,1,0,1,0,1,0,1,4,0,4,0,2,0,2,
// 1,1,0,0,0,0,0,0,1,1,0,1,0,1,0,1,1,2,0,0,0,0,0,2,
// 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,0,0,0,0,0,4,
// 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,0,2,0,2,0,2,
// 1,1,0,0,0,0,0,0,1,1,0,1,0,1,0,1,1,2,4,2,0,2,2,2,
// 1,1,1,1,0,1,1,1,1,1,1,4,0,2,1,4,1,3,3,3,0,3,3,3,
// 2,2,2,2,0,2,2,2,2,4,2,4,0,0,2,0,2,3,0,0,0,0,0,3,
// 2,2,0,0,0,0,0,2,2,4,0,0,0,0,0,0,4,3,0,0,0,0,0,3,
// 2,0,0,0,0,0,0,0,2,4,0,0,0,0,0,0,4,3,0,0,0,0,0,3,
// 1,0,0,0,0,0,0,0,1,4,4,4,4,4,2,0,2,3,3,0,0,0,3,3,
// 2,0,0,0,0,0,0,0,2,2,2,1,2,2,2,2,2,0,0,3,0,3,0,3,
// 2,2,0,0,0,0,0,2,2,2,0,0,0,2,2,0,3,0,3,0,0,0,3,3,
// 2,0,0,0,0,0,0,0,2,0,0,0,0,0,2,3,0,3,0,3,0,3,0,3,
// 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,
// 2,0,0,0,0,0,0,0,2,0,0,0,0,0,2,3,0,3,0,3,0,3,0,3,
// 2,2,0,0,0,0,0,2,2,2,0,0,0,2,2,0,3,0,3,0,0,0,3,3,
// 2,2,2,2,1,2,2,2,2,2,2,1,2,2,2,3,3,3,3,3,3,3,3,3
//};


//fixed point conversion functions
int doubleToInt(double);
//int ToDouble(int);

void DrawTexture(unsigned int columnIndex, unsigned int texX, unsigned int rowStart, unsigned int rowEnd, unsigned int side, unsigned int texNum, unsigned int invLineHeight , unsigned int line_minus_h);
void DrawAccelerate(int angle, int posX, int posY, int countstep, int rayDirX, int rayDirY, unsigned int columnIndex);
int absVal(int v);

unsigned int startarray[640];
unsigned int stoparray[640];

///UNECESSARY CAN BE IMPLEMENTED WITHOUT THESE TWO LOOKUP
int dirsine[lookupLength];
int dircosine[lookupLength];

/// MEMORY HERE CAN BE CUT DOWN IF NECESSARY
int sine[lookupLength];
int cosine[lookupLength];
//int inv_sine[lookupLength];
//int inv_cosine[lookupLength];

int main()
{

	IOWR_RAM_DATA(NIOSINTERFACE_1_0_BASE, 0, 0);
	int dir = 0;
	int posX = doubleToInt(21.5), posY = doubleToInt(11.5);  //x and y start position
 // int posX = 22, posY = 11;  //x and y start position
  int x =0;

  int p, q;


  IOSKYWR_RAM_DATA(SKYGEN_0_BASE, 262143, 0x0000);

  for(p = 0; p < 480; p++){
	  for(q = 0; q < 512; q++)
	  {

		  IOSKYWR_RAM_DATA(SKYGEN_0_BASE, p*512+q, (sky[p*1024+q*2+1]<<8) + (sky[p*1024+q*2]));

	  }
  }

  IOSKYWR_RAM_DATA(SKYGEN_0_BASE, 262143, 0x000F);
  double sine_temp;
  double cosine_temp;

	for(x = 0; x < lookupLength ; x++)
	{
			//calculate ray position and direction

		sine_temp = sin(x*RAD + HALF_RAD);
		cosine_temp = cos(x*RAD + HALF_RAD);

		dirsine[x] = doubleToInt(sin(x*RAD));
		dircosine[x] = doubleToInt(cos(x*RAD));
		sine[x] = doubleToInt(sine_temp);
		cosine[x] = doubleToInt(cosine_temp);
	}


  int angle;
  int fish_angle;
  int move;

  int rayDirX;
  int rayDirY;
  int count_step;

  int k;
  int k2;
  //char key[] = { 'd','d','d','d','d','d','d','d', 'd','d','d','d','d','d','d','d', 'd','d','d','d','d','d','d','d', 'd','d','d','d','d','d','d','d', 'd','d','d','d','d','d','d','d', 'd','d','d','d','d','d','d','d', 'd','d','d','d','d','d','d','d', 'd','d','d','d','d','d','d','d','d','d','d','d','d','d','d','d',
//		  'd','d','d','d','d','d','d','d', 'd','d','d','d','d','d','d','d', 'd','d','d','d','d','d','d','d', 'd','d','d','d','d','d','d','d', 'd','d','d','d','d','d','d','d', 'd','d','d','d','d','d','d','d', 'd','d','d','d','d','d','d','d', 'd','d','d','d','d','d','d','d','d','d','d','d','d','d','d','d'};

  unsigned char code = 0;
  //char key[] = {'d'};
  k2 = 0;
  //start the main loop
  //for (k2 = 0; k2 < sizeof(key); k2++)

  int hardwareData = 0;

  while(1)
  {

	  code = 0;

		//while (!IORD_8DIRECT(DE2_PS2_0_BASE, 0)) ; /* Poll the status */

//		code = IORD_8DIRECT(DE2_PS2_0_BASE, 0);

//		if (code)
			code = IORD_8DIRECT(DE2_PS2_1_BASE, 1);

//		printf("%c, %x, %d\n", code, code, code);

		/* Get received byte */

	  //code = 'k';

	  //hardwareData = IORD_RAM_DATA(NIOSINTERFACE_1_0_BASE, 1);
	  //printf("%d\n", hardwareData >> 4);


     x = 0;
    for(k = -halfScreenWidth; k < halfScreenWidth; k++)
    {
        angle = dir + k;

        if ( angle < 0)
                angle += lookupLength;

        if (angle >= lookupLength)
                angle -= lookupLength;

        fish_angle = k;

        if ( fish_angle < 0)
            fish_angle += lookupLength;

        if (fish_angle >= lookupLength)
             fish_angle -= lookupLength;


      //calculate ray position and direction
      //double cameraX = 2*x/double(w)-1; //x-coordinate in camera space
      rayDirX = cosine[angle]>>extensionFactor;
      rayDirY = sine[angle]>>extensionFactor;
      count_step = cosine[fish_angle]>>extensionFactor;

      //using a happy medium between 1/32 and 1/64 ray extension increment
//      rayDirX = (cosine[angle]>>6)+(cosine[angle]>>7);
//      rayDirY = (sine[angle]>>6) +(sine[angle]>>7);
//      count_step = (cosine[fish_angle]>>6) +(cosine[fish_angle]>>7);


 //     hardwareData = IORD_32DIRECT(NIOSINTERFACE_1_0_BASE, 1);
    //  printf("%d\n", hardwareData);
     DrawAccelerate(angle, posX, posY, count_step, rayDirX, rayDirY, x);
     IOWR_RAM_DATA(NIOSINTERFACE_1_0_BASE, 0, 0);



     hardwareData = IORD_32DIRECT(NIOSINTERFACE_1_0_BASE, 1);

	 while (!(hardwareData & 1)){
		 hardwareData = IORD_32DIRECT(NIOSINTERFACE_1_0_BASE, 1);
	  }


      //IOWR_RAM_DATA(NIOSINTERFACE_1_0_BASE, 0, 0);

      IOWR_RAM_DATA(NIOSINTERFACE_1_0_BASE, 0, 0xFFFFFFFF);



//	  printf("out of loop\n");
	  //IOWR_RAM_DATA(NIOSINTERFACE_0_BASE, 0, 0);

//	  int rayPosX = posX;
//	  int rayPosY = posY;
//	  int count = 0;
//      int mapX;
//      int mapY;
//
//      //what direction to step in x or y-direction (either +1 or -1)
//      int stepX;
//      int stepY;
//
//      int hit = 0; //was there a wall hit?
//      unsigned int side = 0; //was a NS or a EW wall hit?
//
//      //calculate step and initial sideDist
//      if (rayDirX < 0)
//         stepX = -1;
//      else
//         stepX = 1;
//
//      if (rayDirY < 0)
//         stepY = -1;
//      else
//         stepY = 1;
//
//
//      while ( worldMap[rayPosX>>posShift][rayPosY>>posShift] == 0)
//      {
//        count += count_step;
//
//        //jump to next map square, OR in x-direction, OR in y-direction
//        rayPosX += rayDirX;
//        rayPosY += rayDirY;
//
//        //Check if ray has hit a wall
//      }
//
//      hit = 1;
//      ////////////////////////////////////////////////////////////////////////
//      /////LOOOP BACK CODE FOR SMOOTHER EDGES AND FASTER SPEEDS///////////////
//
//      count_step = count_step>>4;
//      rayDirX = rayDirX>>4;
//      rayDirY = rayDirY>>4;
//
//      while(hit == 1)
//      {
//
//        count -= count_step;
//
//        //jump to next map square, OR in x-direction, OR in y-direction
//        rayPosX -= rayDirX;
//        rayPosY -= rayDirY;
//
//        //Check if ray has hit a wall
//        if (worldMap[rayPosX>>posShift][rayPosY>>posShift] == 0) hit = 0;
//
//      }
//
//      count += count_step;
//      rayPosX += rayDirX;
//      rayPosY += rayDirY;
//
//      ////////////////////////////////////////////////////////////////////////
//      ////////////////////////////////////////////////////////////////////////
//
//      mapX = ((rayPosX>>posShift)  + ((1 - stepX)>>1))<<posShift;
//      mapY = ((rayPosY>>posShift)  + ((1 - stepY)>>1))<<posShift;
//
//      //Calculate distance of perpendicular ray (oblique distance will give fisheye effect!)
//      if ( absVal(mapX - rayPosX) <  absVal(mapY - rayPosY) )
//          side  = 1;
//
//
//      //Calculate height of line to draw on screen
//      //int lineHeight = abs( (screenHeight<<posShift) /count);
//      int lineHeight = (screenHeight<<posShift) /count;
//      //printf("lineHeight= %d", lineHeight);
////      if (lineHeight >= screenHeight){
////    	  lineHeight = 0x1FF;
////    	  //printf(" lineHeight= %d", lineHeight);
////      }
//      //printf("\n");
//
//      //calculate lowest and highest pixel to fill in current stripe
//      int drawStart = (-lineHeight >> 1) + (screenHeight >> 1);
//      if(drawStart < 0) drawStart = 0;
//      int drawEnd = (lineHeight >> 1) + (screenHeight >> 1);
//      if(drawEnd >= screenHeight) drawEnd = screenHeight - 1;
//
//      /////////////////////////////////////////////////
//      ////texturing calculations
//      /////////////////////////////////////////////////
//	   unsigned int texNum = worldMap[rayPosX>>posShift][rayPosY>>posShift] - 1; //1 subtracted from it so that texture 0 can      be used!
//
//	   //calculate value of wallX
//	   int wallX; //where exactly the wall was hit
//
//	   if (side == 0) wallX = rayPosX;
//	   else           wallX = rayPosY;
//
//
//	   wallX -= (wallX>>posShift)<<posShift;
//
//	   //x coordinate on the texture
//	   unsigned int texX = (wallX * texWidth)>>posShift;
//	   if(side == 1 && rayDirX > 0) texX = texWidth - texX - 1;
//	   if(side == 0 && rayDirY < 0) texX = texWidth - texX - 1;
//
//	   unsigned int invLineHeight = (texHeight << 16)/lineHeight;
//	   int line_minus_h = lineHeight - screenHeight;
//
//
//
//	   DrawTexture(x, texX, drawStart, drawEnd, side, texNum, invLineHeight , line_minus_h);

//       unsigned int color;
//      switch(worldMap[rayPosX>>posShift][rayPosY>>posShift])
//      {
//        case 1:  color = RED_8BIT;  break; //red
//        case 2:  color = GREEN_8BIT;  break; //green
//        case 3:  color = BLUE_8BIT;   break; //blue
//        case 4:  color = WHITE_8BIT;  break; //white
//        default: color = YELLOW_8BIT; break; //yellow
//      }

      //draw the pixels of the stripe as a vertical line
      //DrawColumn(x, color, drawStart, drawEnd, side);
      //DrawColumn(x, color, lineHeight, side);
      //verLine(x, drawStart, drawEnd, color);

      x++;
    }



    //move forward if no wall in front of you
    if (code =='u')
    {
        move = dircosine[dir]>>4;
        if(worldMap[(posX + move)>>posShift][posY>>posShift] == 0)
                posX += move;

        move = dirsine[dir]>>4;
        if(worldMap[posX>>posShift][(posY+move)>>posShift] == 0)
                posY += move;
    }
    //move backwards if no wall behind you
    if (code =='r')
    {
        move = dircosine[dir]>>4;
        if(worldMap[(posX - move)>>posShift][posY>>posShift] == 0)
                posX -= move;

        move = dirsine[dir]>>4;
        if(worldMap[posX>>posShift][(posY - move)>>posShift] == 0)
                posY -= move;
    }
    //rotate to the right
    if (code == 't')
    {
      //both camera direction and camera plane must be rotated
       dir += 13;

       if(dir >= lookupLength)
           dir -= lookupLength;
    }
    //rotate to the left
    if (code =='k')
    {
      //both camera direction and camera plane must be rotated
        dir -= 13;

        if (dir <0)
            dir += lookupLength;
    }


  }



    return 0;
}

int absVal(int v)
{
  return v * ((v>0) - (v<0));
}

int intToDouble(int a)
{

    return a>>posShift;

}


int doubleToInt(double a)
{
    return (int)(a*(1<<posShift));
}


void DrawTexture(unsigned int columnIndex, unsigned int texX, unsigned int rowStart, unsigned int rowEnd, unsigned int side, unsigned int texNum, unsigned int invLineHeight , unsigned int line_minus_h)
{

	IOWR_RAM_DATA(NIOSINTERFACE_1_0_BASE, 8, (columnIndex  << 22));
	IOWR_RAM_DATA(NIOSINTERFACE_1_0_BASE, 1, ((texNum & 3) + (texX <<2) + (rowEnd<< 8) + (rowStart<<17)+ ((invLineHeight & 0x3F)<<26)));
	IOWR_RAM_DATA(NIOSINTERFACE_1_0_BASE, 2, ((invLineHeight & 0x3FFFF) >> 6) + ((line_minus_h & 0x3FFFF) << 12 ) + (side << 31));
	IOWR_RAM_DATA(NIOSINTERFACE_1_0_BASE, 0, 1);
	IOWR_RAM_DATA(NIOSINTERFACE_1_0_BASE, 0, 0);

	//startarray[columnIndex] = (color + (endRow<< 8));
	//stoparray[columnIndex] = (side << 10) + (endRow >> (9 - 1)) + (startRow << 1);
}

void DrawAccelerate(int angle, int posX, int posY, int countstep, int rayDirX, int rayDirY, unsigned int columnIndex)
{

	IOWR_RAM_DATA(NIOSINTERFACE_1_0_BASE, 8, (columnIndex << 22));
	IOWR_RAM_DATA(NIOSINTERFACE_1_0_BASE, 1, posX);
	IOWR_RAM_DATA(NIOSINTERFACE_1_0_BASE, 2, posY );
	IOWR_RAM_DATA(NIOSINTERFACE_1_0_BASE, 3, countstep );
	IOWR_RAM_DATA(NIOSINTERFACE_1_0_BASE, 4, rayDirX);
	IOWR_RAM_DATA(NIOSINTERFACE_1_0_BASE, 5, rayDirY );
	IOWR_RAM_DATA(NIOSINTERFACE_1_0_BASE, 6, angle & 0x03FF);



	//startarray[columnIndex] = (color + (endRow<< 8));
	//stoparray[columnIndex] = (side << 10) + (endRow >> (9 - 1)) + (startRow << 1);
}

