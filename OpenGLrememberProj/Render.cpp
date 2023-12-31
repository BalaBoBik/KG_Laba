#include "Render.h"

#include <sstream>
#include <iostream>
#include <cmath>

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "GUItextRectangle.h"

bool textureMode = true;
bool lightMode = true;

//����� ��� ��������� ������
class CustomCamera : public Camera
{
public:
	//��������� ������
	double camDist;
	//���� �������� ������
	double fi1, fi2;

	
	//������� ������ �� ���������
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}

	
	//������� ������� ������, ������ �� ����� ��������, ���������� �������
	void SetUpCamera()
	{
		//�������� �� ������� ������ ������
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist*cos(fi2)*cos(fi1),
			camDist*cos(fi2)*sin(fi1),
			camDist*sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//������� ��������� ������
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //������� ������ ������


//����� ��� ��������� �����
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//��������� ������� �����
		pos = Vector3(1, 1, 3);
	}

	
	//������ ����� � ����� ��� ���������� �����, ���������� �������
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);

		
		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale*0.08;
		s.Show();
		
		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//����� �� ��������� ����� �� ����������
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//������ ���������
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale*1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// ��������� ��������� �����
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// �������������� ����������� �����
		// ������� ��������� (���������� ����)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// ��������� ������������ �����
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// ��������� ���������� ������������ �����
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //������� �������� �����




//������ ���������� ����
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//������ ���� ������ ��� ������� ����� ������ ����
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
	}

	
	//������� ���� �� ���������, � ����� ��� ����
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k*r.direction.X() + r.origin.X();
		y = k*r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02*dy);
	}

	
}

void mouseWheelEvent(OpenGL *ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01*delta;

}

void keyDownEvent(OpenGL *ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}
}

void keyUpEvent(OpenGL *ogl, int key)
{
	
}



GLuint texId;

//����������� ����� ������ ��������
void initRender(OpenGL *ogl)
{
	//��������� �������

	//4 ����� �� �������� �������
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//��������� ������ ��������� �������
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//�������� ��������
	glEnable(GL_TEXTURE_2D);
	

	//������ ����������� ���������  (R G B)
	RGBTRIPLE *texarray;

	//������ ��������, (������*������*4      4, ���������   ����, �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
	char *texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("texture.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);

	
	
	//���������� �� ��� ��������
	glGenTextures(1, &texId);
	//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
	glBindTexture(GL_TEXTURE_2D, texId);

	//��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//�������� ������
	free(texCharArray);
	free(texarray);

	//������� ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//������ � ���� ����������� � "������"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// ������������ �������� : �� ����� ����� ����� 1
	glEnable(GL_NORMALIZE);

	// ���������� ������������� ��� �����
	glEnable(GL_LINE_SMOOTH); 


	//   ������ ��������� ���������
	//  �������� GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  ������� � ���������� �������� ���������(�� ���������), 
	//                1 - ������� � ���������� �������������� ������� ��������       
	//                �������������� ������� � ���������� ��������� ����������.    
	//  �������� GL_LIGHT_MODEL_AMBIENT - ������ ������� ���������, 
	//                �� ��������� �� ���������
	// �� ��������� (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}



Vector3 GetNormal(double A [], double B [], double C[])
{
	Vector3 a = Vector3(A[0] - B[0], A[1] - B[1], A[2] - B[2]);
	Vector3 b = Vector3(C[0] - B[0], C[1] - B[1], C[2] - B[2]);
	double x, y, z;
	x = a.Y() * b.Z() - b.Y() * a.Z();
	y = -a.X() * b.Z() + b.X() * a.Z();
	z = a.X() * b.Y() - b.X() * a.Y();

	Vector3 n = Vector3(x, y, z);
	double len=n.length();
	n = Vector3(x / len, y / len, z / len);
	return n;
}

void Render(OpenGL *ogl)
{



	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


	//��������������
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//��������� ���������
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//�������
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//��������
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//����������
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); \
		//������ �����
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//���� ���� �������, ��� ����������� (����������� ���������)
	glShadeModel(GL_SMOOTH);
	//===================================
	//������� ���  


	//������ ��������� ���������� ��������
	double Mid[]{ 7.5,6,0 };
	double MidT[]{ 7.5,6,7 };

	double A[]{ 0,1,0 };
	double AT[]{ 0,1,7 };

	double B[]{ 1,9,0 };
	double BT[]{ 1,9,7 };

	double C[]{ 7,8,0 };
	double CT[]{ 7,8,7 };

	double D[]{ 11,14,0 };
	double DT[]{ 11,14,7 };

	double E[]{ 15,7,0 };
	double ET[]{ 15,7,7 };

	double F[]{ 12,3,0 };
	double FT[]{ 12,3,7 };

	double G[]{ 8,4,0 };
	double GT[]{ 8,4,7 };

	double H[]{ 7,0,0 };
	double HT[]{ 7,0,7 };

	glBindTexture(GL_TEXTURE_2D, texId);
	glBegin(GL_QUADS);

	glColor3d(0, 0, 1);

	double mid2[]{ (A[0] + B[0]) / 2,(A[1] + B[1]) / 2 };

	float radius2 = sqrt(pow(A[0] - B[0], 2) + pow(A[1] - B[1], 2)) / 2;
	float twoPI = 2 * PI;
	for (float i = - 0.124354994546761; i <= - 0.124354994546761 + PI - 0.001; i += 0.001)
	{
		double Point1[]{ (sin(i) * radius2) + mid2[0],  -(cos(i) * radius2) + mid2[1], 0 };
		double Point1T[]{ (sin(i) * radius2) + mid2[0], -(cos(i) * radius2) + mid2[1], 7 };
		double Point2T[]{ (sin(i + 0.001) * radius2) + mid2[0], -(cos(i + 0.001) * radius2) + mid2[1], 7 };
		double Point2[]{ (sin(i + 0.001) * radius2) + mid2[0], -(cos(i + 0.001) * radius2) + mid2[1], 0 };

		Vector3 normal = GetNormal(Point1, Point2, Point2T);
		glNormal3d(normal.X(), normal.Y(), normal.Z());
		glVertex3dv(Point1);
		glVertex3dv(Point1T);
		glVertex3dv(Point2T);
		glVertex3dv(Point2);
	}

	

	glColor3d(0, 0.5, 0.5);
	
	Vector3 normal = GetNormal(B, C, CT);
	glNormal3d(normal.X(), normal.Y(), normal.Z());
	glVertex3dv(B);
	glVertex3dv(BT);
	glVertex3dv(CT);
	glVertex3dv(C);

	normal = GetNormal(C, D, DT);
	glNormal3d(normal.X(), normal.Y(), normal.Z());
	glVertex3dv(C);
	glVertex3dv(CT);
	glVertex3dv(DT);
	glVertex3dv(D);

	glEnd();

	glColor3d(0, 1, 0);

	glBegin(GL_QUADS);
	double mid1[]{ (D[0] + E[0]) / 2,(D[1] + E[1]) / 2 };

	float radius1 = sqrt(pow(D[0] - E[0], 2) + pow(D[1] - E[1], 2) )/2;
	twoPI = 2 * PI;
	for (float i = PI/2- 1.05165021254837; i <= PI / 2 - 1.05165021254837 +PI-0.001; i += 0.001)
	{
		double Point1[] { (sin(i) * radius1) + mid1[0],  -(cos(i) * radius1) + mid1[1], 0 };
		double Point1T[]{ (sin(i) * radius1) + mid1[0], -(cos(i) * radius1) + mid1[1], 7 };
		double Point2T[] { (sin(i + 0.001) * radius1) + mid1[0], -(cos(i + 0.001) * radius1) + mid1[1], 7 };
		double Point2[] { (sin(i + 0.001) * radius1) + mid1[0], -(cos(i + 0.001) * radius1) + mid1[1], 0 };

		normal = GetNormal(Point2 ,Point1,Point1T);
		glNormal3d(normal.X(), normal.Y(), normal.Z());
		glVertex3dv(Point1);
		glVertex3dv(Point1T);
		glVertex3dv(Point2T);
		glVertex3dv(Point2);
	}
		
	glEnd();

	glBegin(GL_QUADS);

	normal = GetNormal(E, F, FT);
	glNormal3d(normal.X(), normal.Y(), normal.Z());
	glColor3d(0, 0.5, 0.5);
	glVertex3dv(E);
	glVertex3dv(ET);
	glVertex3dv(FT);
	glVertex3dv(F);

	normal = GetNormal(F, G, GT);
	glNormal3d(normal.X(), normal.Y(), normal.Z());
	glVertex3dv(F);
	glVertex3dv(FT);
	glVertex3dv(GT);
	glVertex3dv(G);

	normal = GetNormal(G, H, HT);
	glNormal3d(normal.X(), normal.Y(), normal.Z());
	glVertex3dv(G);
	glVertex3dv(GT);
	glVertex3dv(HT);
	glVertex3dv(H);

	normal = GetNormal(H, A, AT);
	glNormal3d(normal.X(), normal.Y(), normal.Z());
	glVertex3dv(H);
	glVertex3dv(HT);
	glVertex3dv(AT);
	glVertex3dv(A);

	glEnd();

	
	glBegin(GL_POLYGON);
	normal = GetNormal(A, Mid, B);
	glNormal3d(normal.X(), normal.Y(), normal.Z());
	glColor3d(0, 0.7, 0.7);
	glVertex3dv(Mid);
	glVertex3dv(C);
	glVertex3dv(D);

	for (float i = PI / 2 - 1.05165021254837; i <= PI / 2 - 1.05165021254837 + PI; i += 0.001)
	{
		double Point1[]{ (sin(i) * radius1) + mid1[0],  -(cos(i) * radius1) + mid1[1], 0 };
		glVertex3dv(Point1);
	}

	glVertex3dv(E);
	glVertex3dv(F);
	glVertex3dv(G);
	glVertex3dv(H);
	glEnd();

	glBegin(GL_POLYGON);

	normal = GetNormal(A, Mid, B);
	glNormal3d(normal.X(), normal.Y(), normal.Z());
	glColor3d(0, 0.7, 0.7);
	glVertex3dv(H);

	for (float i = -0.124354994546761; i <= -0.124354994546761 + PI/2 - 0.001; i += 0.001)
	{
		double Point1[]{ (sin(i) * radius2) + mid2[0],  -(cos(i) * radius2) + mid2[1], 0 };
		glVertex3dv(Point1);
	}

	glEnd();
	glBegin(GL_POLYGON);

	normal = GetNormal(A, Mid, B);
	glNormal3d(normal.X(), normal.Y(), normal.Z());
	glColor3d(0, 0.7, 0.7);
	glVertex3dv(C);

	for (float i = -0.124354994546761 + PI / 2; i <= -0.124354994546761 + PI ; i += 0.001)
	{
		double Point1[]{ (sin(i) * radius2) + mid2[0],  -(cos(i) * radius2) + mid2[1], 0 };
		glVertex3dv(Point1);
	}

	glEnd();
	glBegin(GL_POLYGON);

	normal = GetNormal(A, Mid, B);
	glNormal3d(normal.X(), normal.Y(), normal.Z());
	glColor3d(0, 0.7, 0.7);
	glVertex3dv(Mid);
	glVertex3dv(C);
	glVertex3d((sin(-0.124354994546761 + PI / 2) * radius2) + mid2[0], -(cos(-0.124354994546761 + PI / 2) * radius2) + mid2[1], 0 );
	glVertex3dv(H);
	glVertex3dv(Mid);
	glEnd();

	
	
	glBegin(GL_POLYGON);
	normal = GetNormal(BT, MidT, AT);
	glNormal3d(normal.X(), normal.Y(), normal.Z());
	glTexCoord2f(0, 0);
	glColor3d(0, 1, 1);
	glVertex3dv(MidT);
	//glVertex3dv(AT);

	//for (float i = -0.124354994546761; i <= -0.124354994546761 + PI - 0.001; i += 0.001)
	//{
		//double Point1[]{ (sin(i) * radius2) + mid2[0],  -(cos(i) * radius2) + mid2[1], 7 };
		//glVertex3dv(Point1);
	//}

	//glVertex3dv(BT);
	glVertex3dv(CT);
	glVertex3dv(DT);

	for (float i = PI / 2 - 1.05165021254837; i <= PI / 2 - 1.05165021254837 + PI; i += 0.001)
	{
		double Point1[]{ (sin(i) * radius1) + mid1[0],  -(cos(i) * radius1) + mid1[1], 7 };
		glVertex3dv(Point1);
	}

	glVertex3dv(ET);
	glVertex3dv(FT);
	glVertex3dv(GT);
	glVertex3dv(HT);
	//glVertex3dv(AT);
	glEnd();


	glBegin(GL_POLYGON);

	normal = GetNormal(BT, MidT, AT);
	glNormal3d(normal.X(), normal.Y(), normal.Z());
	glColor3d(0, 1, 1);
	glVertex3dv(HT);

	for (float i = -0.124354994546761; i <= -0.124354994546761 + PI / 2 - 0.001; i += 0.001)
	{
		double Point1[]{ (sin(i) * radius2) + mid2[0],  -(cos(i) * radius2) + mid2[1], 7 };
		glVertex3dv(Point1);
	}

	glEnd();
	glBegin(GL_POLYGON);

	normal = GetNormal(BT, MidT, AT);
	glNormal3d(normal.X(), normal.Y(), normal.Z());
	glColor3d(0, 1, 1);
	glVertex3dv(CT);

	for (float i = -0.124354994546761 + PI / 2; i <= -0.124354994546761 + PI; i += 0.001)
	{
		double Point1[]{ (sin(i) * radius2) + mid2[0],  -(cos(i) * radius2) + mid2[1], 7 };
		glVertex3dv(Point1);
	}

	glEnd();
	glBegin(GL_POLYGON);

	normal = GetNormal(BT, MidT, AT);
	glNormal3d(normal.X(), normal.Y(), normal.Z());
	glColor3d(0, 1, 1);
	glVertex3dv(MidT);
	glVertex3dv(CT);
	glVertex3d((sin(-0.124354994546761 + PI / 2) * radius2) + mid2[0], -(cos(-0.124354994546761 + PI / 2) * radius2) + mid2[1], 7);
	glVertex3dv(HT);
	glVertex3dv(MidT);
	glEnd();

	//����� ��������� ���������� ��������


   //��������� ������ ������

	
	glMatrixMode(GL_PROJECTION);	//������ �������� ������� ��������. 
	                                //(���� ��������� ��������, ����� �� ������������.)
	glPushMatrix();   //��������� ������� ������� ������������� (������� ��������� ������������� ��������) � ���� 				    
	glLoadIdentity();	  //��������� ��������� �������
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //������� ����� ������������� ��������

	glMatrixMode(GL_MODELVIEW);		//������������� �� �����-��� �������
	glPushMatrix();			  //��������� ������� ������� � ���� (��������� ������, ����������)
	glLoadIdentity();		  //���������� �� � ������

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //������� ����� ��������� ��� ������� ������ � �������� ������.
	rec.setSize(300, 150);
	rec.setPosition(10, ogl->getHeight() - 150 - 10);


	std::stringstream ss;
	ss << "T - ���/���� �������" << std::endl;
	ss << "L - ���/���� ���������" << std::endl;
	ss << "F - ���� �� ������" << std::endl;
	ss << "G - ������� ���� �� �����������" << std::endl;
	ss << "G+��� ������� ���� �� ���������" << std::endl;
	ss << "�����. �����: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "�����. ������: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "��������� ������: R="  << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;
	
	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //��������������� ������� �������� � �����-��� �������� �� �����.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}