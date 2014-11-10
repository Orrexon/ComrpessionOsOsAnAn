// main.c

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <bitset>
#include <sstream>

#define SIZE 196666

#pragma pack(push, 1)
struct BMPHeader
{
	uint16_t signature;		// 0x4D42
	uint32_t size;				// unreliable
	uint16_t res0;				// reserved 0
	uint16_t res1;				// reserved 0
	uint32_t offset;			// to start of image
	uint32_t hdrsize;			// 40
	uint32_t width;
	uint32_t height;
	uint16_t planes;			// 1
	uint16_t bpp;					// 1, 4, 8 or 24
	uint32_t compress;		// 0 = none, 1 = rle-8, 2=rle-4
	uint32_t total;				// size in bytes incl padding
	uint32_t h_res;
	uint32_t v_res;
	uint32_t num_color;
	uint32_t imp_color;
};
#pragma pack(pop)

struct Color
{
	Color() 
	{
		m_r = 0;
		m_g = 0;
		m_b = 0;
	}
	
	Color(unsigned long r, unsigned long g, unsigned long b)
	{
		m_r = r;
		m_g = g;
		m_b = b;
	}

	bool operator==(const Color& rhs)
	{
		if (m_r == rhs.m_r && m_g == rhs.m_g && m_b == rhs.m_b)
			return true;
		return false;
	}

	friend std::ostream& operator<<(std::ostream& ost, const Color& rhs)
	{
		if (rhs.m_r < 10)
			ost << "00" << rhs.m_r;
		else if (rhs.m_r < 100)
			ost << "0" << rhs.m_r;
		else
			ost << rhs.m_r;

		if (rhs.m_g < 10)
			ost << "00" << rhs.m_g;
		else if (rhs.m_g < 100)
			ost << "0" << rhs.m_g;
		else
			ost << rhs.m_g;
			
		if (rhs.m_b < 10)
			ost << "00" << rhs.m_b;
		else if (rhs.m_b < 100)
			ost << "0" << rhs.m_b;
		else
			ost << rhs.m_b;

		ost << std::endl;
		return ost;
	}

	unsigned long  m_r;
	unsigned long  m_g;
	unsigned long  m_b;
};

int main()
{
	{
		FILE* file;
		struct BMPHeader header;
		char buffer[SIZE], *pixels;

		file = fopen("test.bmp", "rb");
		fread(buffer, 1, SIZE, file);
		fclose(file);

		memcpy(&header, buffer, sizeof(struct BMPHeader));
		pixels = buffer + header.offset;

		std::ofstream ofs;
		ofs.open("compressed.dat", std::ios_base::binary);

		// Write header data
		ofs << header.signature << std::endl;
		ofs << header.size << std::endl;
		ofs << header.res0 << std::endl;
		ofs << header.res1 << std::endl;
		ofs << header.offset << std::endl;
		ofs << header.hdrsize << std::endl;
		ofs << header.width << std::endl;
		ofs << header.height << std::endl;
		ofs << header.planes << std::endl;
		ofs << header.bpp << std::endl;
		ofs << header.compress << std::endl;
		ofs << header.total << std::endl;
		ofs << header.h_res << std::endl;
		ofs << header.v_res << std::endl;
		ofs << header.num_color << std::endl;
		ofs << header.imp_color << std::endl;

		// Write pixel data
		unsigned int runCount = 1;
		Color previousColor;
		for (int i = 0; i < header.width * header.height * 3; i += 3)
		{
			Color newColor(std::bitset<8>(pixels[i + 2]).to_ulong(), std::bitset<8>(pixels[i + 1]).to_ulong(), std::bitset<8>(pixels[i]).to_ulong());
			if (previousColor == newColor)
			{
				if (i == 0 && previousColor == Color(0, 0, 0))
				{
					//foobar
				}
				else
					runCount += 1;
			}
			else
			{
				if (i == 0 && previousColor == Color(0, 0, 0))
				{
					//foobar
				}
				else
				{
					ofs << runCount << "x" << previousColor;
					runCount = 1;
				}
			}
			previousColor = newColor;
		}
		ofs << runCount << "x" << previousColor;
		ofs.close();
	
		BMPHeader nheader;
		char* npixels;

		// Make new bmp file
		FILE *nfile = fopen("DOODLEMEAT.bmp", "wb");

		/*RGBQUAD palette[256];
		for (int i = 0; i < 256; ++i)
		{
			palette[i].rgbBlue = (byte)i;
			palette[i].rgbGreen = (byte)i;
			palette[i].rgbRed = (byte)i;
		}
		*/

		std::ifstream ifs("compressed.dat");
		std::string line;
		int lineCount = 0;
		while (std::getline(ifs, line))
		{
			if (lineCount < 16)
			{
				switch (lineCount)
				{
				case 0: nheader.signature = uint16_t(stoi(line)); break;
				case 1: nheader.size = uint32_t(stoi(line)); break;
				case 2: nheader.res0 = uint16_t(stoi(line)); break;
				case 3: nheader.res1 = uint16_t(stoi(line)); break;
				case 4: nheader.offset = uint32_t(stoi(line)); break;
				case 5: nheader.hdrsize = uint32_t(stoi(line)); break;
				case 6: nheader.width = uint32_t(stoi(line)); break;
				case 7: nheader.height = uint32_t(stoi(line)); break;
				case 8: nheader.planes = uint16_t(stoi(line)); break;
				case 9: nheader.bpp = uint16_t(stoi(line)); break;
				case 10: nheader.compress = uint32_t(stoi(line)); break;
				case 11: nheader.total = uint32_t(stoi(line)); break;
				case 12: nheader.h_res = uint32_t(stoi(line)); break;
				case 13: nheader.v_res = uint32_t(stoi(line)); break;
				case 14: nheader.num_color = uint32_t(stoi(line)); break;
				case 15: nheader.imp_color = uint32_t(stoi(line)); break;
				}

				if (lineCount == 15)
				{
					fwrite(&nheader, sizeof(BMPHeader), 1, nfile);
				}
			}
			else
			{
				// Write pixels!!!!!
				std::string lhs;
				std::string rhs;
				bool hasFoundX = false;
				for (int i = 0; i < line.size(); i++)
				{
					if (line[i] == 'x')
					{
						hasFoundX = true;
						continue;
					}

					if (!hasFoundX)
					{
						lhs += line[i];
					}
					else
					{
						rhs += line[i];
					}
				}

				char r;
				char g;
				char b;

				r = (char)stoi(rhs.substr(6, 3));
				g = (char)stoi(rhs.substr(3, 3));
				b = (char)stoi(rhs.substr(0, 3));

				int lsh_length = stoi(lhs);
				for (int i = 0; i < lsh_length; i++)
				{
					fwrite(&r, sizeof(char), 1, nfile);
					fwrite(&g, sizeof(char), 1, nfile);
					fwrite(&b, sizeof(char), 1, nfile);
				}
			}
			lineCount += 1;
		}


		fclose(nfile);

	}
	return 0;
}
