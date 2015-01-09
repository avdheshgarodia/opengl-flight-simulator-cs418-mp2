#include <iostream>
#include <stdlib.h>
#include <GLUT/glut.h>
#include <math.h>


using namespace std;


//Angles used for Pitch, Yaw, and Roll 
#define ROLL_A  0.06
#define  PITCH_A 0.06
#define  YAW_A 0.06

//Value the speed can be increased or decresed by
#define speedchange .0005


// A sturct the defines a vector with 3 coordinates.
typedef struct{

	double x;
	double y;
	double z;

}vec3;

/*A function that takes in two Vectors a
and b and and returns their cross product*/
vec3 cross(vec3 a, vec3 b)
{
	//the return vector
	vec3 ret;
	ret.x=(a.y*b.z)-(a.z*b.y);
	ret.y= -((a.x*b.z)-(a.z*b.x));
	ret.z=(a.x*b.y)-(a.y*b.x);
    return ret;
}

/*
This function takes in a vec3 to represent a point,
a vec3 used as a vector, and a float for the angle

It the rotates that point by the angle around the vector
The logic was point here:
http://inside.mines.edu/fs_home/gmurray/ArbitraryAxisRotation/ 
*/
vec3 rotate(vec3 point, vec3 axis, float angle)
{
    //the length of the vector squared
    double length = (axis.x*axis.x)+(axis.y*axis.y)+(axis.z*axis.z);
	//the return vector
	vec3 ret;
	
    ret.x = ((axis.x*((axis.x*point.x)+(axis.y*point.y)+(axis.z*point.z)))*(1-cos(angle))
		 + (length*point.x*cos(angle))
	     + (sqrt(length)*(-axis.z*point.y+axis.y*point.z)*sin(angle)))/length;
	
    ret.y = ((axis.y*((axis.x*point.x)+(axis.y*point.y)+(axis.z*point.z)))*(1-cos(angle))
		 + (length*point.y*cos(angle))
	     + (sqrt(length)*(axis.z*point.x-axis.x*point.z)*sin(angle)))/length;
    ret.z = ((axis.z*((axis.x*point.x)+(axis.y*point.y)+(axis.z*point.z)))*(1-cos(angle))
		 + (length*point.z*cos(angle))
	     + (sqrt(length)*(-axis.y*point.x+axis.x*point.y)*sin(angle)))/length;
    

    return ret;
}

/*
The class for the plane which holds the planes postion speed
directiona and normal vector, This class also has methods 
for Picth Roll and Yaw. Also there is a method to keep the plane
moving forward at every tick
*/
class Plane{
public:
	//default method for the plane
	Plane(){}
	
	//initialize method to set the initial position direction normal and speed
	void initialize(){
		//initial position set to 0,0,0
		position.x=0.0;
		position.y=0.0;
		position.z=0.0;
		
		//direction set to forward in x direction
		dir.x=1.0;
		dir.y=0.0;
		dir.z=0.0;
		
		//normal vector set to be up in z direction
		//this makes sure we are facing up if 
		//z was set to -1 we would start facing down
		normal.x=0.0;
		normal.y=0.0;
		normal.z=1.0;
    	
		//setting the speed of the plane
		//this can be changed via keys
	    speed = 0.004;
	
	}
	/*
		Method used for moving forward position
		multiplying speed times direction gives up the
		velecity for the x,y,z components of the plane
		we know that newposition = oldposition + velocity * time 
		since we have no accerelation.
		We assume time is just 1 unit.
	*/
	void forward() 
	{
		
	    position.x += speed * dir.x;
	    position.y += speed * dir.y;
	    position.z += speed * dir.z;
	}
	/*
	Method used for rolling the plane
	Takes in an angle to roll by which can be negative
	
	to roll we dont cange the direction vector only the norma vector,
	normal is rotated about the direction vector
	*/
	void roll(double angle) 
	{
	    normal = rotate(normal, dir, angle);
	}
	/*
	Method used for the yaw of plane
	Takes in an angle to yaw by which can be negative
	
	to yaw we dont cange the normal vector only the direction vector,
	direction is rotated about the normal vector
	*/
	void yaw(double angle)
	{
		dir = rotate(dir, normal, angle);
	}
	/*
	Method used for pitching the plane
	Takes in an angle to pitch by which can be negative
	
	to pitch we need to change both direction and normal vector and we need to
	use the cross product.
	We first rotate direction around direction cross normal
	then we do normal around direction cross normal
	*/	
	void pitch(double angle) 
	{
	    dir = rotate(dir, cross(dir, normal), angle);
	    normal = rotate(normal, cross(dir, normal), angle);
	}
	//this method increases the speed
	void speedUp(){
		speed+=speedchange;
	}
	//this method decreses the speed
	void slowdown(){
		
		//we dont let the speed of the pane hit negative
		if(speed!=0.0){
			speed-=speedchange;
		}
	}
	//position of plane	
	vec3 position;
	//direction of plane
	vec3 dir;
	//normal vector of plane
	vec3 normal;
	//speed of plane
	double speed;


};
//A instance of the plane class
Plane plane;


//Form here this is is code taken form mountain-retained.cpp
float sealevel;

int res = 257;

#define ADDR(i,j,k) (3*((j)*res + (i)) + (k))

GLfloat *verts = 0;
GLfloat *norms = 0;
GLuint *faces = 0;

float frand(float x, float y) {
	//Only thing changed are the seed values to generate a  different terrain
    static int a = 1588659011, b = 19015696;
    int xi = *(int *)&x;
	int yi = *(int *)&y;
    srand(((xi * a) % b) - ((yi * b) % a));
	return 2.0*((float)rand()/(float)RAND_MAX) - 1.0;
}

void mountain(int i, int j, int s)
{
	if (s > 1) {

		float x0,y0,z0,x1,y1,z1,x2,y2,z2,x3,y3,z3,x01,y01,z01,x02,y02,z02,x13,y13,z13,x23,y23,z23,x0123,y0123,z0123;

		x0 = verts[ADDR(i,j,0)];
		y0 = verts[ADDR(i,j,1)];
		z0 = verts[ADDR(i,j,2)];

		x1 = verts[ADDR(i+s,j,0)];
		y1 = verts[ADDR(i+s,j,1)];
		z1 = verts[ADDR(i+s,j,2)];

		x2 = verts[ADDR(i,j+s,0)];
		y2 = verts[ADDR(i,j+s,1)];
		z2 = verts[ADDR(i,j+s,2)];

		x3 = verts[ADDR(i+s,j+s,0)];
		y3 = verts[ADDR(i+s,j+s,1)];
		z3 = verts[ADDR(i+s,j+s,2)];

		x01 = 0.5*(x0 + x1);
		y01 = 0.5*(y0 + y1);
		z01 = 0.5*(z0 + z1);

		x02 = 0.5*(x0 + x2);
		y02 = 0.5*(y0 + y2);
		z02 = 0.5*(z0 + z2);

		x13 = 0.5*(x1 + x3);
		y13 = 0.5*(y1 + y3);
		z13 = 0.5*(z1 + z3);

		x23 = 0.5*(x2 + x3);
		y23 = 0.5*(y2 + y3);
		z23 = 0.5*(z2 + z3);

		x0123 = 0.25*(x0 + x1 + x2 + x3);
		y0123 = 0.25*(y0 + y1 + y2 + y3);
		z0123 = 0.25*(z0 + z1 + z2 + z3);

		z01 += 0.5*((float)s/res)*frand(x01,y01);
		z02 += 0.5*((float)s/res)*frand(x02,y02);
		z13 += 0.5*((float)s/res)*frand(x13,y13);
		z23 += 0.5*((float)s/res)*frand(x23,y23);
		z0123 += 0.5*((float)s/res)*frand(x0123,y0123);

		verts[ADDR(i+s/2,j,0)] = x01;
		verts[ADDR(i+s/2,j,1)] = y01;
		verts[ADDR(i+s/2,j,2)] = z01;

		verts[ADDR(i,j+s/2,0)] = x02;
		verts[ADDR(i,j+s/2,1)] = y02;
		verts[ADDR(i,j+s/2,2)] = z02;

		verts[ADDR(i+s,j+s/2,0)] = x13;
		verts[ADDR(i+s,j+s/2,1)] = y13;
		verts[ADDR(i+s,j+s/2,2)] = z13;

		verts[ADDR(i+s/2,j+s,0)] = x23;
		verts[ADDR(i+s/2,j+s,1)] = y23;
		verts[ADDR(i+s/2,j+s,2)] = z23;

		verts[ADDR(i+s/2,j+s/2,0)] = x0123;
		verts[ADDR(i+s/2,j+s/2,1)] = y0123;
		verts[ADDR(i+s/2,j+s/2,2)] = z0123;

		mountain(i,j,s/2);
		mountain(i+s/2,j,s/2);
		mountain(i,j+s/2,s/2);
		mountain(i+s/2,j+s/2,s/2);

	} else {

		float dx,dy,dz;

		if (i == 0) {
			dx = verts[ADDR(i+1,j,2)] - verts[ADDR(i,j,2)];
		} else if (i == res-1) {
			dx = verts[ADDR(i,j,2)] - verts[ADDR(i-1,j,2)];
		} else {
			dx = (verts[ADDR(i+1,j,2)] - verts[ADDR(i-1,j,2)])/2.0;
		}

		if (j == 0) {
			dy = verts[ADDR(i,j+1,2)] - verts[ADDR(i,j,2)];
		} else if (j == res-1) {
			dy = verts[ADDR(i,j,2)] - verts[ADDR(i,j-1,2)];
		} else {
			dy = (verts[ADDR(i,j+1,2)] - verts[ADDR(i,j-1,2)])/2.0;
		}

		dx *= res;
		dy *= res;
		dz = 1.0/sqrt(dx*dx + dy*dy + 1.0);
		dx *= dz;
		dy *= dz;

		norms[ADDR(i,j,0)] = dx;
		norms[ADDR(i,j,1)] = dy;
		norms[ADDR(i,j,2)] = dz;
	}
}

void makemountain()
{
	int i,j;

	if (verts) free(verts);
	if (norms) free(norms);
	if (faces) free(faces);

	verts = (GLfloat *)malloc(res*res*3*sizeof(GLfloat));
	norms = (GLfloat *)malloc(res*res*3*sizeof(GLfloat));
	faces = (GLuint *)malloc((res-1)*(res-1)*6*sizeof(GLuint));

	verts[ADDR(0,0,0)] = -5.0;
	verts[ADDR(0,0,1)] = -5.0;
	verts[ADDR(0,0,2)] = 0.0;

	verts[ADDR(res-1,0,0)] = 5.0;
	verts[ADDR(res-1,0,1)] = -5.0;
	verts[ADDR(res-1,0,2)] = 0.0;

	verts[ADDR(0,res-1,0)] = -5.0;
	verts[ADDR(0,res-1,1)] = 5.0;
	verts[ADDR(0,res-1,2)] = 0.0;

	verts[ADDR(res-1,res-1,0)] = 5.0;
	verts[ADDR(res-1,res-1,1)] = 5.0;
	verts[ADDR(res-1,res-1,2)] = 0.0;

	mountain(0,0,res-1);

	GLuint *f = faces;
	for (j = 0; j < res-1; j++) {
		for (i = 0; i < res-1; i++) {
			*f++ = j*res + i;
			*f++ = j*res + i + 1;
			*f++ = (j+1)*res + i + 1;
			*f++ = j*res + i;
			*f++ = (j+1)*res + i + 1;
			*f++ = (j+1)*res + i;
		}
	}

}

void init(void)
{
		GLfloat amb[] = {0.2,0.2,0.2};
		GLfloat diff[] = {1.0,1.0,1.0};
		GLfloat spec[] = {1.0,1.0,1.0};

		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);

		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, diff);
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glClearColor (0.5, 0.5, 1.0, 0.0);	// sky
		glEnable(GL_DEPTH_TEST);

		sealevel = 0.0;

		makemountain();
		//added a line to initialize the plane in the init method
		plane.initialize();
}

void display(void)
{

		GLfloat tanamb[] = {0.2,0.15,0.1,1.0};
		GLfloat tandiff[] = {0.4,0.3,0.2,1.0};
		GLfloat tanspec[] = {0.0,0.0,0.0,1.0};	// dirt doesn't glisten

		GLfloat seaamb[] = {0.0,0.0,0.2,1.0};
		GLfloat seadiff[] = {0.0,0.0,0.8,1.0};
		GLfloat seaspec[] = {0.5,0.5,1.0,1.0};	// Single polygon, will only have highlight if light hits a vertex just right

		GLfloat lpos[] = {0.0,0.0,10.0,0.0};	// sun, high noon


		glLoadIdentity ();  
		
		

    
	
		//here the glulooAt is called so the we are seeing what the plane is seeing
		
	    gluLookAt(plane.position.x, plane.position.y, plane.position.z, // first the position of the plane is passed in
		plane.position.x+plane.dir.x, 									//then the postition where the plane will be is passed in 
		plane.position.y+plane.dir.y, 									//this is what the plane is looking at
		plane.position.z+plane.dir.z, 
		plane.normal.x, plane.normal.y, plane.normal.z);				//then the normal vector is passed in which is the 				
		
		
		//Here the forward method is called to update the planes position 
	    plane.forward(); 

		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glColor3f (1.0, 1.0, 1.0);

		static GLfloat angle = 0.0;
		glRotatef(angle, 0.0, 0.0, 1.0);
		

		// send the light position down as if it was a vertex in world coordinates
		glLightfv(GL_LIGHT0, GL_POSITION, lpos);

		// load terrain material
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, tanamb);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, tandiff);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, tanspec);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 50.0);

		// Send terrain mesh through pipeline
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		glVertexPointer(3,GL_FLOAT,0,verts);
		glNormalPointer(GL_FLOAT,0,norms);
		glDrawElements(GL_TRIANGLES, 6*(res-1)*(res-1), GL_UNSIGNED_INT, faces);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);

		// load water material
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, seaamb);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, seadiff);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, seaspec);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 10.0);

		// Send water as a single quad
		glNormal3f(0.0,0.0,1.0);
		glBegin(GL_QUADS);
			glVertex3f(-5,-5,sealevel);
			glVertex3f(5,-5,sealevel);
			glVertex3f(5,5,sealevel);
			glVertex3f(-5,5,sealevel);
		glEnd();

		glutSwapBuffers();
		glFlush ();

		glutPostRedisplay();
	
	
}

void reshape (int w, int h)
{
	glViewport (0, 0, (GLsizei) w, (GLsizei) h); 
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90.0,(float)w/h,0.01,10.0);
	glMatrixMode (GL_MODELVIEW);
}

//THe keyboardfunction to get keyinput
void keys(unsigned char key, int x, int y) // Keyboard callback function
{
    switch (key) {
		//- key: lower sealevel
 		case '-':
 			sealevel -= 0.01;
 			break;
		//+ key: raise sealevel
 		case '+':
		case '=':
 			sealevel += 0.01;
 			break;
		//f key: raise resolution
		//chrashes my computer	
 		case 'f':
 			res = (res-1)*2 + 1;
 			makemountain();
 			break;
		//c key: lower resolution	
 		case 'c':
 			res = (res-1)/2 + 1;
 			makemountain();
 			break;
		//a key: yaw left	
		case 'a':
			//positive angle passed in to yaw left
			plane.yaw(YAW_A);
			break;
		//d key: yaw right	
		case 'd':
			//negative angle passed in to yaw right
			plane.yaw(-YAW_A);
			break;
		//w key:  speedup
		case 'w':
			plane.speedUp();
			break;
		//w key:  slowdown / brake
		case 's':
			plane.slowdown();
			break;	
		//esc key:  Quit program
 		case 27:
 			exit(0);
 			break;
    }
}
//THe special keyboardfunction to get arrow key input
void arrows(int key, int x, int y)
{
    switch (key) {
		//Left key: roll left
        case GLUT_KEY_LEFT: 
			//negative angle rolls left
            plane.roll(-ROLL_A);
            break;
		//right key: roll right
        case GLUT_KEY_RIGHT: 
			//positive angle roll right
            plane.roll(ROLL_A);
            break;
		//down key: pitch down
        case GLUT_KEY_DOWN: 
			//negative angle pitches down
              plane.pitch(-PITCH_A);    
            break;
			
		//inverted controls were not used I find them frustrating	
		//up key: pitch up
        case GLUT_KEY_UP: 
			//positive angle pitches up
            plane.pitch(PITCH_A);        
            break;
    }
}
//main method
int main(int argc, char** argv)
{
	//initialize the window	
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	//window size
    glutInitWindowSize (640, 480);
	//window position
    glutInitWindowPosition (100, 100);
	glutCreateWindow (argv[0]);
	
	// call the init method
    init();
	//set up the display function
    glutDisplayFunc(display);
	//set up the reshape function
	glutReshapeFunc(reshape);
	//set up the keyboard function
    glutKeyboardFunc(keys); 
	//set up the arrowkeys function
    glutSpecialFunc(arrows);
	
	//start main loop
    glutMainLoop();
    return 0;
}