#include <SDL/SDL.h>
#include <SDL/SDL_Image.h>
#include <gl/gl.h>

#include "Shapes.h"


int main(int argc, char *argv[]){
  
//// VARS /////////////////////////////////////////////////////////////////
  
  int width  = 640;                       /* Screen Dimensions           */
  int height = 480;

  SDL_Event event;                        /* SDL Event Holder            */
    
  GLfloat theta = 0.0f;                   /* Rotation Var                */
  struct {                                /* Mouse Coord Var             */
      int x;
      int y;
  } mouse;
  
  GLuint texture;                      /* holds openGL generated ID   */
  SDL_Surface *sdlsurf;                /* SDL Load Image              */



//// SDL/GL INIT //////////////////////////////////////////////////////////

  SDL_Init        ( SDL_INIT_VIDEO );
  SDL_SetVideoMode( width, height, 0, SDL_OPENGL | SDL_HWSURFACE );
  SDL_ShowCursor  ( SDL_DISABLE );

  glViewport  ( 0, 0, width, height    ); /* screen <-> windows coords   */
  
  glClearColor( 1.0f, 1.0f, 1.0f, 0.0f ); /* Screen Clear Color          */
  
  glClearDepth( 1.0           );          /* Clears Depth Buffer upto X  */
  glDepthFunc ( GL_LESS       );          /* How openGL handles Depth    */
  glEnable    ( GL_DEPTH_TEST );          /* Turn Depth On               */
  
  glShadeModel( GL_SMOOTH     );          /* Shading: FLAT or SMOOTH     */
  
  glEnable    ( GL_BLEND      );          /* Enable Transparency         */
  glBlendFunc ( GL_SRC_ALPHA,             /* Blending Type               */
                GL_ONE_MINUS_SRC_ALPHA );



//// TEXTURE INIT /////////////////////////////////////////////////////////

  sdlsurf = IMG_Load( "kitty1.png" );  /* Img Library, Load our Image */
  
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  
  glGenTextures( 1, &texture );           /* Normally create an array .. */
                                          /* of IDs but we just want one */
  glBindTexture( GL_TEXTURE_2D, texture );
                                          /* ^ Bind texture type         */
                                          
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
                                          /* Texture Distance Filters .. */
                                          /* NEAREST/LINEAR antistropic? */
                                          
  glTexImage2D( GL_TEXTURE_2D,            /* Texture Type                */
                0,                        /* LOD lvl can have >1 per tex */
                GL_RGBA,                   /* Internal Color Format (3)   */
                sdlsurf->w,               /* Size W and H                */
                sdlsurf->h,
                0,                        /* Image Border                */
                GL_RGBA,                   /* Color Format of Source Img  */
                GL_UNSIGNED_BYTE,         /* Data Format of Source Img   */
                sdlsurf->pixels );        /* The Image Data itself       */
                
  //glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);  
  glEnable ( GL_TEXTURE_2D );
                
  SDL_FreeSurface( sdlsurf );             /* Clean up ImageLib Memory    */                
  

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
        switch( event.type )
        {
            case SDL_QUIT:
                 
               done = true;
               break;                            
                                           
            case SDL_KEYDOWN:
                 
               if(event.key.keysym.sym == SDLK_ESCAPE)
                  done = 1;                              
               break;
            
            case SDL_MOUSEMOTION:
               
               mouse.x = event.motion.x;
               mouse.y = event.motion.y;
               
               break;
               
        } /* switch */
     } /* while */



//// DRAW /////////////////////////////////////////////////////////////////  
           
    glClear( GL_COLOR_BUFFER_BIT          /* What to Clear each Frame    */
           | GL_DEPTH_BUFFER_BIT );

    glLoadIdentity( );                     /* Reset Position              */
    
    glTranslated( mouse.x, (height - mouse.y), 0 );
    glRotatef   ( theta, 0.0f, 0.0f, 1.0f        );
    
    
         glBindTexture( GL_TEXTURE_2D, texture );
         jSquare      ( 128 );

         glRotatef   ( theta, 0.0f, 0.0f, 1.0f );         
         glTranslated( 20, 20, 0 );
         jSquare     ( 64 );


    theta += .5;
    
    SDL_GL_SwapBuffers();                 /* SDL's Screen Swap Func      */

  } /* for */
  
  SDL_Quit();
  return(0);
}
