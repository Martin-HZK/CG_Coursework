#pragma once
#include <glad/glad.h> 
#include <iostream>
#include "bitmap.h"

GLuint setup_texture(const char* filename)
{
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);

	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	unsigned char* pixelBuffer = NULL;
	BITMAPINFOHEADER info;
	BITMAPFILEHEADER file;
	loadbitmap(filename, pixelBuffer, &info, &file);

	if (pixelBuffer != NULL)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, info.biWidth, info.biHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, pixelBuffer);
	}

	delete[] pixelBuffer;

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	return tex;
}

GLuint setup_mipmaps(const char* filename[], int n)
{
	return 0;
}
