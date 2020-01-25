// Viewer here: https://www.kylepaulsen.com/stuff/NetpbmViewer/
#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include "FastNoise.h"
#include "Vec.h"

const double ColorSteps = 255.0;
const int NOISE_MULTIPLE = 7;

float intersectSphere(vec3 origin, vec3 dir, vec3 center, float radius)
{
	vec3 oc = origin - center;
	float a = dir.dot(dir);
	float b = 2.0f * oc.dot(dir);
	float c = oc.dot(oc) - radius * radius;
	float discriminant = b * b - 4 * a * c;
	if (discriminant < 0.0f)
	{
		return -1.0f;
	}
	return (-b - sqrtf(discriminant)) / (2.0f * a);
}

void FillColors(std::vector<std::string>& colors)
{
	double red = 0, green = 0, blue = 0;
	// Set your color here. Be sure to set the maximum value you set
	// Below in the code which writes the header
	int redMax = 105, greenMax = 2, blueMax = 116;
	double redStep	= redMax / ColorSteps;
	double greenStep = greenMax / ColorSteps;
	double blueStep	= blueMax / ColorSteps;

	for (int i = 0; i <= ColorSteps; i++)
	{
		char buffer[64];
		sprintf_s(buffer, sizeof(buffer), "%.0f %.0f %.0f\n", red,  green, blue);
		red += redStep, green += greenStep, blue += blueStep;
		colors.push_back(buffer);
	}
};

int main()
{
	// Image width and height.
	const int width = 1000;
	const int height = 1000;

	std::ofstream outfile;
	outfile.open("c:/temp/raytrace.txt");
	// Write the header
	// P3 is color, next is image size in pixels, finally 255 represents brightest
	// color it will encounter
	outfile << "P3" << std::endl << "1000 1000" << std::endl << "255" << std::endl;

	FastNoise myNoise; // Create a FastNoise object
	// Adjust these values to change your texture
	myNoise.SetNoiseType(FastNoise::PerlinFractal); // Set the desired noise type
	myNoise.SetSeed(1337);
	myNoise.SetFrequency(0.08);
	myNoise.SetFractalType(FastNoise::FBM);
	myNoise.SetInterp(FastNoise::Quintic);
	myNoise.SetFractalOctaves(8);
	myNoise.SetFractalLacunarity(1.5);
	myNoise.SetFractalGain(0.02);
	myNoise.SetCellularDistanceFunction(FastNoise::Euclidean);
	myNoise.SetGradientPerturbAmp(30.0);
	
	// For square pixels, e.g. PPM file, set to 1.0f
	const float pixelAspect = 1.0f;

	// Sphere position and radius.
	// Adjust the 3rd parameter to move the camera back. Too close
	// and you will have a black hole in the sphere. If you have a black
	// hole, just increase the value. It should be close, just under, the radius
	const vec3 center{ 0.0f, 0.0f, 360.0f };

	// Adjust this to make the spere bigger
	const float radius = 382.0f;

	// Light shines from top left 
	vec3 light{ 1.0f, -1.0f, 2.0f };
	light.normalize();

	// Darkest to lightest
	std::vector<std::string> colors;
	FillColors(colors);
	const size_t numColors = colors.size();

	// Ray direction - all parallel, down Z axis.
	const vec3 dir{ 0.0f, 0.0f, 1.0f };

	// For each pixel...
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			float xWorld = x - (width / 2) + 0.5f;
			float yWorld = (y - (height / 2) + 0.5f) * pixelAspect;
			vec3 origin = vec3{ xWorld, yWorld, -30.0f };

			float t = intersectSphere(origin, dir, center, radius);

			if (t > 0.0f)
			{
				// Ray hit.
				vec3 intersection = origin + (dir * t);
				vec3 normal = intersection - center;
				normal.normalize();
				float directional;
				directional = normal.dot(-light) * 0.95f;

				if (directional < 0.0f)
				{
					directional = 0.0f;
				}

				float ambient = 0.05f;

				float lum = directional + ambient;

				if (lum < 0)
				{
					// Give some brightness even if pointing
					// completely away from the light.
					// With 256 shades, 1 isn't enough
					outfile << colors[42];
				}
				else
				{
					auto per = myNoise.GetNoise(x, y);
					// Notice I'm multiplying the noise by a value
					// I found that too little noise will result in 
					// too much black in the texture
					auto value = (numColors * lum * (per * NOISE_MULTIPLE));
					// Adjust for range
					if (value > ColorSteps)
						value = ColorSteps;
					if (value < 0)
						value = 0;
					outfile << colors[(int)value];
				}
			}
			else
			{
				// Ray miss. Make it solid black
				outfile << colors[0];
			}
		}
		outfile << std::endl;
	}
	outfile.close();
	return 0;
}
