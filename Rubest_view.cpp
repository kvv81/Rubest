/***************************************************************************
                          Rubest_view.cpp  -  description
                             -------------------
    begin                : Jun 17 11:38:41 EEST 2007
    copyright            : 2007 by Volodymyr Khomenko
    email                : khomenko.volodymyr at gmail.com

****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>

#include "CGlKubRub.h"

#define WND_ASPECT (4.0f/3.0f)
#define WND_YSIZE 768
#define WND_XSIZE ((int)(WND_YSIZE*WND_ASPECT))

#define ACCUM_FACTOR 0.1f /* 0.5f */

// GLUT demo programm
static bool is_motion_blur = false;
static bool was_resized = true;

void ReshapeFunc(int width,int height)
{
	float	w=((float)width)/WND_XSIZE,
		h=((float)height)/(WND_YSIZE*WND_ASPECT);

	glViewport(0,0,width,height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if(w <= h)
		glOrtho(-2.0,2.0,-2.0*(GLfloat)h/(GLfloat)w,
			2.0*(GLfloat)h/(GLfloat)w,-10.0,10.0);
	else
		glOrtho(-2.0*(GLfloat)w/(GLfloat)h,
			2.0*(GLfloat)w/(GLfloat)h,-2.0,2.0,-10.0,10.0);
	gluPerspective(20.0f,1.0f,0.1f,100.0f);
	glMatrixMode(GL_MODELVIEW);

	was_resized = true;
}
//-----------------------------------------------------------------

// Sequental function: should return the next operation;
// can store all required prev ops (or some state) inside.
SRubanOp GetNexpOp()
{
	// Repeat F=
	static SRubanOp F[] = {OP_R, OP_U, OP_F, OP_U_, OP_F_, OP_R_, OP_F};
	static int op_pos=0;

	const int cmb_size = sizeof(F)/sizeof(*F);

	if(op_pos >= cmb_size)
		return SRubanOp();

	return F[(op_pos++)%cmb_size];

	// Set next operation to random...
	//return SRubanOp(rand()%RUBAN_OPS_NUM);
}

extern SUnpackedRubanState inital_unpacked_ruban_state;
static float phi=0.0,psi=0.0;
void DrawScene()
{

#if 0
	if (is_motion_blur)
	{
		if(resized == 0)
		{
			if(use_accum_src)
			{
				glAccum(GL_MULT, 1.0f-ACCUM_FACTOR);
			}
			else
			{
#if 0
				// glClear(GL_ACCUM_BUFFER_BIT);
				glAccum(GL_LOAD, 1.0f-ACCUM_FACTOR);
#endif
			}
		}
	}
#endif

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	glTranslatef(0.0f,0.0f,-7.0f);
	glRotatef(phi,0.0,1.0,1.0);
	glRotatef(psi,1.0,0.0,0.0);

	static CGlKubRub kub;
	static int step=0;
	static SUnpackedRubanState cur_ruban_state;
	static SRubanOp cur_op(DEFAULT_RUBAN_OP);

#define OP_STEP 200

	if(step%OP_STEP==0)
	{  
		cur_op = GetNexpOp();
		if(cur_op.op != UNDEFINED_RUBAN_OP)
			kub.SetOperation(cur_op);
		std::cout << step << ", Setup cur_op=" << (int)cur_op.op << std::endl;
	}

	if(cur_op.op != UNDEFINED_RUBAN_OP)
	{
		//kub.SetOperationCompleteness(((float)(OP_STEP-abs(OP_STEP-((step++)%(2*OP_STEP)))))/OP_STEP);
		kub.SetOperationCompleteness(((float)(step%OP_STEP)) / OP_STEP);
	}
	else
		kub.SetOperationCompleteness(0.1);
	kub.Draw(cur_ruban_state);

	if((++step)%OP_STEP==0)
		if(cur_op.op != UNDEFINED_RUBAN_OP)
		{
			std::cout << step-1 << ", Execute cur_op=" << (int)cur_op.op << std::endl;
			cur_ruban_state.ExecuteOp(cur_op);
		}

#if 0
	/* Note - color is used only on
		GL_COLOR_MATERIAL enabled! */
	//glColor3f(0.4f, 0.0f, 0.0f);
	//glutWireTeapot(1.0);
	glutSolidTeapot(1.0);
#endif

	/*
	glBegin(GL_POLYGON);
	glVertex3f(1.0f, 1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, 0.0f);
	glVertex3f(-1.0f, 1.0f, 0.0f);
	glEnd();
	*/
	phi+=0.1f;	psi+=0.1f; /* Increment angles */
/*
	Demo.Step();
	Demo.Draw();
*/
	glFlush();

	if (is_motion_blur)
	{
		if(was_resized)
		{
			was_resized = false;
			glAccum(GL_LOAD, 1.0f-ACCUM_FACTOR);
		}
		else 
		{
			glAccum(GL_ACCUM, ACCUM_FACTOR);
			glAccum(GL_RETURN, 1.0);
			glAccum(GL_MULT, 1.0f-ACCUM_FACTOR);
		}
#if 0
		if(use_accum_src)
		{
			glAccum(GL_LOAD, 1.0f);
		}
		else
		{
			glAccum(GL_LOAD, 1.0f-ACCUM_FACTOR);
			// glAccum(GL_MULT, 1.0f-ACCUM_FACTOR);
		}
#endif
	}
	glutSwapBuffers();
}

void DrawFunc()
{
	DrawScene();
	//glutPostRedisplay();
}

void KeyFunc(unsigned char key,int x,int y)
{
	static int bFullSceen=0;

	if(key==27) // Esc=27
		exit(0);
	if(key == 'm')
	{
		is_motion_blur = !is_motion_blur;
		was_resized = true;
	}

#if 0
	if(key == 'a') use_accum_src = !use_accum_src;
#endif

	if(key==13) // Enter=13
	{

		if((glutGetModifiers()&GLUT_ACTIVE_ALT)==0) return;

		// Alt+Enter=FullScreen/Windowed
		if(bFullSceen)
		{
			bFullSceen=0;
			glutReshapeWindow(WND_XSIZE,WND_YSIZE);

			int xs=glutGet(GLUT_SCREEN_WIDTH), ys=glutGet(GLUT_SCREEN_HEIGHT);
			if(xs!=0 && ys!=0)
				glutPositionWindow((xs-WND_XSIZE)>>1,(ys-WND_YSIZE)>>1);
			glutSetCursor(GLUT_CURSOR_INHERIT);
		}
		else
		{
			bFullSceen=1;
			glutFullScreen();
			glutSetCursor(GLUT_CURSOR_NONE);
		}
		was_resized = true;
	}

	if(key=='+')
	{
		phi+=0.5f;
	}
	if(key=='-')
	{
		psi+=0.5f;
	}
}

void VisibilityFunc(int state)
{
	if(state==GLUT_VISIBLE)
		glutIdleFunc(&DrawFunc);
	else if(state==GLUT_NOT_VISIBLE)
		glutIdleFunc(NULL);
}

void MouseZoomFunc(int button,int state,int x,int y)
{
/*
	if(state==GLUT_UP)
		CDemo::mZoomDir=0;
	else if(button==GLUT_LEFT_BUTTON && state==GLUT_DOWN)
		CDemo::mZoomDir=1;
	else if(button==GLUT_RIGHT_BUTTON && state==GLUT_DOWN)
		CDemo::mZoomDir=-1;
*/
}

const GLfloat	L0Pos[]={-2.0f,2.0f,-3.0f,1.0f},

	/* White light source */
	L0Diffuse[]={1.0f,1.0f,1.0f},
	L0Ambient[]={1.0f,1.0f,1.0f},
	L0Specular[]={1.0f,1.0f,1.0f},
	
	/* Diffuse and ambient colors
	should be similiar for natural look! */
	M0Diffuse[]={0.6f,0.1f,0.1f},
	M0Ambient[]={0.2f,0.0f,0.0f},
	M0Specular[]={1.0f,1.0f,1.0f},  /* reddy-white reflection */
	
	M0Emission[]={0.1f, 0.1f, 0.1f, 1.0f},
	M0Shininess = 30.0f,  /* 10..100 is the best */
	
	GlobalLight[]={1.0f,1.0f,1.0f,1.0f};

//  Detect OS & specify main or WinMain
#ifdef _WIN32

#include <windows.h>
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	int argc=1;
	char *argv[]={"",NULL};
#else
int main(int argc, char *argv[])
{
#endif
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE| /*GLUT_SINGLE |*/GLUT_RGB|GLUT_DEPTH|GLUT_ACCUM);
	glutInitWindowSize(WND_XSIZE,WND_YSIZE);

	glutCreateWindow("Rubest_view application");

	glutReshapeFunc(&ReshapeFunc);
	glutDisplayFunc(&DrawScene);
	glutKeyboardFunc(&KeyFunc);
	glutMouseFunc(&MouseZoomFunc);
//	glutIdleFunc(&DrawFunc); // Automatically sets by VisibilityFunc
	glutVisibilityFunc(&VisibilityFunc);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_GEQUAL);
	glClearDepth(0.0f);
	//glEnable(GL_LINE_SMOOTH);
	//glShadeModel(GL_SMOOTH);
	glEnable(GL_AUTO_NORMAL);

/*
	glEnable(GL_DITHER);
	glEnable(GL_BLEND);
*/

/*
	// GL_EXP, GL_EXP2, GL_LINEAR
	glFogi(GL_FOG_MODE, GL_EXP);					// Fog Mode
//	glFogfv(GL_FOG_COLOR, fogColor);		// Set Fog Color
	glFogf(GL_FOG_DENSITY, 0.2f);					// How Dense Will The Fog Be
	glHint(GL_FOG_HINT, GL_DONT_CARE);		// Fog Hint Value
	glFogf(GL_FOG_START, 1.0f);						// Fog Start Depth
	glFogf(GL_FOG_END, 10.0f);						// Fog End Depth
	glEnable(GL_FOG);									    // Enables GL_FOG
*/

	//glEnable(GL_CULL_FACE); // View only front face!
	glDisable(GL_CULL_FACE); // View both faces

#if 1
	/* Which channel will be tracked as color of primitive.
		Disabled by default!  */
	glColorMaterial(GL_FRONT_AND_BACK,/*GL_DIFFUSE GL_SPECULAR*/ GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

	/* Note - combining ambient and diffuse colors is a bad idea... */
#endif

	//glLightModelfv(GL_LIGHT_MODEL_AMBIENT, GlobalLight);  // Small ambient foreground
#if 1
	glLightfv(GL_LIGHT0, GL_POSITION, L0Pos);
	glLightfv(GL_LIGHT0, GL_AMBIENT,  L0Ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE,  L0Diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, L0Specular);

	glEnable(GL_LIGHT0);

	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,GL_TRUE);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_TRUE); // Use light for both sides
	glEnable(GL_LIGHTING);  // Enable light AFTER light configuration !!!
#endif


#if 0
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   M0Ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   M0Diffuse);
#endif

#if 0
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  M0Specular);

	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION,  M0Emission);
	glMaterialf(GL_FRONT_AND_BACK,  GL_SHININESS, M0Shininess);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_POLYGON_SMOOTH);
#endif

	// Enable using vertex array for glDrawElements
	glEnableClientState(GL_VERTEX_ARRAY);
	glClear(GL_ACCUM_BUFFER_BIT);

	glutPostRedisplay(); /* In some cases glut doesn't post the first redisplay... */
	//glutFullScreen();
	glutMainLoop();
	return 0;
}

