/* File: simple_encode.c		-*- C -*- 			     */
/* Created by: Alex Knowles (alex@ed.ac.uk) Sat Dec  2 19:49:23 1995	     */
/* Last Modified: Time-stamp: <26 Feb 96 1613 Alex Knowles> 		     */
/* RCS $Id: simple_encode.c,v 1.6 1996/02/28 17:58:57 ark Exp $ */
static char rcsid[]=
"$Id: simple_encode.c,v 1.6 1996/02/28 17:58:57 ark Exp $";
char *simple_encode_version( void )
{ return rcsid; }
/* (C)Copyright 1995, Alex Knowles                                      */
/*                                                                      */
/* This program, it's source and it's documentation is produced by      */
/* Alex Knowles.                                                        */
/*                                                                      */
/* This program has been included for  its instructional value. It has  */
/* been  tested with  care but  is not guaranteed   for any particular  */
/* purpose. Neither the authors, nor anyone  associated with him offer  */
/* any   warranties  or  representations,  nor  do   they accept   any  */
/* liabilities with respect to this program.                            */
/*                                                                      */
/* This program must not be used for commmercial gain without the       */
/* written permission of the author(s).                                 */
/*                                                                      */
/* For more information please contact: Alex Knowles (alex@ed.ac.uk)    */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mpege.h"

/* this is a small app to try and show hgow to use MPEGelib 		*/

/* it is not intended as a lesson in reading ppm files (thank goodnes) 	*/

/*  more information can be found at	 				*/
/*  http://www.tardis.ed.ac.uk/~ark/mpegelib	 			*/

/* this is the maximum length of line we expect in out ppm files 	*/
#define LINE_LENGTH 1024

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

/* this is needed by my automatic prototype emacs code */
/* START: Function Prototypes from simple_encode.c */
int main( int argc, char *argv[] );
ImVfb *read_ppm( char *prefix, int i );
void usage( void );
void get_line( char *line, FILE *in );

/* END: Function Prototypes from simple_encode.c */

/* o.k. put on your thinking caps and dig into the code here! */
int main( int argc, char *argv[] )
{
  int start,end,i;
  char *prefix;
  FILE *output;
  ImVfb *image=NULL;
  MPEGe_options options;
  
  /* check they called us right */
  if( argc != 5 ){
    usage();
    return 0;
  }
  
  fprintf(stderr,"Starting....\n");
  start=atoi(argv[1]);
  end  =atoi(argv[2]);
  prefix=argv[3];
  
  /* let's get a file pointer pointing to the output file */
  output=fopen(argv[4], "w" );
  if( !output ){
    fprintf(stderr,"Failed to open file \"%s\" for writing\n",argv[5]);
    return 0;
  }
  
  MPEGe_default_options( &options );
  
  /* start the library with the default options */
  if( !MPEGe_open(output, &options ) ){
    fprintf(stderr,"MPEGe library initialisation failure!:%s\n",options.error);
    return 0;
  }
  
  /* now loop through the frames, reading them in then encoding them */
  for( i=start; i<=end ; i++ ){
    fprintf(stderr,"Processing Frame %d\r",i); fflush(stderr);
    
    image=read_ppm( prefix, i );
    if( !MPEGe_image(image, &options) ){
      fprintf(stderr,"Oh dear MPEGe_image failure: %s\n",options.error);
    }
  }
  fprintf(stderr,"\nclosing MPEG\n");
  
  /* add the MPEG sequence ender and then quit */
  if( !MPEGe_close(&options) ){
    fprintf(stderr,"Had a bit of difficulty closing the file: %s\n",
	    options.error);
  }
  
  fprintf(stderr,"tah dah\n");
  
  return 0;
}

/* this reads the image into a ImVfb and returns a pointer to it */
ImVfb *read_ppm( char *prefix, int i )
{
  static ImVfb *image=NULL; /* we only wnat to alloc memory for these once */
  static char *filename=NULL;
  int width, height,x,y,r,g,b;
  char line[LINE_LENGTH];
  FILE *in=NULL;
  ImVfbPtr ptr;
  enum { RAW, ASCII } filetype;
  
  if( !filename ){
    filename=(char *)malloc((strlen(prefix) + 10) *sizeof( char ));
  }
  
  sprintf(filename,"%s%d",prefix, i );
  
  in=fopen( filename, "r" );
  if( !in ){
    fprintf(stderr,"Couldn't open file %s\n",filename);
    return NULL;
  }    
  
  get_line(line,in );
  if( !strncmp("P3",line,2 ) ){
    filetype=ASCII;
  } else if( !strncmp("P6",line,2 ) ){
    filetype=RAW;
  } else {
    fprintf(stderr,"file %s not PPM P3/P6 file :-(\n",filename);
    fprintf(stderr,"I only understand these file types, sorry\n");
    return NULL;
  }
  
  get_line( line, in );
  if( 2 != sscanf(line,"%d %d",&width,&height ) ){
    fprintf(stderr,"Couldn't read dimensions\n");
    return NULL;
  }
  
  /* skip line with 255 on it */
  get_line( line, in );
  
  /* set up the ImVfb used to store the image */
  if( !image ){
    image=MPEGe_ImVfbAlloc( width,height, IMVFBRGB, TRUE );
    if( !image ){
      fprintf(stderr,"Coudln't allocate memory for frame buffer\n");
      exit(2);
    }
  }
  
  /* get to the first pixel in the image */
  ptr=ImVfbQPtr( image,0,0 );
  
  switch( filetype ){
  case RAW:
    if( fread( ptr, 3, width*height, in ) != width*height ){
      fprintf(stderr,"error reading file %s not enough stuff read in!\n",
	      filename);
      exit(2);
    }
    break;
  case ASCII:
    for( y=0 ; y<height ; y++ )
      for( x=0 ; x<width ; x++ ){
	fscanf(in,"%d %d %d",&r,&g,&b);
	ImVfbSRed(   image, ptr, r );
	ImVfbSGreen( image, ptr, g );
	ImVfbSBlue(  image, ptr, b );
	ptr=ImVfbQNext( image, ptr );
      }
    break;
  default:
    fprintf(stderr,"strange file type!\n");
    exit(2);
  }
  
  return image;
}

void usage( void )
{
  /* horrible placing of \n just to get my lines < 80 chars! sorry folks */
  fprintf(stderr,"Usage:\n");
  fprintf(stderr,"simple_encode <start> <end> <file_prefix> <output_file>\n");
  fprintf(stderr,"\ntry ./simple_encode 0 15 data/pic test.mpg\n\n");
  fprintf(stderr,"See http://www.tardis.ed.ac.uk/~ark/mpegelib/example.html");
  fprintf(stderr,"\nfor more info\n");
  fprintf(stderr,"If you don't have the example images then get them from\n");
  fprintf(stderr,"ftp://ftp.tardis.ed.ac.uk/users/ark/mpegelib/\n");
}
  
/* reads in a line and ignores lines which start with a hash */
void get_line( char *line, FILE *in )
{
  do{
    fgets(line, LINE_LENGTH, in );
  } while( line[0]=='#' );
}
  
