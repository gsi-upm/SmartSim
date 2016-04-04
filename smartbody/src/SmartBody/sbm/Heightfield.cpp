#include "Heightfield.h"
#include <sb/SBTypes.h>
#include <cstdio>
#include <cstdlib>
#include <math.h>
#include "sbm/lin_win.h"
#include "sr/sr_gl.h"

#define BITMAP_ID 0x4D42

Heightfield::Heightfield( void )	{
	defaults();
}

Heightfield::~Heightfield() {
	clear();
}

void Heightfield::defaults( void ) {

#ifdef WIN32
	_header = NULL;
#endif
	_imageData = NULL;
	
	image_width = 0;
	image_height = 0;
	num_color_comps = 0;
	
	mesh_resx = 0;
	mesh_resz = 0;

	mesh_scale[ 0 ] = 100.0f;
	mesh_scale[ 1 ] = 100.0f;
	mesh_scale[ 2 ] = 100.0f;

	mesh_origin[ 0 ] = 0.0f;
	mesh_origin[ 1 ] = 0.0f;
	mesh_origin[ 2 ] = 0.0f;
	
	vertex_arr = NULL;
	normal_arr = NULL;
	color_arr = NULL;
	dirty_normals = true;
}

void Heightfield::clear( void ) {

#ifdef WIN32
	if (_header)	{
		delete _header;
	}
#endif
	if (_imageData) {
		delete [] _imageData;
	}
	if( vertex_arr )	{
		delete [] vertex_arr;
	}
	if( normal_arr )	{
		delete [] normal_arr;
	}
	if( color_arr )	{
		delete [] color_arr;
	}
}

void Heightfield::load( char* filename )	{

	clear();
	defaults();
	
#if 0
	_header = new BITMAPINFOHEADER();
	_imageData = LoadBitmapFile( filename, _header );
#else
	_imageData = read_ppm( filename );
#endif
	if( _imageData )	{
		initializeTerrain( _imageData );
	}
}

#if 0
void Heightfield::paste_img( void )	{

	int alignment;
	glGetIntegerv( GL_UNPACK_ALIGNMENT, &alignment );
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

	int w = mesh_resx + 1;
	int h = mesh_resz;

	GLfloat project_mat[ 16 ];
	glGetFloatv( GL_PROJECTION_MATRIX, project_mat );
	glPushAttrib( GL_VIEWPORT_BIT|GL_SCISSOR_BIT );
		glPushMatrix();
			glViewport( 0, 0, w, h ); 
			glScissor( 0, 0, w, h ); 
			glMatrixMode( GL_PROJECTION );
			glLoadIdentity();
//			gluOrtho2D( 0.0, (GLdouble)w, 0.0, (GLdouble)h );
			glOrtho( 0.0, (GLdouble)w, 0.0, (GLdouble)h, 0.0, 0.0 );
			glMatrixMode( GL_MODELVIEW );
			glLoadIdentity();

			glRasterPos2i( 0, 0 );
			glDrawPixels( 
				w, h, 
				GL_RGB, GL_UNSIGNED_BYTE, 
				(GLvoid*)_imageData
			);

		glPopMatrix();
	glPopAttrib();
	glMatrixMode( GL_PROJECTION );
	glLoadMatrixf( project_mat );
	glMatrixMode( GL_MODELVIEW );

	glPixelStorei( GL_UNPACK_ALIGNMENT, alignment );
}
#endif

void Heightfield::render( int renderMode )	{
#if defined (__ANDROID__) || defined (SB_IPHONE) || defined(__native_client__)
#else
	if( vertex_arr && color_arr )	{

		if( dirty_normals )	{
			load_normals();
		}
	
		glPushAttrib(GL_POLYGON_BIT);
		if (renderMode == 0)
			glPolygonMode(GL_FRONT, GL_FILL);
		else
			glPolygonMode(GL_FRONT, GL_LINE);

		glPushMatrix();
		glTranslatef( mesh_origin[ 0 ], mesh_origin[ 1 ], mesh_origin[ 2 ] );
		glScalef( mesh_scale[ 0 ], mesh_scale[ 1 ], mesh_scale[ 2 ] );

#if 0
		for( int j = 0; j < mesh_resz - 1; j++ )	{

			glBegin( GL_TRIANGLE_STRIP );
			for( int i = 0; i < mesh_resx - 1; i++ )	{

				int index = ( j * mesh_resx + i ) * 3;
				glColor3ubv( color_arr + index );
				glVertex3fv( vertex_arr + index );

				index = ( ( j + 1 ) * mesh_resx + i ) * 3;
				glColor3ubv( color_arr + index );
				glVertex3fv( vertex_arr + index );

				index = ( j * mesh_resx + i + 1 ) * 3;
				glColor3ubv( color_arr + index );
				glVertex3fv( vertex_arr + index );

				index = ( ( j + 1 ) * mesh_resx + i + 1 ) * 3;
				glColor3ubv( color_arr + index );
				glVertex3fv( vertex_arr + index );
			}
			glEnd();
		}

#elif 1
		int norm_index = 0;
		glBegin( GL_TRIANGLES );
		for( int j = 0; j < mesh_resz - 1; j++ )	{
		
			for( int i = 0; i < mesh_resx - 1; i++ )	{
			
				glNormal3fv( normal_arr + norm_index );
				int index = ( j * mesh_resx + i ) * 3;
				glColor3ubv( color_arr + index );
				glVertex3fv( vertex_arr + index );

				index = ( ( j + 1 ) * mesh_resx + i ) * 3;
				glColor3ubv( color_arr + index );
				glVertex3fv( vertex_arr + index );

				index = ( j * mesh_resx + i + 1 ) * 3;
				glColor3ubv( color_arr + index );
				glVertex3fv( vertex_arr + index );

				norm_index += 3;

				glNormal3fv( normal_arr + norm_index );
				index = ( j * mesh_resx + i + 1 ) * 3;
				glColor3ubv( color_arr + index );
				glVertex3fv( vertex_arr + index );

				index = ( ( j + 1 ) * mesh_resx + i ) * 3;
				glColor3ubv( color_arr + index );
				glVertex3fv( vertex_arr + index );

				index = ( ( j + 1 ) * mesh_resx + i + 1 ) * 3;
				glColor3ubv( color_arr + index );
				glVertex3fv( vertex_arr + index );

				norm_index += 3;
			}
		}
		glEnd();
#else
		int norm_index = 0;
		glBegin( GL_TRIANGLES );
	glColor3ub( 127, 127, 127 );
//	glColor3ub( 255, 255, 255 );
		for( int j = 0; j < mesh_resz - 1; j++ )	{
		
			for( int i = 0; i < mesh_resx - 1; i++ )	{
			
				glNormal3fv( normal_arr + norm_index );
				int index = ( j * mesh_resx + i ) * 3;
				glVertex3fv( vertex_arr + index );

				index = ( ( j + 1 ) * mesh_resx + i ) * 3;
				glVertex3fv( vertex_arr + index );

				index = ( j * mesh_resx + i + 1 ) * 3;
				glVertex3fv( vertex_arr + index );

				norm_index += 3;

				glNormal3fv( normal_arr + norm_index );
				index = ( j * mesh_resx + i + 1 ) * 3;
				glVertex3fv( vertex_arr + index );

				index = ( ( j + 1 ) * mesh_resx + i ) * 3;
				glVertex3fv( vertex_arr + index );

				index = ( ( j + 1 ) * mesh_resx + i + 1 ) * 3;
				glVertex3fv( vertex_arr + index );

				norm_index += 3;
			}
		}
		glEnd();
#endif

		glPopMatrix();
		glPopAttrib();

	}
#endif
}

void Heightfield::normalize_arr3( float V[ 3 ] ) {

	float len = sqrt( V[ 0 ] * V[ 0 ] + V[ 1 ] * V[ 1 ] + V[ 2 ] * V[ 2 ] );
	V[ 0 ] /= len;
	V[ 1 ] /= len;
	V[ 2 ] /= len;
}

void Heightfield::calc_normal( float *N_out, float *A, float *B, float *C, float *S )	{

	float v0[ 3 ];
	v0[ 0 ] = ( B[ 0 ] - A[ 0 ] ) * S[ 0 ];
	v0[ 1 ] = ( B[ 1 ] - A[ 1 ] ) * S[ 1 ];
	v0[ 2 ] = ( B[ 2 ] - A[ 2 ] ) * S[ 2 ];
	normalize_arr3( v0 );
	
	float v1[ 3 ];
	v1[ 0 ] = ( C[ 0 ] - B[ 0 ] ) * S[ 0 ];
	v1[ 1 ] = ( C[ 1 ] - B[ 1 ] ) * S[ 1 ];
	v1[ 2 ] = ( C[ 2 ] - B[ 2 ] ) * S[ 2 ];
	normalize_arr3( v1 );
	
	N_out[ 0 ] = v0[ 1 ] * v1[ 2 ] - v0[ 2 ] * v1[ 1 ];
	N_out[ 1 ] = v0[ 2 ] * v1[ 0 ] - v0[ 0 ] * v1[ 2 ];
	N_out[ 2 ] = v0[ 0 ] * v1[ 1 ] - v0[ 1 ] * v1[ 0 ];
	normalize_arr3( N_out );
}

void Heightfield::initializeTerrain( unsigned char* terrain )	{

#if 0
	int hist[ 8 ] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	int n = image_width * image_height * num_color_comps;
	for( int i=0; i< n; i++ )	{
		hist[ (int)( terrain[ i ] ) / 32 ]++;
	}
	printf( "HIST: %d %d %d %d %d %d %d %d\n", hist[0], hist[1], hist[2], hist[3], hist[4], hist[5], hist[6], hist[7] );
#endif

	vertex_arr = new float[ image_width * image_height * 3 ];
	color_arr = new unsigned char[ image_width * image_height * 3 ];

	for( int j = 0; j < image_height; j++ )	{
		for( int i = 0; i < image_width; i++ )	{
			
			int img_index = ( j * image_width + i ) * num_color_comps;
			int arr_index = ( j * image_width + i ) * 3;
			
			vertex_arr[ arr_index + 0 ] = (float) i / (float)( image_width - 1 );
			vertex_arr[ arr_index + 1 ] = (float) _imageData[ img_index ] / 255.0f;
			vertex_arr[ arr_index + 2 ] = (float) j / (float)( image_height - 1 );
			
			char grey = 63 + _imageData[ img_index ] / 2;
			color_arr[ arr_index + 0 ] = grey;
			color_arr[ arr_index + 1 ] = grey;
			color_arr[ arr_index + 2 ] = grey;
		}
	}

	mesh_resx = image_width;
	mesh_resz = image_height;
}

void Heightfield::load_normals( void )	{

	if( normal_arr )	{
		delete [] normal_arr;
	}
	normal_arr = new float[ ( image_width - 1 ) * ( image_height - 1 ) * 2 * 3 ];
	int norm_index = 0;

	for( int j = 0; j < mesh_resz - 1; j++ )	{
		for( int i = 0; i < mesh_resx - 1; i++ )	{
			
			int A_index = ( j * mesh_resx + i ) * 3;
			int B_index = ( ( j + 1 ) * mesh_resx + i ) * 3;
			int C_index = ( j * mesh_resx + i + 1 ) * 3;

			calc_normal( 
				normal_arr + norm_index,
				vertex_arr + A_index,
				vertex_arr + B_index,
				vertex_arr + C_index,
				mesh_scale
			);
			norm_index += 3;

			A_index = ( j * mesh_resx + i + 1 ) * 3;
			B_index = ( ( j + 1 ) * mesh_resx + i ) * 3;
			C_index = ( ( j + 1 ) * mesh_resx + i + 1 ) * 3;
			
			calc_normal( 
				normal_arr + norm_index,
				vertex_arr + A_index,
				vertex_arr + B_index,
				vertex_arr + C_index,
				mesh_scale
			);
			norm_index += 3;
		}
	}
	dirty_normals = false;
#if 0
	for( int i = 0; i <= 10; i++ )	{
//		float px = 0.0f;
//		float px = 1.0f;
		float px = (float)i/10.0f;
//		float px = 1.0f - (float)i/10.0f;
//		float pz = 0.0f;
//		float pz = 1.0f;
		float pz = (float)i/10.0f;
//		float pz = 1.0f - (float)i/10.0f;
		float n[ 3 ];
		float y = get_elevation( px, pz, n );
		printf( "%f %f : %f : %f %f %f\n", px, pz, y, n[0], n[1], n[2] );
	}
#endif
}

void Heightfield::set_scale( float x, float y, float z )	{

	mesh_scale[ 0 ] = x;
	mesh_scale[ 1 ] = y;
	mesh_scale[ 2 ] = z;
	dirty_normals = true;
}

void Heightfield::set_origin( float x, float y, float z )	{

	mesh_origin[ 0 ] = x;
	mesh_origin[ 1 ] = y;
	mesh_origin[ 2 ] = z;
	dirty_normals = true;
}

void Heightfield::set_auto_origin( void )	{

	mesh_origin[ 0 ] = -mesh_scale[ 0 ] * 0.5f;
	mesh_origin[ 1 ] = 0.0f;
	mesh_origin[ 2 ] = -mesh_scale[ 2 ] * 0.5f;

	float center_elev = get_elevation( 0.0f, 0.0f );
	mesh_origin[ 1 ] = -center_elev;
	dirty_normals = true;
}

unsigned char* Heightfield::parse_ppm( FILE *stream )	{
	
	char ppm_key[256];
	if( fscanf( stream, "%s", ppm_key ) != 1 )	{
		printf( "Heightfield::parse_ppm ERR: could not scan ppm_key\n" );
		return( NULL );
	}

	int binary;
	if( strcmp( ppm_key, "P3" ) == 0 )
		binary = 0;
	else
	if( strcmp( ppm_key, "P6" ) == 0 )
		binary = 1;
	else	{
		printf( "Heightfield::parse_ppm ERR: key '%s' not recognized\n", ppm_key );
		return( NULL );
	}

	char tmp_buffer[1024];
	int found_size = 0;
	while( !found_size )	{
		if( fscanf( stream, "%d %d", &image_width, &image_height ) == 2 )	{
			if( image_width * image_height > 0 )	{
				found_size = 1;
			}
			else	{
				printf( "Heightfield::parse_ppm ERR: could not read size\n" );
				return( NULL );
			}
		}
		else
			fscanf( stream, "%[^\n]", tmp_buffer );
	}

	num_color_comps = 3;
	long image_area = image_width * image_height;
	printf( " image: width %d height %d comps %d\n", image_width, image_height, num_color_comps );
	long image_bytes = image_area * num_color_comps;

	float max = 0.0;
	int found_max = 0;
	while( !found_max )	{
		if( fscanf( stream, "%f", &max ) == 1 )	{
			if( max > 0.0 )	{
				if( binary && ( max != 255.0f ) )	{
					printf( "Heightfield::parse_ppm ERR: maxval '%f' not supported\n", max );
					return( NULL );
				}
				found_max = 1;
			}
			else	{
				printf( "Heightfield::parse_ppm ERR: could not read maxval\n" );
				return( NULL );
			}
		}
		else
			fscanf( stream, "%[^\n]", tmp_buffer );
	}

	unsigned char* buff = new unsigned char [ image_bytes ];
	if( buff == NULL )	{
		printf( "Heightfield::Heightfield ERR: new [%d] bytes FAILED\n", (int)image_bytes );
		return( NULL );
	}

	int i, j, k;
	long n = 0;
	if( buff == NULL )	{
		printf( "Heightfield::write_ppm ERR: no pixels\n" );
		return( NULL );
	}
	if( binary )	{
		unsigned char new_line;
		fread( &new_line, sizeof( unsigned char ), 1, stream );
		if( new_line != '\n' )	{
			printf( "Heightfield::parse_ppm ERR: read byte: %d: not a newline\n", (int)new_line );
			delete [] buff;
			return( NULL );
		}
		int err = 0;
		for( j=image_height-1;j>=0 && !err;j-- )	{
#if 1
			n += (int)fread( (void*)( buff + j * image_width * num_color_comps ), sizeof( unsigned char ), image_width * num_color_comps, stream );
#else
			long b = (int)fread( (void*)( buff + j * image_width * num_color_comps ), sizeof( unsigned char ), size_t( image_width * num_color_comps ), stream );
			n += b;
			printf( "[%d]: (%d): %d : e:%d eof:%d\n", j, image_width * num_color_comps, b, ferror( stream ), feof( stream ) );
#endif
			if( ferror( stream ) || feof( stream ) )	{
				err = 1;
			}
		}
	}
	else	{
		float inv_max = 1.0f/max;
		float f;
		for( j=image_height-1;j>=0;j-- )	{
			for( i=0;i<image_width;i++ )	{
				for( k=0;k<num_color_comps;k++ )	{
					n += fscanf( stream, "%f", &f );
					buff[ ( j * image_width + i ) * num_color_comps + k ] = (unsigned char)( f * inv_max * 255.0 );
				}
			}
		}
	}
	if( n != image_bytes )	{
		printf( "Heightfield::parse_ppm ERR: read %d of %d components\n", (int)n, (int)image_bytes );
		delete [] buff;
		return( NULL );
	}
	return( buff );
}

unsigned char* Heightfield::read_ppm( const char* filename )	{

	FILE *stream = fopen( filename, "rb" );
	if( stream == NULL )	{
		printf( "Heightfield::read_ppm ERR: fopen '%s' FAILED\n", filename );
		return( NULL );
	}
	unsigned char* buff = parse_ppm( stream );
	fclose( stream );
	return( buff );
}

#ifdef WIN32
unsigned char* Heightfield::LoadBitmapFile(char *filename, BITMAPINFOHEADER * bitmapInfoHeader)
{
	FILE *filePtr; //the file pointer
	BITMAPFILEHEADER bitmapFileHeader; //bitmap file header
	unsigned char *bitmapImage; //bitmap image data

#if 0
	bitmapImage = (unsigned char*)malloc( 4 );
	image_width = 2;
	image_height = 2;
	num_color_comps = 1;
	bitmapImage[ 0 ] = 0;
	bitmapImage[ 1 ] = 63;
	bitmapImage[ 2 ] = 0;
	bitmapImage[ 3 ] = 255;
	return bitmapImage;
#endif

#if 0
	bitmapImage = (unsigned char*)malloc( 6 );
	image_width = 3;
	image_height = 2;
	num_color_comps = 1;
	bitmapImage[ 0 ] = 0;
	bitmapImage[ 1 ] = 0;
	bitmapImage[ 2 ] = 0;
	bitmapImage[ 3 ] = 63;
	bitmapImage[ 4 ] = 127;
	bitmapImage[ 5 ] = 255;
	return bitmapImage;
#endif

#if 0
	bitmapImage = (unsigned char*)malloc( 9 );
	image_width = 3;
	image_height = 3;
	num_color_comps = 1;
	bitmapImage[ 0 ] = 0;
	bitmapImage[ 1 ] = 0;
	bitmapImage[ 2 ] = 0;
	bitmapImage[ 3 ] = 0;
	bitmapImage[ 4 ] = 255;
	bitmapImage[ 5 ] = 0;
	bitmapImage[ 6 ] = 0;
	bitmapImage[ 7 ] = 0;
	bitmapImage[ 8 ] = 0;
	return bitmapImage;
#endif

	printf( "Heightfield::LoadBitmapFile: open '%s'\n", filename );

	//open filename in "read binary" mode
	filePtr = fopen( filename, "rb" );
	if( filePtr == NULL)	{
		printf( "Heightfield::LoadBitmapFile ERR: fopen '%s' FAILED\n", filename );
		return NULL;
	}
	
	//read the bitmap file header
	fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);

#if 0
	printf( "HF:BM: FileHeader:\n" );
	printf( "HF:BM: bfType %d == %d\n", bitmapFileHeader.bfType, BITMAP_ID );
	printf( "HF:BM: bfSize %d\n", bitmapFileHeader.bfSize );
#endif

	//verify that this is a bitmap by checking for the universal bitmap id
	if( bitmapFileHeader.bfType != BITMAP_ID )
	{
		fclose(filePtr);
		return NULL;
	}

	//read the bitmap information header
	fread(bitmapInfoHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);

#if 0
	printf( "HF:BM: InfoHeader:\n" );
	printf( "HF:BM: biSize %d\n", bitmapInfoHeader->biSize );
	printf( "HF:BM: biWidth %d\n", bitmapInfoHeader->biWidth );
	printf( "HF:BM: biHeight %d\n", bitmapInfoHeader->biHeight );
	printf( "HF:BM: biPlanes %d\n", bitmapInfoHeader->biPlanes );
	printf( "HF:BM: biBitCount %d\n", bitmapInfoHeader->biBitCount );
	printf( "HF:BM: biCompression %d\n", bitmapInfoHeader->biCompression );
	printf( "HF:BM: biSizeImage %d\n", bitmapInfoHeader->biSizeImage );
	printf( "HF:BM: biClrUsed %d\n", bitmapInfoHeader->biClrUsed );
	printf( "HF:BM: biClrImportant %d\n", bitmapInfoHeader->biClrImportant );
#endif

	image_width = bitmapInfoHeader->biWidth;
	image_height = bitmapInfoHeader->biHeight;
	long image_area = image_width * image_height;
	num_color_comps = 0;
	for( int i=1; num_color_comps == 0; i++ )	{
		if( ( image_area * ( i + 1 ) ) > (int)( bitmapFileHeader.bfSize ) )	{
			num_color_comps = i;
		}
	}
	printf( " image: width %d height %d comps %d\n", image_width, image_height, num_color_comps );
	long image_bytes = image_area * num_color_comps;

	//move file pointer to beginning of bitmap data
	fseek( filePtr, bitmapFileHeader.bfOffBits, SEEK_SET );

	//allocate enough memory for the bitmap image data
//	bitmapImage = (unsigned char*)malloc( image_bytes );
	bitmapImage = new unsigned char [ image_bytes ];

	//verify memory allocation
	if( bitmapImage == NULL )	{
		printf( "Heightfield::LoadBitmapFile ERR: malloc %d bytes FAILED\n", image_bytes );
		fclose(filePtr);
		return NULL;
	}

	//read in the bitmap image data
	size_t bytes_read = fread( bitmapImage, 1, image_bytes, filePtr );

	fclose(filePtr);

	//make sure bitmap image data was read
	if( (int)( bytes_read ) < image_bytes )	{
		printf( "Heightfield::LoadBitmapFile ERR: file '%s': read %d of %d bytes\n", bytes_read, image_bytes );
		return NULL;
	}

#if 0
	unsigned char tempRGB; //swap variable
	//swap the R and B values to get RGB since the bitmap color format is in BGR
	for( int imageIdx = 0; imageIdx < image_bytes; imageIdx += 3 )
	{
		tempRGB = bitmapImage[imageIdx];
		bitmapImage[imageIdx] = bitmapImage[imageIdx + 2];
		bitmapImage[imageIdx + 2] = tempRGB;
	}
#endif

	return bitmapImage;
}
#endif

float Heightfield::get_raw_elevation( int i, int j )	{

	if( i < 0 ) return( 0.0f );
	if( j < 0 ) return( 0.0f );
	if( i >= mesh_resx ) return( 0.0f );
	if( j >= mesh_resz ) return( 0.0f );
	return( vertex_arr[ ( j * mesh_resx + i ) * 3 + 1 ] );
}

float Heightfield::get_elevation( float px, float pz, float *normal_p )	{

	if( vertex_arr )	{

		if( dirty_normals )	{
			load_normals();
		}

		float nx = ( px - mesh_origin[ 0 ] ) / mesh_scale[ 0 ];
		float nz = ( pz - mesh_origin[ 2 ] ) / mesh_scale[ 2 ];

		if( ( nx < 0.0 )||( nz < 0.0 )||( nx >= 1.0 )||( nz >= 1.0 ) )	{
			if( normal_p != NULL )	{
				normal_p[ 0 ] = 0.0;
				normal_p[ 1 ] = 1.0;
				normal_p[ 2 ] = 0.0;
			}
			return( 0.0 );
		}

		float ix = nx * ( mesh_resx - 1 );
		float iz = nz * ( mesh_resz - 1 );

		int i = (int) ix;
		int j = (int) iz;

		float dx = ix - (float)i;
		float dz = iz - (float)j;

		float y, y0;

		if( ( dx + dz ) < 1.0f )	{

			y0 = get_raw_elevation( i, j );
			y = y0 
				+ ( get_raw_elevation( i + 1, j ) - y0 ) * dx
				+ ( get_raw_elevation( i, j + 1 ) - y0 ) * dz;

			if( normal_p != NULL )	{
				int ni = ( ( j * ( mesh_resx - 1 ) + i ) * 2 ) * 3;
				float *np = normal_arr + ni;
				normal_p[ 0 ] = np[ 0 ];
				normal_p[ 1 ] = np[ 1 ];
				normal_p[ 2 ] = np[ 2 ];
			}
		}
		else	{

			y0 = get_raw_elevation( i + 1, j + 1 );
			y = y0 
				+ ( get_raw_elevation( i + 1, j ) - y0 ) * ( 1.0f - dz )
				+ ( get_raw_elevation( i, j + 1 ) - y0 ) * ( 1.0f - dx );

			if( normal_p != NULL )	{
				int ni = ( ( j * ( mesh_resx - 1 ) + i ) * 2 + 1 ) * 3;
				float *np = normal_arr + ni;
				normal_p[ 0 ] = np[ 0 ];
				normal_p[ 1 ] = np[ 1 ];
				normal_p[ 2 ] = np[ 2 ];
			}
		}
		
		float py = y * mesh_scale[ 1 ] + mesh_origin[ 1 ];
		return( py );
	}
	else
	if( normal_p != NULL )	{
		normal_p[ 0 ] = 0.0;
		normal_p[ 1 ] = 1.0;
		normal_p[ 2 ] = 0.0;
	}
    return( 0.0 );
}






