//// HEADER ///////////////////////////////////////////////////////////////
//
//  Johnny SDL Test
//  Recursive Functions, Matrix Stacks, and Dot/Cross Products
//

#include <SDL.h>                      /* Display/Input Library       */
#include <SDL_opengl.h>               /* OpenGL Cross Platform       */


#include <IL/il.h>                        /* DevIL Image Lib w/openGL    */
#include <IL/ilu.h>
#include <IL/ilut.h>


#include <AntTweakBar.h>                  /* Ant GUI for Var changing    */

#include "Shapes.h"                       /* My Shape Library            */

#include <iostream>
#include <cstdio>
#include <cmath>

#include <cairo/cairo.h>
#include <librsvg/rsvg.h>
#include <librsvg/rsvg-cairo.h>


int main(int argc, char *argv[]){
  
//// VARS /////////////////////////////////////////////////////////////////
  
  int width  = 640;                       /* Screen Dimensions           */
  int height = 480;
  int wh     = width/2;                   /* for angle comparison        */
  int hh     = height/2;

  SDL_Event event;                        /* SDL Event Holder            */
    
  GLfloat theta = 0.0f;                   /* Rotation Var                */
  GLfloat rotspeed = .5f;                 /* Rotation Speed              */
  
  struct {                                /* Mouse Coord Var             */
      int x;
      int y;
  } mouse;
  
  GLuint       texture;                   /* holds openGL generated ID   */
  SDL_Surface *sdlsurf;                   /* SDL Load Image              */

  int numleafs = 5;                       /* How many Tree's to display  */
  int degspace = 360/numleafs;            /* Offset between Tree's       */
  
  TwBar *guiBar;                          /* GUI Bar Pointer             */
  
  int     catnodes   = 5;                 /* How many Cats Circling      */
  int     catdepth   = 2;                 /* How many sub trees          */
  int     catsize    = 32;                /* Initial size                */
  GLfloat catszmulti = 0.75;              /* Controls Sizing             */
  int     catspacing = 95;                /* Distance Between Cats       */
  GLfloat catspmulti = 0.3;               /* Control Distance            */
  GLfloat catromulti = 1.0;               /* Rotation Multiplier         */
  
  GLfloat vLength    = 0.0;               /* Vector of the mouse pos     */
  GLfloat vAngle     = 0.0;
  #define vPI          3.14159265
  GLfloat vDeg       = (180 / vPI);
  int xA = 0;
  int yA = 0;
  int xB = 0;
  int yB = 0;
  

  

//// SDL/GL INIT //////////////////////////////////////////////////////////

  SDL_Init        ( SDL_INIT_VIDEO );
  SDL_SetVideoMode( width, height, 0, SDL_OPENGL | SDL_HWSURFACE );

  //SDL_ShowCursor  ( SDL_DISABLE );
  glViewport  ( 0, 0, width, height    ); /* screen <-> windows coords   */
  
  glClearColor( 0.0f, 0.0f, 0.0f, 0.5f ); /* Screen Clear Color          */
  
  glClearDepth( 1.0           );          /* Clears Depth Buffer upto X  */
  //glDepthFunc ( GL_LEQUAL  );           /* How openGL handles Depth    */
  //glEnable    ( GL_DEPTH_TEST );        /* Turn Depth On               */
  
  glShadeModel( GL_SMOOTH     );          /* Shading: FLAT or SMOOTH     */
  
  glEnable    ( GL_BLEND      );          /* Enable Transparency         */
  glBlendFunc ( GL_SRC_ALPHA,             /* Blending Type               */
                GL_ONE_MINUS_SRC_ALPHA );

  //ilInit  ( );                            /* DevIL Init Section          */
  //iluInit ( );
  //ilutRenderer( ILUT_OPENGL );
  //ilutInit( );
  
  TwInit      ( TW_OPENGL, NULL );        /* Ant Tweak Bar               */
  TwWindowSize( width, height   );



//// TWEAK BAR ////////////////////////////////////////////////////////////
  
  guiBar = TwNewBar( "Johnny SDL" );      /* Build the TweakBar Menu     */
  
  TwAddVarRW( guiBar, "Points",  TW_TYPE_INT32, &catnodes, "min=1"          );
  TwAddVarRO( guiBar, "vAngle",  TW_TYPE_FLOAT, &vAngle,   "group='Vector'" );  
  TwAddVarRO( guiBar, "vLength", TW_TYPE_FLOAT, &vLength,  "group='Vector'" );  
  TwAddVarRO( guiBar, "Mouse.X", TW_TYPE_INT32, &mouse.x,  "group='Mouse'"  );
  TwAddVarRO( guiBar, "Mouse.Y", TW_TYPE_INT32, &mouse.y,  "group='Mouse'"  );
  TwAddVarRO( guiBar, "xA",      TW_TYPE_INT32, &xA,       "group='A'"      );
  TwAddVarRO( guiBar, "yA",      TW_TYPE_INT32, &yA,       "group='A'"      );
  TwAddVarRO( guiBar, "xB",      TW_TYPE_INT32, &xB,       "group='B'"      );
  TwAddVarRO( guiBar, "yB",      TW_TYPE_INT32, &yB,       "group='B'"      );    

  

//// TEXTURE INIT /////////////////////////////////////////////////////////

                                          /* Img Library, Load our Image */
                                          /* Calls glGenTextures for you */
  //unsigned int texid = ilutGLLoadImage( "kitty1.png" );

  ILuint texid;
  ILboolean success;
  GLuint image;

  ilInit(); /* Initialization of DevIL */
  ilGenImages(1, &texid); /* Generation of one image name */
  ilBindImage(texid); /* Binding of image name */
  success = ilLoadImage("kitty1.png"); /* Loading of image "image.jpg" */
  if(success) {
	  std::cout << "loaded:" << success << std::endl;
  }
  success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

  if(success) {
	  std::cout << "converted:" << success << std::endl;
  }

  glGenTextures(1, &image); /* Texture name generation */
  glBindTexture(GL_TEXTURE_2D, image); /* Binding of texture name */
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); /* We will use linear interpolation for magnification filter */
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); /* We will use linear interpolation for minifying filter */
  glTexImage2D(
		GL_TEXTURE_2D,
		0,
		ilGetInteger(IL_IMAGE_BPP),
		ilGetInteger(IL_IMAGE_WIDTH),
		ilGetInteger(IL_IMAGE_HEIGHT),
		0,
		ilGetInteger(IL_IMAGE_FORMAT),
		GL_UNSIGNED_BYTE,
		ilGetData()); /* Texture specification */  

                                /* make a reuse func to contain all this */
  
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);  /* ?? */

  glEnable ( GL_TEXTURE_2D );             /* Enable texturing in openGL  */
    
  glBindTexture( GL_TEXTURE_2D, image );  /* Make texid active for below */
                                            
  //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
                                          /* Texture Distance Filters .. */
                                          /* NEAREST/LINEAR antistropic? */
                                          
                
  glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );  

  

//// SVG TEST AREA ////////////////////////////////////////////////////////

RsvgHandle        *rSVG;                  /* Handle to SVG surface       */
RsvgDimensionData  rSize;                 /* Dimension Data of SVG       */
cairo_t           *cCtex;                 /* Cairo Context for rendering */
cairo_surface_t   *cSurf;                 /* Cairo Destination: Image    */
GError            *pError = NULL;         /* Error Holder for GLib       */
int                tSize = 256;           /* Destination Size            */
#define  J_BGRA    0x80E1                 /* ColorSpace of Image         */

g_type_init();                            /* Glib Init (Needed for data) */

                                          /* Load the SVG file           */
rSVG = rsvg_handle_new_from_file( "ballInkscape.svg", &pError );

if(pError != NULL)                        /* Is everything OK ?          */
  std::cout << "ERR: " << pError->message;
  
                                          /* Store the Dimensions of SVG */
rsvg_handle_get_dimensions( rSVG, &rSize );

                                          /* Create Image Surface wAlpha */
cSurf = cairo_image_surface_create( CAIRO_FORMAT_ARGB32, tSize, tSize );

cCtex = cairo_create( cSurf );            /* Bind Context to Surface     */

double bSize;                             /* Uniform Scale to Tex Face   */
if(rSize.width < rSize.height)
  bSize = rSize.height;
else
  bSize = rSize.width;
  
bSize = tSize/bSize;

/* Do We need to Center here?.. and scale based on short side too? */
/* Maybe not the scaling but atleast a center for rectangle shapes */
  
cairo_scale(cCtex, bSize, bSize);         /* Normalize Size to Texture   */

rsvg_handle_render_cairo( rSVG, cCtex );  /* Render into Tex Fullsize    */

cairo_surface_write_to_png(cSurf, "cSurf.png");

GLuint svgglimage;
glGenTextures(1, &svgglimage);
glBindTexture( GL_TEXTURE_2D, svgglimage );  /* Make texid active for below */
glTexImage2D( GL_TEXTURE_2D,              /* Texture Type                */
                          0,              /* LOD lvl can have >1 per tex */
                   GL_RGBA,              /* Internal Color Format (3)   */
										  /* Size W and H                */
	 cairo_image_surface_get_width( cSurf ),
    cairo_image_surface_get_height( cSurf ),
                          0,              /* Image Border                */
	                 GL_BGRA,              /* Color Format of Source Img  */
           GL_UNSIGNED_BYTE,              /* Data Format of Source Img   */
           cairo_image_surface_get_data( cairo_get_target( cCtex ) )
            );                            /* The Image Data itself       */

  

//// (RE)SIZE /////////////////////////////////////////////////////////////
  
  glMatrixMode  (GL_PROJECTION);          /* Determine the Camera        */
  glLoadIdentity( );                      /* Reset the Transform Matrix  */
  
  glOrtho( 0, width, 0, height, -100, 100 );
                                          /* Set a Pixel Perfect Screen  */
                                          /* With 200 Layers for sprites */

  
  glMatrixMode  ( GL_MODELVIEW );         /* Object Mode                 */
  glLoadIdentity( );                      /* Reset Trans.                */

  
  
//// LOOP /////////////////////////////////////////////////////////////////  

  int done;
  for( done = 0; !done; )
  {     
     
      

//// SDL EVENT ////////////////////////////////////////////////////////////  
    
    while( SDL_PollEvent(&event) )
    {
                                          /* Pass if not used by Ant     */
      if( !TwEventSDL(&event, SDL_MAJOR_VERSION, SDL_MINOR_VERSION) )
      {
        switch( event.type )
        {
            case SDL_QUIT:                /* Clicked the X Button        */
                 
               done = true;
               break;                            
                                           
            case SDL_KEYDOWN:             /* Pressed a Key               */
                 
               if(event.key.keysym.sym == SDLK_ESCAPE)
                  done = 1;                              
               break;
            
            case SDL_MOUSEMOTION:         /* Update Mouse XY vars        */
               
               mouse.x = event.motion.x;
               mouse.y = event.motion.y;
               
               break;
               
        } /* switch */
      }  /* if !handled */        
     } /* while */



//// DRAW /////////////////////////////////////////////////////////////////  
           
    glClear( GL_COLOR_BUFFER_BIT          /* What to Clear each Frame    */
           | GL_DEPTH_BUFFER_BIT );

    glLoadIdentity( );                    /* Reset Position              */  

    /* Dont really think of it as 'two points' but.. a vector (the center)
     * and its direction (where the mouse is) 
     */

    xA = wh;                              /* helps make sense of math    */
    yA = hh;
    
    xB = mouse.x;
    yB = height-mouse.y;
    
    glTranslated( xA, yA, 0 );            /* Move to Center of Screen    */
    
    vAngle = atan2( yB-yA, xB-xA );       /* Angle between two points    */
    vAngle = vAngle * vDeg;               /* Convert to Degrees          */
    
                                          /* Calculate Length            */
    vLength = sqrt( pow(xA-xB, 2) + pow(yA-yB, 2) );
      
    glBindTexture( GL_TEXTURE_2D, image );/* Set Texture to Kitty        */
	glBegin( GL_QUADS );
		glColor4ub( 0, 255, 255, 255 );   
		glTexCoord2d(0.0,0.0); glVertex2d(  0.0,  0.0);
		glTexCoord2d(1.0,0.0); glVertex2d(100.0,  0.0);
		glTexCoord2d(1.0,1.0); glVertex2d(100.0,100.0);
		glTexCoord2d(0.0,1.0); glVertex2d(  0.0,100.0);
	glEnd();	

										  /* Set Texture to Ball         */
    glBindTexture( GL_TEXTURE_2D, svgglimage );

    for(int i = 0; i < catnodes; i++)     /* Loop and draw points around */
    {
        glPushMatrix();                   /* Origin to come back to      */
        
                                          /* Rotate to face mouse        */
        glRotatef( vAngle + (i * (360.0/catnodes)), 0.0f, 0.0f, 1.0f );
        
        glTranslated( vLength, 0, 0 );    /* Move out "Length" units     */
        
        jSquare( 64 );
        
        glPopMatrix();
    
    }
    
    TwDraw();                             /* Draw the Ant Tweak Bar      */
    
    SDL_GL_SwapBuffers();                 /* SDL's Screen Swap Func      */

  } /* for */
  
  SDL_Quit();
  return(0);
}
