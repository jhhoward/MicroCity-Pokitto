#include <stdio.h>
#include <stdint.h>
#include <iostream>
#include <fstream>
#include "../MicroCity/lodepng.cpp"

#define GENERATED_PATH "Source/MicroCity/Generated"

constexpr int numColours = 16;
uint16_t palette[numColours];
uint8_t paletteRGB[numColours * 3];
const char* palettePath = "Images/palette.png";
const char* paletteOutputPath = GENERATED_PATH "/Palette.inc.h";
using namespace std;

uint16_t ToRGB565(uint8_t red, uint8_t green, uint8_t blue)
{
	uint16_t b = (blue >> 3) & 0x1f;
	uint16_t g = ((green >> 2) & 0x3f) << 5;
	uint16_t r = ((red >> 3) & 0x1f) << 11;

	return (uint16_t)(r | g | b);
}

int GetPaletteIndex(uint8_t red, uint8_t green, uint8_t blue)
{
	int closest = -1;
	int closestDist = 0;

	for (int n = 0; n < numColours; n++)
	{
		int distance = 0;
		distance += (red - paletteRGB[n * 3]) * (red - paletteRGB[n * 3]);
		distance += (green - paletteRGB[n * 3 + 1]) * (green - paletteRGB[n * 3 + 1]);
		distance += (blue - paletteRGB[n * 3 + 2]) * (blue - paletteRGB[n * 3 + 2]);
		if (closest == -1 || distance < closestDist)
		{
			closest = n;
			closestDist = distance;
		}
	}

	return closest;
}

bool LoadPalette()
{
	vector<unsigned char> pixels;
	unsigned width, height;
	unsigned error = lodepng::decode(pixels, width, height, palettePath);

	if (error)
	{
		cerr << "Error loading palette file" << endl;
		return false;
	}

	if (width != numColours || height != 1)
	{
		cerr << "Palette wrong dimensions" << endl;
		return false;
	}

	for (int n = 0; n < numColours; n++)
	{
		palette[n] = ToRGB565(pixels[n * 4], pixels[n * 4 + 1], pixels[n * 4 + 2]);
		paletteRGB[n * 3] = pixels[n * 4];
		paletteRGB[n * 3 + 1] = pixels[n * 4 + 1];
		paletteRGB[n * 3 + 2] = pixels[n * 4 + 2];
	}

	ofstream paletteHeader;
	paletteHeader.open(paletteOutputPath);
	paletteHeader << "const uint16_t MicroCityPalette[] = {" << endl << "  ";
	for (int n = 0; n < numColours; n++)
	{
		paletteHeader << palette[n];
		if (n != numColours - 1)
		{
			paletteHeader << ", ";
		}
	}
	paletteHeader << endl << "};" << endl;
	paletteHeader.close();

	return true;
}

void LoadTilesImage(const char* filename, const char* outputPath)
{
	vector<unsigned char> pixels;
	unsigned width, height;
	unsigned error = lodepng::decode(pixels, width, height, filename);

	if (!error)
	{
		int tilesX = width / 8;
		int tilesY = height / 8;

		ofstream os;
		os.open(outputPath);
		os << "// From " << filename << " : " << width << " x " << height << endl;

		for (int ty = 0; ty < tilesY; ty++)
		{
			for (int tx = 0; tx < tilesX; tx++)
			{
				for (int y = 0; y < 8; y++)
				{
					for (int x = 0; x < 8; x++)
					{
						int n = (ty * 8 + y) * width + (tx * 8 + x);
						int index = GetPaletteIndex(pixels[n * 4], pixels[n * 4 + 1], pixels[n * 4 + 2]);
						os << index;

						if (n < width * height - 1)
						{
							os << ", ";
						}
					}
				}

				os << endl;
			}
		}

		os.close();
	}
	else
	{
		cerr << "Failed to load " << filename << endl;
	}
}

void LoadImage(const char* filename, const char* outputPath)
{
	vector<unsigned char> pixels;
	unsigned width, height;
	unsigned error = lodepng::decode(pixels, width, height, filename);

	if (!error)
	{
		ofstream os;
		os.open(outputPath);
		os << "// From " << filename << " : " << width << " x " << height << endl;

		for (int n = 0; n < width * height; n++)
		{
			if ((n % 32) == 0 && n > 0)
			{
				os << endl;
			}

			int index = GetPaletteIndex(pixels[n * 4], pixels[n * 4 + 1], pixels[n * 4 + 2]);
			os << index;
			if (n < width * height - 1)
			{
				os << ", ";
			}
		}

		os.close();
	}
	else
	{
		cerr << "Failed to load " << filename << endl;
	}
}

int main(int argc, char* argv[])
{
	if (!LoadPalette())
	{
		cerr << "Failed to load palette" << endl;
		return 0;
	}

	LoadTilesImage("Images/tiles-colour.png", GENERATED_PATH "/TileData.inc.h");
	LoadImage("Images/logo-colour.png", GENERATED_PATH "/LogoData.inc.h");

	return 0;
}
