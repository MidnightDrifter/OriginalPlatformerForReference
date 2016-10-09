#include "Matrix2D.h"
#include <math.h>

#define PI      3.1415926535897932384626433832795
/*
This function sets the matrix Result to the identity matrix
*/
void Matrix2DIdentity(Matrix2D *pResult)
{
	int i, j;
	for (i = 0; i<3; i++)
	{
		for (j = 0; j<3; j++)
		{
			if (i == j)
			{
				pResult->m[i][j] = 1;
			}
			else
			{
				pResult->m[i][j] = 0;
			}
		}
	}

}

// ---------------------------------------------------------------------------

/*
This functions calculated the transpose matrix of Mtx and saves it in Result
*/
void Matrix2DTranspose(Matrix2D *pResult, Matrix2D *pMtx)
{


	Matrix2D tempMat;
	Matrix2DIdentity(&tempMat);


	tempMat.m[0][0] = pMtx->m[0][0];

	tempMat.m[1][0] = pMtx->m[0][1];
	tempMat.m[0][1] = pMtx->m[1][0];


	tempMat.m[2][2] = pMtx->m[2][2];

	tempMat.m[0][2] = pMtx->m[2][0];
	tempMat.m[2][1] = pMtx->m[1][2];
	tempMat.m[2][0] = pMtx->m[0][2];
	tempMat.m[1][2] = pMtx->m[2][1];
	tempMat.m[1][1] = pMtx->m[1][1];

	int i, j;

	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
		{
			float otherTemp;
			otherTemp = tempMat.m[i][j];
			pResult->m[i][j] = otherTemp;
		}
	}



}

// ---------------------------------------------------------------------------

/*
This function multiplies Mtx0 with Mtx1 and saves the result in Result
Result = Mtx0*Mtx1
*/
void Matrix2DConcat(Matrix2D *pResult, Matrix2D *pMtx0, Matrix2D *pMtx1)
{

	Matrix2D temp1, temp2, tempOut;

	int t1, t2, t3;

	for (t1 = 0; t1 < 3; t1 = t1 + 1)
	{
		for (t2 = 0; t2 < 3; t2 = t2 + 1)
		{
			temp1.m[t1][t2] = pMtx0->m[t1][t2];
			temp2.m[t1][t2] = pMtx1->m[t1][t2];
			tempOut.m[t1][t2] = 0;

		}


	}




	for (t1 = 0; t1 < 3; t1 = t1 + 1)
	{
		for (t2 = 0; t2 < 3; t2 = t2 + 1)
		{
			for (t3 = 0; t3 < 3; t3 = t3 + 1)
			{
				tempOut.m[t1][t2] = tempOut.m[t1][t2] + (temp1.m[t1][t3] * temp2.m[t3][t2]);
			}

		}


	}

	for (t1 = 0; t1 < 3; t1 = t1 + 1)
	{
		for (t2 = 0; t2 < 3; t2 = t2 + 1)
		{
			pResult->m[t1][t2] = tempOut.m[t1][t2];
		}
	}


















	/*

	Matrix2D a, b, c;*/
	/* , t1, output;*/

	/*Matrix2DIdentity(&t1);*/
	/*float x, y;
	int g, o, z;
	for (g = 0; g < 3; g++)
	{
	for (o = 0; o < 3; o = o + 1)
	{

	for (z = 0; z < 3; z++)
	{
	c.m[g][o] = 0;
	}
	}
	}



	for (g = 0; g < 3; g++)
	{
	for (o = 0; o < 3; o = o + 1)
	{

	for (z = 0; z < 3; z++)
	{
	printf("Value at a[%i][%i]:  %f:  \n", g, z, a.m[g][z]);
	printf("Value at b[%i][%i]:  %f:  \n", g, z, b.m[g][z]);
	printf("Value at c[%i][%i]:  %f:  \n", g, z, c.m[g][z]);
	c.m[g][o] = c.m[g][o] + a.m[g][z] * b.m[z][o];

	}

	}
	}


	for (g = 0; g < 3; g++)
	{
	for (o = 0; o < 3; o++)
	{
	printf("Value of (%i, %i):  %f \n", g, o, c.m[g][o]);

	}
	}



	for (g = 0; g < 3; g++)
	{
	for (o = 0; o < 3; o = o + 1)
	{



	pResult->m[g][o] = c.m[g][o];



	}
	}
	*/

	/*
	for (a = 0; a < 3; a++)
	{
	for (b = 0; b < 3; b++)
	{
	x = pMtx0->m[a][b];
	y = pMtx1->m[a][b];

	t0.m[a][b] = x;
	t1.m[a][b] = y;

	}

	}

	*/
	/*float temp;
	for (a = 0; a < 3; a++)
	{
	for (b = 0; b < 3; b=b+1)
	{
	temp = 0;
	/*t0.m[a][b] = 0;
	for (c = 0; c < 3; c++)
	{
	x = (pMtx0->m[a][c]);
	y = (pMtx1->m[c][b]);
	temp = temp +  x*y;
	/*t0.m[a][b] += x*y;
	}
	t0.m[a][b] = temp;
	}

	}

	*pResult = t0;
	*/
	/*
	for (a = 0; a < 3; a++)
	{
	for (b = 0; b < 3; b++)
	{
	x = output.m[a][b];
	pResult->m[a][b] = x;
	}

	}

	*/





	/*
	for(int i=0; i<=Array.getLength(m.getMatrix())-1;i++)

	{ for(int j=0;j<=Array.getLength(m.getMatrixRow(0))-1;j++)
	{Triple t = new Triple();
	for(int k = 0; k<=Array.getLength(m.getMatrixRow(0))-1;k++)
	{t=t.Add(this.getMatrixElement(i, k).Multiply(m.getMatrixElement(k,j)));}
	temp.setMatrixElement(i, j, t);

	}



	Matrix2D tempMat;
	Matrix2DIdentity(&tempMat);

	int i, j, k;
	for(i=0; i<3; i=i+1)
	{
	for(j=0; j<3; j=j+1)
	{
	float a, b, temp = 0;
	for(k=0; k<3; k=k+1)
	{a = pMtx0->m[i][k];
	b = pMtx1->m[k][j];


	temp = temp + a*b;
	printf("Value of first matrix at (%i, %i):  %f\n", i, k, a);
	printf("Value of second matrix at(%i, %i):  %f\n", k, j, b);

	}
	tempMat.m[i][j]=temp;
	printf("Current output value at (%i, %i):  %f\n",i, j, temp);
	}
	}

	for (i = 0; i < 3; i++)
	{
	for (j = 0; j < 3; j++)
	{
	float otherTemp;
	otherTemp = tempMat.m[i][j];
	pResult->m[i][j] = otherTemp;


	/*printf("Matrix 0 value at (%i, %i):  %f \n", i, j, pMtx0->m[i][j]);
	printf("Matrix 1 value at (%i, %i):  %f \n", i, j, pMtx1->m[i][j]);
	printf("Result value at (%i, %i):  %f \n", i, j, pResult->m[i][j]);

	}
	}

	/*
	/*

	Matrix2D tempMat;
	Matrix2DIdentity(&tempMat);
	int i, j, k;
	float  t, m1, m2;
	for (i = 0; i < 3; i++)
	{
	for (j = 0; j < 3; j++)
	{
	t = 0;
	for (k = 0; k < 3; k++)
	{
	m1 = pMtx0->m[i][k];
	m2 = pMtx1->m[k][j];

	t += (m1*m2);




	}
	tempMat.m[i][j] = t;
	}

	}

	int a, b;
	float r;
	for (a = 0; a < 3; a++)
	{
	for (b = 0; b < 3; b++)
	{
	r = tempMat.m[a][b];
	pResult->m[a][b] = r;

	}
	}




	pResult->m[0][0] = pMtx0->m[1][1] + pMtx1->m[1][1];


	pResult->m[0][0] =    ((pMtx0->m[0][0] * pMtx1->m[0][0]) + (pMtx0->m[0][1] * pMtx1->m[1][0]) + (pMtx0->m[0][2] * pMtx1->m[2][0]));
	pResult->m[0][1] = (pMtx0->m[0][0] * pMtx1->m[0][1]) + (pMtx0->m[0][1] * pMtx1->m[1][1]) + (pMtx0->m[0][2] * pMtx1->m[2][1]);
	pResult->m[0][2] = pMtx0->m[0][0] * pMtx1->m[0][2] + pMtx0->m[0][1] * pMtx1->m[1][2] + pMtx0->m[0][2] * pMtx1->m[2][2];
	pResult->m[1][0] = pMtx0->m[1][0] * pMtx1->m[0][0] + pMtx0->m[1][1] * pMtx1->m[1][0] + pMtx0->m[1][2] * pMtx1->m[2][0];
	pResult->m[1][1] = pMtx0->m[1][0] * pMtx1->m[0][1] + pMtx0->m[1][1] * pMtx1->m[1][1] + pMtx0->m[1][2] * pMtx1->m[2][1];
	pResult->m[1][2] = pMtx0->m[1][0] * pMtx1->m[0][2] + pMtx0->m[1][1] * pMtx1->m[1][2] + pMtx0->m[1][2] * pMtx1->m[2][2];
	pResult->m[2][0] = pMtx0->m[2][0] * pMtx1->m[0][0] + pMtx0->m[2][1] * pMtx1->m[1][0] + pMtx0->m[2][2] * pMtx1->m[0][2];
	pResult->m[2][1] = pMtx0->m[2][0] * pMtx1->m[0][1] + pMtx0->m[2][1] * pMtx1->m[1][1] + pMtx0->m[2][2] * pMtx1->m[2][1];
	pResult->m[2][2] = pMtx0->m[2][0] * pMtx1->m[0][2] + pMtx0->m[2][1] * pMtx1->m[2][1] + pMtx0->m[2][2] * pMtx1->m[2][2];
	*/
	/*
	Matrix2D tempMat;
	Matrix2DIdentity(&tempMat);

	float mat000, mat001, mat002, mat010, mat011, mat012, mat020, mat021, mat022, mat100, mat101, mat102, mat110, mat111, mat112, mat120, mat121, mat122;

	mat000 = pMtx0->m[0][0];
	mat001 = pMtx0->m[0][1];
	mat002 = pMtx0->m[0][2];
	mat010 = pMtx0->m[1][0];
	mat011 = pMtx0->m[1][1];
	mat012 = pMtx0->m[1][2];
	mat020 = pMtx0->m[2][0];
	mat021 = pMtx0->m[2][1];
	mat022 = pMtx0->m[2][2];

	printf("Value of m0(0,0): %f \n", mat000);
	printf("Value of m0(0,1): %f \n", mat001);
	printf("Value of m0(0,2): %f \n", mat002);
	printf("Value of m0(1,0): %f \n", mat010);
	printf("Value of m0(1,1): %f \n", mat011);
	printf("Value of m0(1,2): %f \n", mat012);
	printf("Value of m0(2,0): %f \n", mat020);
	printf("Value of m0(2,1): %f \n", mat021);
	printf("Value of m0(2,2): %f \n", mat022);


	mat100 = pMtx1->m[0][0];
	mat101 = pMtx1->m[0][1];
	mat102 = pMtx1->m[0][1];
	mat110 = pMtx1->m[1][0];
	mat111 = pMtx1->m[1][1];
	mat112 = pMtx1->m[1][2];
	mat120 = pMtx1->m[2][0];
	mat121 = pMtx1->m[2][1];
	mat122 = pMtx1->m[2][2];


	printf("Value of m1(0,0): %f \n", mat100);
	printf("Value of m1(0,1): %f \n", mat101);
	printf("Value of m1(0,2): %f \n", mat102);
	printf("Value of m1(1,0): %f \n", mat110);
	printf("Value of m1(1,1): %f \n", mat111);
	printf("Value of m1(1,2): %f \n", mat112);
	printf("Value of m1(2,0): %f \n", mat120);
	printf("Value of m1(2,1): %f \n", mat121);
	printf("Value of m1(2,2): %f \n", mat122);


	tempMat.m[0][0] = (mat000*mat100) + (mat001)*(mat110)+(mat002 * mat120);
	tempMat.m[0][1] = (mat000*mat101) + (mat011)*(mat111)+(mat002 * mat121);
	tempMat.m[0][2] = (mat000*mat102) + (mat001)*(mat112)+(mat002 * mat122);
	tempMat.m[1][0] = (mat010*mat100) + (mat011)*(mat110)+(mat012 * mat120);
	tempMat.m[1][1] = (mat010*mat101) + (mat011)*(mat111)+(mat012 * mat121);
	tempMat.m[1][2] = (mat010*mat102) + (mat011)*(mat112)+(mat012 * mat122);
	tempMat.m[2][0] = (mat020*mat100) + (mat021)*(mat110)+(mat022 * mat120);
	tempMat.m[2][1] = (mat020*mat101) + (mat021)*(mat111)+(mat022 * mat121);
	tempMat.m[2][2] = (mat020*mat102) + (mat021)*(mat112)+(mat022 * mat120);


	int i, j;
	float t;
	for (i = 0; i < 3; i++)
	{
	for (j = 0; j < 3; j++)
	{
	t = tempMat.m[i][j];
	pResult->m[i][j] = t;
	printf("Value of result matrix at m(%i, %i):  %f \n", i, j, t);


	}
	}
	*/

}

// ---------------------------------------------------------------------------

/*
This function creates a translation matrix from x *p y and saves it in Result     DOUBLE CHECK THIS WITH PROF
*/
void Matrix2DTranslate(Matrix2D *pResult, float x, float y)
{
	int i, j;

	for (i = 0; i<3; i++)
	{
		for (j = 0; j<3; j++)
		{
			if (i == j)
			{
				pResult->m[i][j] = 1;
			}

			else
			{
				pResult->m[i][j] = 0;
			}

		}
	}

	pResult->m[2][2] = 1;
	pResult->m[0][2] = x;
	pResult->m[1][2] = y;

}

// ---------------------------------------------------------------------------

/*
This function creates a scaling matrix from x *p y and saves it in Result
*/
void Matrix2DScale(Matrix2D *pResult, float x, float y)
{

	int i, j;
	for (i = 0; i<3; i++)
	{
		for (j = 0; j<3; j++)
		{
			if (i == 0 && j == 0)
			{
				pResult->m[i][j] = x;
			}
			else if (i == 1 && j == 1)
			{
				pResult->m[i][j] = y;
			}

			else if (i == 2 && j == 2)
			{
				pResult->m[i][j] = 1;
			}

			else
			{
				pResult->m[i][j] = 0;
			}
		}
	}

}

// ---------------------------------------------------------------------------

/*
This matrix creates a rotation matrix from "Angle" whose value is in degree.
Save the resultant matrix in Result
*/
void Matrix2DRotDeg(Matrix2D *pResult, float Angle)
{
	float a = (PI*Angle) / 180;
	Matrix2DRotRad(pResult, a);






}

// ---------------------------------------------------------------------------

/*
This matrix creates a rotation matrix from "Angle" whose value is in radian.
Save the resultant matrix in Result
*/
void Matrix2DRotRad(Matrix2D *pResult, float Angle)
{





	pResult->m[0][0] = cosf(Angle);
	pResult->m[0][1] = (-1)*sinf(Angle);
	pResult->m[0][2] = 0;
	pResult->m[1][0] = sinf(Angle);
	pResult->m[1][1] = cosf(Angle);
	pResult->m[1][2] = 0;
	pResult->m[2][0] = 0;
	pResult->m[2][1] = 0;
	pResult->m[2][2] = 1;




}

// ---------------------------------------------------------------------------

/*
This function multiplies the matrix Mtx with the vector Vec and saves the result in Result
Result = Mtx * Vec
*/
void Matrix2DMultVec(Vector2D *pResult, Matrix2D *pMtx, Vector2D *pVec)
{


	int i;
	float weight = 0;
	for (i = 0; i<3; i++)
	{
		weight = weight + pMtx->m[2][i];
	}

	float x1, x2, x3, y3, y1, y2, xVec, yVec, outX, outY;
	xVec = pVec->x;
	yVec = pVec->y;
	x1 = pMtx->m[0][0] * xVec;
	x2 = pMtx->m[0][1] * yVec;
	x3 = pMtx->m[0][2];

	yVec = pVec->y;
	y1 = pMtx->m[1][0] * xVec;
	y2 = pMtx->m[1][1] * yVec;
	y3 = pMtx->m[1][2];



	outX = (1 / weight)*(x1 + x2 + x3);
	outY = (1 / weight)*(y1 + y2 + y3);


	pResult->x = outX;
	pResult->y = outY;




	/*
	pResult->x = (pMtx->m[0][0] * pVec->x + pMtx->m[0][1] * pVec->y + pMtx->m[0][2])/weight;
	pResult->y = (pMtx->m[1][0] * pVec->x + pMtx->m[1][1] * pVec->y + pMtx->m[1][2])/weight;
	*/
}

// ---------------------------------------------------------------------------
