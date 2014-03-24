   /***********************
   **   Shape Functions   **
    **    Header File    **
     ********************/


#include <SDL.h>
#include <SDL_opengl.h>                   /* OpenGL Cross Platform       */


void jSquare( int );      /* Guarenteed size, +/- 1 offset error for odd */
void jTree( int, int, GLfloat, int, GLfloat, int, GLfloat, GLfloat );  /* make this 'abstract' for diff shapes */
