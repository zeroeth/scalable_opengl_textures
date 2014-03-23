rm svgGLtexture
CFLAGS=`pkg-config sdl cairo librsvg-2.0 gdk-pixbuf-2.0 glib-2.0 ILU ILUT --cflags --libs`
g++ main.c Shapes.c -o svgGLtexture $CFLAGS -framework opengl -lanttweakbar
