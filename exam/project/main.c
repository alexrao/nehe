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
#include "print.h"
#include "image.h"

/* ASCII code for the escape key. */
#define KEY_ESC         27
#define KEY_SHIFT       10
#define KEY_TAB         9
#define KEY_CAP         20
#define KEY_ALT         18
#define KEY_SPACE       1
#define KEY_BACKSPACE   8
#define KEY_Q           'Q'
#define KEY_q           'q'
#define KEY_f           'f'
#define KEY_F           'F'
#define KEY_DEC          45
#define KEY_ADD           61

// Direction
#define KEY_LEFT            100
#define KEY_UP              101
#define KEY_RIGHT           102
#define KEY_DOWN            103


#define G_BMB_BPP   32
static int g_full_screen_flag = 0;
struct Rect
{
    int x,y,w,h;
};
struct Rect g_win_rect = {100, 0, 1280, 720};
float g_rotated_z = 0, g_rotated_y = 0, g_rotated_x = 0;
float g_color = 1.0f;
float g_scale_count = 1.0f;
unsigned int g_texture[3];
int g_texture_count = 0;

/* The number of our GLUT window */
int window;

GLfloat g_light_ambient[] = {0.5f, 0.5f, 0.5f, 1.0f};
GLfloat g_light_diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat g_light_position[] = {0.0f, 0.0f, 2.0f, 1.0f};
GLfloat g_light_position1[] = {0.0f, 3.0f, 0.0f, 0.0f};
GLfloat filter;

#define STAR_NUM   50

typedef struct
{
    int r, g, b;
    GLfloat     dist;
    GLfloat     angle;
} struct_stars;

struct_stars star[STAR_NUM];

int load_gltextures(void)
{
    char path[256];
    stu_image *image;
    int bpp = 0;


    for(int i=0; i<3; i++)
    {
        image = (stu_image *)malloc(sizeof(stu_image));
        if(image == NULL)
        {
            print("malloc image fail\n");
            return 0;
        }

        snprintf(path, sizeof(path), "data/test%d.bmp", i);
        bpp =image_load(path, image);
        if(bpp == 0)
        {
            print("load image fail");
            free(image);
            return 0;
        }

        // creante texture
        glGenTextures(1, &g_texture[i]);
        glBindTexture(GL_TEXTURE_2D, g_texture[i]);

        // set rejust image
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        //glTexImage2D(GL_TEXTURE_2D, 0, 4, image->x, image->y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->data);
        if(bpp == 32) // 32位RGBA图片处理
        {
            gluBuild2DMipmaps(GL_TEXTURE_2D, 4, image->x, image->y, GL_RGBA, GL_UNSIGNED_BYTE, image->data);
        }
        else // 24位RGB图片处理
        {
            gluBuild2DMipmaps(GL_TEXTURE_2D, 3, image->x, image->y, GL_RGB, GL_UNSIGNED_BYTE, image->data);
        }

        if(image)
        {
            if(image->data)
            {
                free(image->data);
                image->data = NULL;
            }

            free(image);
            image = NULL;
        }
    }

    return 1;
}

/* A general OpenGL initialization function.  Sets all of the initial parameters. */
void InitGL(int Width, int Height)	        // We call this right after our OpenGL window is created.
{
    if(load_gltextures() == 0)
    {
        print("load texture fail\n");
        exit(1);
    }

    glEnable(GL_TEXTURE_2D);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);		// This Will Clear The Background Color To Black
    glClearDepth(1.0);				// Enables Clearing Of The Depth Buffer
    glDepthFunc(GL_LESS);				// The Type Of Depth Test To Do
#if 0
    glEnable(GL_DEPTH_TEST);			// Enables Depth Testing
#else
    //glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);		    // Turn Blending On
#endif
    glShadeModel(GL_SMOOTH);			// Enables Smooth Color Shading

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();				// Reset The Projection Matrix

    gluPerspective(45.0f,(GLfloat)Width/(GLfloat)Height,0.1f,100.0f);	// Calculate The Aspect Ratio Of The Window

    glMatrixMode(GL_MODELVIEW);

    glLightfv(GL_LIGHT0, GL_AMBIENT, g_light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, g_light_diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, g_light_position);
    glEnable(GL_LIGHT0);

//    glLightfv(GL_LIGHT1, GL_AMBIENT, g_light_ambient);
//    glLightfv(GL_LIGHT1, GL_DIFFUSE, g_light_diffuse);
//    glLightfv(GL_LIGHT1, GL_POSITION, g_light_position1);
//    glEnable(GL_LIGHT1);


    /* setup blending */
    glBlendFunc(GL_SRC_ALPHA,GL_ONE);			// Set The Blending Function For Translucency
    glColor4f(1.0f, 1.0f, 1.0f, 0.5);

    for(int i=0; i<STAR_NUM; i++)
    {
        star[i].angle = 0.0f;
        star[i].dist = i*1.0f/STAR_NUM *5.0f;

        star[i].r = rand()%256;
        star[i].g = rand()%256;
        star[i].b = rand()%256;
    }
}



/* The function called when our window is resized (which shouldn't happen, because we're fullscreen) */
void window_resize(int Width, int Height)
{
    if (Height==0)				// Prevent A Divide By Zero If The Window Is Too Small
        Height=1;

    glViewport(0, 0, Width, Height);		// Reset The Current Viewport And Perspective Transformation

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(45.0f,(GLfloat)Width/(GLfloat)Height,0.1f,100.0f);
    glMatrixMode(GL_MODELVIEW);
}
#if 1
void my_display(void)
{
    static GLfloat spin = 0.0f;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindTexture(GL_TEXTURE_2D, g_texture[g_texture_count]);

    for(int i=0; i<STAR_NUM; i++)
    {
        glLoadIdentity();
        glTranslatef(0.0f, 0.0f, -10.0 + 1.0f*g_scale_count);
        glRotatef(g_rotated_x,1.0f, 0.0f,0.0f);
        glRotatef(g_rotated_y,0.0f, 1.0f,0.0f);
        glRotatef(g_rotated_z,0.0f, 0.0f,1.0f);

        glRotatef(90, 1.0f, 0.0f, 0.0f);       // tilt the view.
        glRotatef(star[i].angle, 0.0f, 1.0f, 0.0f);
        //glTranslatef(0.0f, 0.0f, star[i].dist);
        glTranslatef( star[i].dist, 0.0f, 0.0f);

        glRotatef(-star[i].angle, 0.0f, 1.0f, 0.0f);
        //glTranslatef(star[i].angle, 0.0f, 0.0f);
        glRotatef(-90, 1.0f, 0.0f, 0.0f);      // cancel the screen tilt.

        glRotatef(spin, 0.0f, 0.0f, 1.0f);

        glColor4ub(star[i].r, star[i].g, star[i].b, 255);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.3f, -0.3f, 0.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(0.3f, -0.3f, 0.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(0.3f, 0.3f, 0.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.3f, 0.3f, 0.0f);
        glEnd();

        spin += 0.01f;
        star[i].angle += i*1.0f/STAR_NUM * 1.0f;
        star[i].dist -= 0.01f;

        if(star[i].dist < 0.0f)
        {
            star[i].dist += 5.0f;
            star[i].r = rand()/256;
            star[i].g = rand()/256;
            star[i].b = rand()/256;
        }
    }

    glutSwapBuffers();
}

#else
/* The main drawing function. */
void my_display(void)
{
    //static int count = 0;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear The Screen And The Depth Buffer
    //print("count = %d\n", count++);
    glLoadIdentity();				// Reset The View

    glTranslatef(0.0f,0.0f,-6.0+1.0f*g_scale_count);		// Move Left 1.5 Units And Into The Screen 6.0
    glRotatef(g_rotated_x,1.0f, 0.0f,0.0f);
    glRotatef(g_rotated_y,0.0f, 1.0f,0.0f);
    glRotatef(g_rotated_z,0.0f, 0.0f,1.0f);
    //glScaled(1.0f*g_scale_count,1.0f*g_scale_count,1.0f*g_scale_count);

    glBindTexture(GL_TEXTURE_2D, g_texture[g_texture_count]);

    // draw a triangle
    //glBegin(GL_POLYGON);				// start drawing a polygon
    glBegin(GL_QUADS);				// start drawing a polygon

    glColor4f(g_color, g_color, g_color,  g_color);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);	// Bottom Left Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);	// Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);	// Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);	// Top Left Of The Texture and Quad

    // Back Face
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);	// Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);	// Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);	// Top Left Of The Texture and Quad
    glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);	// Bottom Left Of The Texture and Quad
    glEnd();                                    // done with the polygon.

    glBindTexture(GL_TEXTURE_2D, g_texture[1]);
    glBegin(GL_QUADS);				// start drawing a polygon
    // Top Face
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);	// Top Left Of The Texture and Quad
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  1.0f,  1.0f);	// Bottom Left Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,  1.0f,  1.0f);	// Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);	// Top Right Of The Texture and Quad

    // Bottom Face
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);	// Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, -1.0f, -1.0f);	// Top Left Of The Texture and Quad
    glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);	// Bottom Left Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);	// Bottom Right Of The Texture and Quad
    glEnd();                                    // done with the polygon.

    glBindTexture(GL_TEXTURE_2D, g_texture[2]);
    glBegin(GL_QUADS);				// start drawing a polygon
    // Right face
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);	// Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);	// Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);	// Top Left Of The Texture and Quad
    glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);	// Bottom Left Of The Texture and Quad

    // Left Face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);	// Bottom Left Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);	// Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);	// Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);	// Top Left Of The Texture and Quad

    glEnd();                                    // done with the polygon.

/*
    glLoadIdentity();				// Reset The View
    glTranslatef(1.5f,0.0f,-6.0+1.0f*g_scale_count);		// Move Left 1.5 Units And Into The Screen 6.0
    glRotatef(g_rotated_x,0.0f, 1.0f,0.0f);

    // draw a square (quadrilateral)
    glBegin(GL_QUADS);				// start drawing a polygon (4 sided)
    glVertex3f(-1.0f, 1.0f, 0.0f);		// Top Left
    glVertex3f( 1.0f, 1.0f, 0.0f);		// Top Right
    glVertex3f( 1.0f,-1.0f, 0.0f);		// Bottom Right
    glVertex3f(-1.0f,-1.0f, 0.0f);		// Bottom Left
    glEnd();					// done with the polygon
    */

    // swap buffers to display, since we're double buffered.
    glutSwapBuffers();
}
#endif

void mouse_fun(int button, int state, int x, int y)
{
    print("button = %d, state = %d, x=%d, y = %d", button, state, x, y);
    if(button == 4) // 滚轮向后
    {
        g_scale_count -= 0.1f;
    }
    else if(button == 3) // 滚轮向前
    {
        g_scale_count += 0.1f;
    }

    my_display();
}

/* The function called whenever a key is pressed. */
void key_pressed(unsigned char key, int x, int y)
{
    /* avoid thrashing this procedure */
    //usleep(100);

    print("key = %d, x = %d, y = %d", key, x, y);

    /* If escape is pressed, kill everything. */
    if (key == KEY_ESC)
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
    else if(key == 'c')
    {
        g_color += 0.1f;
        if(g_color > 1.0f)
            g_color = 0.0f;
    }
    else if(key == 'z')
    {
        g_color -= 1.0f;
        if(g_color < 0.0f)
            g_color = 1.0f;
    }
    else if(key == 'l'
            || key == 'L')
    {
        static int light = 0;
        if(light)
        {
            light = 0;
            glEnable(GL_LIGHTING);
            print("enable light");
        }
        else
        {
            light = 1;
            glDisable(GL_LIGHTING);
            print("disable light");
        }
    }
    else if(key == 'n'
            || key == 'N')
    {
        g_texture_count += 1;
        if(g_texture_count == 3)
            g_texture_count = 0;
    }
    else if(key == 'b'
            || key == 'B')
    {
        static int blent = 0;
        if(blent)
        {
            print("disable blend");
            blent = 0;
            glDisable(GL_BLEND);
            glEnable(GL_DEPTH_TEST);
        }
        else
        {
            blent = 1;
            print("enable blend");
            glEnable(GL_BLEND);
            glDisable(GL_DEPTH_TEST);
        }
    }
    my_display();
}

void specialkey_press(int key, int x, int y)
{
    print("key = %d, x=%d, y=%d", key, x, y);
    switch(key)
    {
        case GLUT_KEY_PAGE_UP:
            g_rotated_z += 2.0f;
            break;
        case GLUT_KEY_PAGE_DOWN:
            g_rotated_z -= 2.0f;
            break;
        case GLUT_KEY_UP:
            g_rotated_x -= 2.0f;
            break;
        case GLUT_KEY_DOWN:
            g_rotated_x += 2.0f;
            break;
        case GLUT_KEY_LEFT:
            g_rotated_y -= 2.0f;
            break;
        case GLUT_KEY_RIGHT:
            g_rotated_y += 2.0f;
            break;
    }

    my_display();
}

int main(int argc, char **argv)
{
    /* Initialize GLUT state - glut will take any command line arguments that pertain to it or
       X Windows - look at its documentation at http://reality.sgi.com/mjk/spec3/spec3.html */
    glutInit(&argc, argv);

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
    window = glutCreateWindow("RXP OpenGL练习测试");

    /* Register the function to do all our OpenGL drawing. */
    glutDisplayFunc(&my_display);

    /* Go fullscreen.  This is the soonest we could possibly go fullscreen. */
    glutFullScreen();
    g_full_screen_flag = 1;

    /* Even if there are no events, redraw our gl scene. */
    glutIdleFunc(&my_display);

    /* Register the function called when our window is resized. */
    glutReshapeFunc(&window_resize);

    /* Register the function called when the keyboard is pressed. */
    glutKeyboardFunc(&key_pressed);

    glutSpecialFunc(&specialkey_press);

    /* Initialize our window. */
    InitGL(g_win_rect.w,  g_win_rect.h);

    glutMouseFunc(mouse_fun);

    /* Start Event Processing Engine */
    glutMainLoop();

    return 1;
}
