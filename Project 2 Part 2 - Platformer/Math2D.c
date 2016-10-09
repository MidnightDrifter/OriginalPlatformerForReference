#include "Math2D.h"
#include "stdio.h"
#include <math.h>
#define PI      3.1415926535897932384626433832795
////////////////////
// From Project 2 //
////////////////////


/*
This function checks if the point P is colliding with the circle whose
center is "Center" and radius is "Radius"
*/
int StaticPointToStaticCircle(Vector2D *pP, Vector2D *pCenter, float Radius)
{
	float distance, xValue, yValue;

	xValue = pow((pP->x - pCenter->x), 2);
	yValue = pow((pP->y - pCenter->y), 2);
	distance = xValue + yValue;

	if (distance <= pow(Radius, 2))
	{
		return 1;
	}

	else
	{
		return 0;
	}
}


/*
This function checks if the point Pos is colliding with the rectangle
whose center is Rect, width is "Width" and height is Height
*/
int StaticPointToStaticRect(Vector2D *pPos, Vector2D *pRect, float Width, float Height)
{

	float left, right, top, bot, x, y, pX, pY;
	x = pRect->x;
	y = pRect->y;
	left = x - Width / 2;
	right = x + Width / 2;
	top = y + Height / 2;
	bot = y - Height / 2;

	pX = pPos->x;
	pY = pPos->y;

	if (pX < left || pX>right || pY < bot || pY > top)
	{
		return 0;
	}

	else
	{
		return 1;
	}



}

/*
This function checks for collision between 2 circles.
Circle0: Center is Center0, radius is "Radius0"
Circle1: Center is Center1, radius is "Radius1"
*/
int StaticCircleToStaticCircle(Vector2D *pCenter0, float Radius0, Vector2D *pCenter1, float Radius1)
{
	float distance, xVal, yVal, radDist;

	radDist = pow((Radius1 + Radius0), 2);
	xVal = pow((pCenter0->x - pCenter1->x), 2);
	yVal = pow((pCenter0->y - pCenter1->y), 2);
	distance = xVal + yVal;

	if (distance <= radDist)
	{
		return 1;
	}

	else
	{
		return 0;
	}


}

/*
This functions checks if 2 rectangles are colliding
Rectangle0: Center is pRect0, width is "Width0" and height is "Height0"
Rectangle1: Center is pRect1, width is "Width1" and height is "Height1"
*/
int StaticRectToStaticRect(Vector2D *pRect0, float Width0, float Height0, Vector2D *pRect1, float Width1, float Height1)
{

	float r0, r1, l0, l1, t0, t1, b0, b1;

	r0 = pRect0->x + (Width0) / 2;
	l0 = pRect0->x - (Width0) / 2;
	t0 = pRect0->y + (Height0) / 2;
	b0 = pRect0->y - (Height0) / 2;


	r1 = pRect1->x + (Width1) / 2;
	l1 = pRect1->x - (Width1) / 2;
	t1 = pRect1->y + (Height1) / 2;
	b1 = pRect1->y - (Height1) / 2;


	if (r0<l1 || r1 <l0 || t0<b1 || t1<b0)
	{
		return 0;
	}

	else
	{
		return 1;
	}




}
//////////////////////
// New to Project 3 //
//////////////////////

/*
This function checks if a static circle is intersecting with a static rectangle

Circle:		Center is "Center", radius is "Radius"
Rectangle:	Center is "Rect", width is "Width" and height is "Height"
Function returns true is the circle and rectangle are intersecting, otherwise it returns false
*/

int StaticCircleToStaticRectangle(Vector2D *pCenter, float Radius, Vector2D *pRect, float Width, float Height)
{
	Vector2D closestPoint;
	Vector2DSet(&closestPoint, 0, 0);
	
//	SetVector2D(closestPoint, pRect->x, pRect->y);



if(pCenter->x > pRect->x + 0.5*Width)
{
	closestPoint.x = pRect->x + 0.5*Width;
}

else if (pCenter->x < pRect->x - 0.5*Width)
{
	closestPoint.x = pRect->x - 0.5*Width;
}

else
{
	closestPoint.x = pCenter->x;
}


if(pCenter->y > pRect->y + 0.5*Height)
{
	closestPoint.y = pRect->y + 0.5*Height;
}

else if(pCenter->y < pRect->y - 0.5*Height)
{
	closestPoint.y = pRect->y - 0.5*Height;
}

else
{
	closestPoint.y = pCenter->y;
}


int output = StaticPointToStaticCircle(&closestPoint, pCenter, Radius);

	return output;
}