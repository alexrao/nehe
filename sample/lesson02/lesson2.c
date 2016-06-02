//
// This code was created by Jeff Molofee '99 (ported to Linux/GLUT by Richard Campbell '99)
//
// If you've found this code useful, please let me know.
//
// Visit me at www.demonews.com/hosted/nehe
// (email Richard Campbell at ulmont@bellsouth.net)
//
#include <GL/glut.h>    // Header File For The GLUT Library
#include <GL/gl.h>	// Header File For The OpenGL32 Library
#include <GL/glu.h>	// Header File For The GLu32 Library
#include <unistd.h>     // Header File For sleeping.
#include <stdio.h>
#include <stdarg.h>

/* ASCII code for the escape key. */
#define KEY_ESC         27
#define KEY_SHIFT       10
#define KEY_TAB         9
#define KEY_CAP         20
#define KEY_ALT         18
#define KEY_SPACE       1
#define KEY_BACKSPACE   8
#define KEY_LEFT        37
#define KEY_UP          38
#define KEY_RIGHT       39
#define KEY_DOWN        40
#define KEY_Q           'Q'
#define KEY_q           'q'
#define KEY_f           'f'
#define KEY_F           'F'
#define KEY_DEC          45
#define KEY_ADD           61

static int g_full_screen_flag = 0;
struct Rect
{
    int x,y,w,h;
};
struct Rect g_win_rect = {100, 0, 1280, 720};
float g_move_y = 0, g_rotated_count = 0;
float g_scale_count = 1.0f;

/* The number of our GLUT window */
int window;



#define print(...) print_debug(__FILE__, __FUNCTION__,__LINE__, ## __VA_ARGS__)
void print_debug(const char *file, const char *funtion, int linenum,const char *fmt, ...)
{
    char buf[512]={0};
    int offset = 0;
    int level = 0;
    if(fmt[0] > 0 && fmt[0] < 7)
    {
        offset = 1;
        if(fmt[0] <= 1)//DEBUG_HIDDEN
        {
            goto end;
        }
        else
        {
            level = fmt[0];
        }
    }
    va_list ap;
    va_start (ap, fmt);
    vsnprintf(buf,512,fmt,ap);
    va_end (ap);

    fprintf(stderr,"[%s] %d <%s>: ", file, linenum, funtion);

    if(level == 3 || level == 4 || level == 5 || level == 6)//DEBUG_FATAL DEBUG_LIGHT
    {
        //fprintf(stderr,"\033[31;46;5m");//闪烁
        if(level == 3 || level == 4)
        {
            fprintf(stderr,"\033[31;46;1m");
        }
        else
        {
            fprintf(stderr,"\033[31;42;1m");
        }
    }
    fprintf(stderr,"%s",buf + offset);

    if(level == 3 || level == 4 || level == 5 || level == 6)//DEBUG_FATAL DEBUG_LIGHT
    {
        fprintf(stderr,"\033[39;49;0m");
    }
    fprintf(stderr,"\n");
    if(level == 5)//DEBUG_FATAL
    {
        if(level == 5)
        {
            fprintf(stderr,"\n");
            fprintf(stderr,"\033[31;46;5m");//闪烁
            fprintf(stderr,"======================FATAL ERROR======================");
            fprintf(stderr,"\033[39;49;0m");
            fprintf(stderr,"\n");
        }
    }
end:
    return;
}

/* A general OpenGL initialization function.  Sets all of the initial parameters. */
void InitGL(int Width, int Height)	        // We call this right after our OpenGL window is created.
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);		// This Will Clear The Background Color To Black
    glClearDepth(1.0);				// Enables Clearing Of The Depth Buffer
    glDepthFunc(GL_LESS);				// The Type Of Depth Test To Do
    glEnable(GL_DEPTH_TEST);			// Enables Depth Testing
    glShadeModel(GL_SMOOTH);			// Enables Smooth Color Shading

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();				// Reset The Projection Matrix

    gluPerspective(45.0f,(GLfloat)Width/(GLfloat)Height,0.1f,100.0f);	// Calculate The Aspect Ratio Of The Window

    glMatrixMode(GL_MODELVIEW);
}

/* The function called when our window is resized (which shouldn't happen, because we're fullscreen) */
void ReSizeGLScene(int Width, int Height)
{
    if (Height==0)				// Prevent A Divide By Zero If The Window Is Too Small
        Height=1;

    glViewport(0, 0, Width, Height);		// Reset The Current Viewport And Perspective Transformation

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(45.0f,(GLfloat)Width/(GLfloat)Height,0.1f,100.0f);
    glMatrixMode(GL_MODELVIEW);
}

/* The main drawing function. */
void DrawGLScene()
{
    static int count = 0;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear The Screen And The Depth Buffer
    print("count = %d\n", count++);
    glLoadIdentity();				// Reset The View

    glTranslatef(-1.5f,0.0f,-6.0+1.0f*g_scale_count);		// Move Left 1.5 Units And Into The Screen 6.0
    glRotatef(g_rotated_count,0.0f, 1.0f,0.0f);
    //glScaled(1.0f*g_scale_count,1.0f*g_scale_count,1.0f*g_scale_count);

    // draw a triangle
    //glBegin(GL_POLYGON);				// start drawing a polygon
    glBegin(GL_TRIANGLES);				// start drawing a polygon
    glVertex3f( 0.0f, 1.0f, 0.0f);		// Top
    glVertex3f( 1.0f,-1.0f, 0.0f);		// Bottom Right
    glVertex3f(-1.0f,-1.0f, 0.0f);		// Bottom Left
    glEnd();					// we're done with the polygon

    glTranslatef(3.0f,0.0f,0.0f);		        // Move Right 3 Units
    glRotatef(g_rotated_count,5.0f, 1.0f,0.0f);

    // draw a square (quadrilateral)
    glBegin(GL_QUADS);				// start drawing a polygon (4 sided)
    glVertex3f(-1.0f, 1.0f, 0.0f);		// Top Left
    glVertex3f( 1.0f, 1.0f, 0.0f);		// Top Right
    glVertex3f( 1.0f,-1.0f, 0.0f);		// Bottom Right
    glVertex3f(-1.0f,-1.0f, 0.0f);		// Bottom Left
    glEnd();					// done with the polygon

    // swap buffers to display, since we're double buffered.
    glutSwapBuffers();
}

/* The function called whenever a key is pressed. */
void keyPressed(unsigned char key, int x, int y)
{
    /* avoid thrashing this procedure */
    usleep(100);

    //print("key = %d\n", key);

    /* If escape is pressed, kill everything. */
    if (key == KEY_ESC
            || key == KEY_Q
            || key == KEY_q
       )
    {
        /* shut down our window */
        glutDestroyWindow(window);

        /* exit the program...normal termination. */
        exit(0);
    }
    else if(key == KEY_F
            || key == KEY_f
           )
    {
        if(g_full_screen_flag)
        {
            g_full_screen_flag = 0;
            glutReshapeWindow(g_win_rect.w,  g_win_rect.h);
            glutPositionWindow(g_win_rect.x, g_win_rect.y);

        }
        else
        {
            g_full_screen_flag = 1;
            glutFullScreen();
        }
    }
    else if(key == KEY_DEC || key == KEY_ADD)
    {
        if(key == KEY_ADD)
        {
            g_scale_count += 0.1f;
        }
        else
        {
            g_scale_count -= 0.1f;
        }
    }
    else if(key == 'w')
    {
        g_move_y += 0.2f;
    }
    else if(key == 's')
    {
        g_move_y -= 0.2f;
    }
    else if(key == 'a')
    {
        g_rotated_count += 1.0f;
    }
    else if(key == 'd')
    {
        g_rotated_count -= 1.0f;
    }
    DrawGLScene();
}

int main(int argc, char **argv)
{
    /* Initialize GLUT state - glut will take any command line arguments that pertain to it or
       X Windows - look at its documentation at http://reality.sgi.com/mjk/spec3/spec3.html */
    glutInit(&argc, argv);

    print( "start");
    /* Select type of Display mode:
       Double buffer
       RGBA color
       Alpha components supported
       Depth buffer */
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);

    /* get a 640 x 480 window */
    glutInitWindowSize(g_win_rect.w,  g_win_rect.h);

    /* the window starts at the upper left corner of the screen */
    glutInitWindowPosition(g_win_rect.x,  g_win_rect.y);

    /* Open a window */
    window = glutCreateWindow("Jeff Molofee's GL Code Tutorial ... NeHe '99");

    /* Register the function to do all our OpenGL drawing. */
    glutDisplayFunc(&DrawGLScene);

    /* Go fullscreen.  This is the soonest we could possibly go fullscreen. */
    glutFullScreen();
    g_full_screen_flag = 1;

    /* Even if there are no events, redraw our gl scene. */
    //glutIdleFunc(&DrawGLScene);

    /* Register the function called when our window is resized. */
    glutReshapeFunc(&ReSizeGLScene);

    /* Register the function called when the keyboard is pressed. */
    glutKeyboardFunc(&keyPressed);

    /* Initialize our window. */
    InitGL(g_win_rect.w,  g_win_rect.h);

    /* Start Event Processing Engine */
    glutMainLoop();

    return 1;
}
