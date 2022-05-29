//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//           INSTITUTO POLITÉCNICO DO CÁVADO E DO AVE
//                          2011/2012
//             ENGENHARIA DE SISTEMAS INFORMÁTICOS
//                    VISÃO POR COMPUTADOR
//
//             [  DUARTE DUQUE - dduque@ipca.pt  ]
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Desabilita (no MSVC++) warnings de fun��es n�o seguras (fopen, sscanf, etc...)
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <malloc.h>
#include <stdlib.h>
#include "vc.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//            FUN��ES: ALOCAR E LIBERTAR UMA IMAGEM
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Alocar mem�ria para uma imagem
IVC *vc_image_new(int width, int height, int channels, int levels)
{
	IVC *image = (IVC *)malloc(sizeof(IVC));

	if (image == NULL)
		return NULL;
	if ((levels <= 0) || (levels > 255))
		return NULL;

	image->width = width;
	image->height = height;
	image->channels = channels;
	image->levels = levels;
	image->bytesperline = image->width * image->channels;
	image->data = (unsigned char *)malloc(image->width * image->height * image->channels * sizeof(char));

	if (image->data == NULL)
	{
		return vc_image_free(image);
	}

	return image;
}

// Libertar mem�ria de uma imagem
IVC *vc_image_free(IVC *image)
{
	if (image != NULL)
	{
		if (image->data != NULL)
		{
			free(image->data);
			image->data = NULL;
		}

		free(image);
		image = NULL;
	}

	return image;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//    FUN��ES: LEITURA E ESCRITA DE IMAGENS (PBM, PGM E PPM)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Gerar negativo da imagem gray
int vc_gray_negative(IVC *srcdst)
{
	unsigned char *data = (unsigned char *)srcdst->data;
	int width = srcdst->width;
	int height = srcdst->height;
	int bytesperline = srcdst->width * srcdst->channels;
	int channels = srcdst->channels;
	int x, y;
	long int pos;

	// verificação de erros
	if ((srcdst->width <= 0) || (srcdst->height <= 0) || (srcdst->data == NULL))
		return 0;
	if (channels != 1)
		return 0;

	// inverter imagem
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			pos = y * bytesperline + x * channels;
			data[pos] = 255 - data[pos];
		}
	}

	return 1;
}

float myMax(float a, float b, float c)
{
	if (a >= b && a >= c)
	{
		return a;
	}
	else if (b >= a && b >= c)
	{
		return b;
	}
	else
	{
		return c;
	}
}

float myMin(float a, float b, float c)
{
	if (a <= b && a <= c)
	{
		return a;
	}
	else if (b <= a && b <= c)
	{
		return b;
	}
	else
	{
		return c;
	}
}

int vc_rgb_to_hsv(IVC *src, IVC *dst)
{

	unsigned char *datasrc = (unsigned char *)src->data;
	int bytesperline_src = src->width * src->channels;
	int channels_src = src->channels;

	unsigned char *datadst = (unsigned char *)dst->data;
	int bytesperline_dst = dst->width * dst->channels;
	int channels_dst = dst->channels;

	int width = src->width;
	int height = src->height;
	int x, y;
	long int pos_src, pos_dst;
	float h, s, v;
	float r, g, b;
	float Max, Min;
	// Verificação de erros
	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL))
		return 0;
	if ((src->width != dst->width) || (src->height != dst->height))
		return 0;
	if ((src->channels != 3) || (dst->channels != 3))
		return 0;
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			pos_src = y * bytesperline_src + x * channels_src;
			pos_dst = y * bytesperline_dst + x * channels_dst;

			r = (float)datasrc[pos_src];
			g = (float)datasrc[pos_src + 1];
			b = (float)datasrc[pos_src + 2];

			Max = myMax(r, g, b);
			Min = myMin(r, g, b);
			v = Max;
			if (v == 0 || Min == Max)
			{
				s = 0;
				h = 0;
			}
			else
			{
				s = ((v - Min) / v) * 255;
			}

			if (Max == r)
			{
				if (g >= b)
				{
					h = (60 * (g - b) / (Max - Min)) / 360 * 255;
				}
				else
				{
					h = (360 + 60 * (g - b) / (Max - Min)) / 360 * 255;
				}
			}
			else if (Max == g)
			{
				h = (120 + 60 * (b - r) / (Max - Min)) / 360 * 255;
			}
			else
			{
				h = (240 + 60 * (r - g) / (Max - Min)) / 360 * 255;
			}

			datadst[pos_dst] = h;
			datadst[pos_dst + 1] = s;
			datadst[pos_dst + 2] = v;
		}
	}
	return 1;
}

int vc_hsv_segmentation(IVC *src, IVC *dst, int hmin, int hmax, int smin, int smax, int vmin, int vmax)
{
	unsigned char *datasrc = (unsigned char *)src->data;
	int bytesperline_src = src->width * src->channels;
	int channels_src = src->channels;

	unsigned char *datadst = (unsigned char *)dst->data;
	int bytesperline_dst = dst->width * dst->channels;
	int channels_dst = dst->channels;

	int width = src->width;
	int height = src->height;
	int x, y;
	long int pos_src, pos_dst;
	float h, s, v;

	// Verificação de erros
	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL))
		return 0;
	if ((src->width != dst->width) || (src->height != dst->height))
		return 0;
	if ((src->channels != 3) || (dst->channels != 1))
		return 0;
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			pos_src = y * bytesperline_src + x * channels_src;
			pos_dst = y * bytesperline_dst + x * channels_dst;

			h = (float)datasrc[pos_src];
			s = ((float)datasrc[pos_src + 1] * 100) / 255;
			v = ((float)datasrc[pos_src + 2] * 100) / 255;

			if (((h <= hmax) && (h >= hmin)) && ((s <= smax) && (s >= smin)) && ((v <= vmax) && (v >= vmin)))
			{

				datadst[pos_dst] = 255;
			}
			else
			{
				datadst[pos_dst] = 0;
			}
		}
	}
	return 1;
}

int vc_scale_gray_to_rgb(IVC *src, IVC *dst)
{
	unsigned char *datasrc = (unsigned char *)src->data;
	int bytesperline_src = src->width * src->channels;
	int channels_src = src->channels;

	unsigned char *datadst = (unsigned char *)dst->data;
	int bytesperline_dst = dst->width * dst->channels;
	int channels_dst = dst->channels;

	int width = src->width;
	int height = src->height;
	int x, y;
	long int pos_src, pos_dst;
	float c, r, g, b;

	// Verificação de erros
	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL))
		return 0;
	if ((src->width != dst->width) || (src->height != dst->height))
		return 0;
	if ((src->channels != 1) || (dst->channels != 3))
		return 0;
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{

			pos_src = y * bytesperline_src + x * channels_src;
			pos_dst = y * bytesperline_dst + x * channels_dst;

			c = (float)datasrc[pos_src];

			if (c >= 0 && c <= 63)
			{
				r = 0;
				g = c * 4; //?
				b = 255;
			}
			else if (c >= 64 && c <= 127)
			{
				r = 0;
				g = 255;
				b = (255 - (c - 64)) * 4; //?
			}
			else if (c >= 128 && c <= 191)
			{
				r = (c - 128) * 4; //?
				g = 255;
				b = 0;
			}
			else if (c >= 192 && c <= 255)
			{
				r = 255;
				g = (255 - (c - 192)) * 4; //?
				b = 0;
			}
			datadst[pos_dst] = r;
			datadst[pos_dst + 1] = g;
			datadst[pos_dst + 2] = b;
		}
	}
	return 1;
}

int vc_gray_to_binary(IVC *src, IVC *dst, int threshold)
{
	unsigned char *datasrc = (unsigned char *)src->data;
	int bytesperline_src = src->width * src->channels;
	int channels_src = src->channels;

	unsigned char *datadst = (unsigned char *)dst->data;
	int bytesperline_dst = dst->width * dst->channels;
	int channels_dst = dst->channels;

	int width = src->width;
	int height = src->height;
	int x, y;
	long int pos_src, pos_dst;
	float src_gray;

	// Verificação de erros
	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL))
		return 0;
	if ((src->width != dst->width) || (src->height != dst->height))
		return 0;
	if ((src->channels != 1) || (dst->channels != 1))
		return 0;
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			pos_src = y * bytesperline_src + x * channels_src;
			pos_dst = y * bytesperline_dst + x * channels_dst;
			src_gray = (float)datasrc[pos_src];

			if (src_gray > threshold)
			{
				datadst[pos_dst] = 255;
			}
			else
			{

				datadst[pos_dst] = 0;
			}
		}
	}
}

int vc_gray_to_binary_midpoint(IVC *src, IVC *dst, int kernel_width)
{
	unsigned char *datasrc = (unsigned char *)src->data;
	int bytesperline_src = src->width * src->channels;
	int channels_src = src->channels;

	unsigned char *datadst = (unsigned char *)dst->data;
	int bytesperline_dst = dst->width * dst->channels;
	int channels_dst = dst->channels;

	int width = src->width;
	int height = src->height;
	int x, y, xk, yk;
	long int pos_src, pos_dst;
	float src_gray, vmin, vmax, offset, threshold;

	// Verificação de erros
	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL))
		return 0;
	if ((src->width != dst->width) || (src->height != dst->height))
		return 0;
	if ((src->channels != 1) || (dst->channels != 1))
		return 0;

	offset = (kernel_width - 1) / 2;

	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			threshold = (vmin + vmax) / 2;
			pos_src = y * bytesperline_src + x * channels_src;

			src_gray = (float)datasrc[pos_src];
			vmin, vmax = src_gray;

			for (yk = -offset; yk < offset; yk++)
			{
				for (xk = -offset; xk < offset; xk++)
				{
					if (((y + yk) >= 0 && (y + yk) < src->height) && ((x + xk) >= 0 && (x + xk) < src->width))
					{
						pos_src = (y + yk) * bytesperline_src + (x + xk) * channels_src;
						src_gray = (float)datasrc[pos_src];
						if (src_gray < vmin)
						{
							vmin = src_gray;
						}
						if (src_gray > vmax)
						{
							vmax = src_gray;
						}
					}
				}
			}
			threshold = (vmin + vmax) / 2;
			pos_dst = y * bytesperline_dst + x * channels_dst;
			pos_src = y * bytesperline_src + x * channels_src;

			src_gray = (float)datasrc[pos_src];
			if (src_gray > threshold)
			{
				datadst[pos_dst] = 255;
			}
			else
			{

				datadst[pos_dst] = 0;
			}
		}
	}
	return 1;
}

int vc_gray_to_binary_bersen(IVC *src, IVC *dst, int kernel_width, int cmin)
{
	unsigned char *datasrc = (unsigned char *)src->data;
	int bytesperline_src = src->width * src->channels;
	int channels_src = src->channels;

	unsigned char *datadst = (unsigned char *)dst->data;
	int bytesperline_dst = dst->width * dst->channels;
	int channels_dst = dst->channels;

	int width = src->width;
	int height = src->height;
	int x, y, xk, yk;
	long int pos_src, pos_dst;
	float src_gray, vmin, vmax, offset, threshold;

	// Verificação de erros
	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL))
		return 0;
	if ((src->width != dst->width) || (src->height != dst->height))
		return 0;
	if ((src->channels != 1) || (dst->channels != 1))
		return 0;

	offset = (kernel_width - 1) / 2;

	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			threshold = (vmin + vmax) / 2;
			pos_src = y * bytesperline_src + x * channels_src;

			src_gray = (float)datasrc[pos_src];

			vmin, vmax = src_gray;
			for (yk = -offset; yk < offset; yk++)
			{
				for (xk = -offset; xk < offset; xk++)
				{
					if (((y + yk) >= 0 && (y + yk) < src->height) && ((x + xk) >= 0 && (x + xk) < src->width))
					{
						pos_src = (y + yk) * bytesperline_src + (x + xk) * channels_src;
						src_gray = (float)datasrc[pos_src];
						if (src_gray < vmin)
						{
							vmin = src_gray;
						}
						if (src_gray > vmax)
						{
							vmax = src_gray;
						}
					}
				}
			}

			if ((vmax - vmin) < cmin)
			{
				threshold = src->levels / 2;
			}
			else
			{
				threshold = (vmin + vmax) / 2;
			}
			pos_dst = y * bytesperline_dst + x * channels_dst;
			pos_src = y * bytesperline_src + x * channels_src;

			src_gray = (float)datasrc[pos_src];
			if (src_gray > threshold)
			{
				datadst[pos_dst] = 255;
			}
			else
			{

				datadst[pos_dst] = 0;
			}
		}
	}
	return 1;
}

int vc_gray_to_binary_niblack(IVC *src, IVC *dst, int kernel_width, float k)
{

	unsigned char *datasrc = (unsigned char *)src->data;
	int bytesperline_src = src->width * src->channels;
	int channels_src = src->channels;

	unsigned char *datadst = (unsigned char *)dst->data;
	int bytesperline_dst = dst->width * dst->channels;
	int channels_dst = dst->channels;

	int width = src->width;
	int height = src->height;
	int x, y, xk, yk;
	long int pos_src, pos_dst;
	float src_gray, vmin, vmax, offset, threshold, u, ro, sum_vi, sum_vi_ro;

	// Verificação de erros
	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL))
		return 0;
	if ((src->width != dst->width) || (src->height != dst->height))
		return 0;
	if ((src->channels != 1) || (dst->channels != 1))
		return 0;

	offset = (kernel_width - 1) / 2;

	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			threshold = (vmin + vmax) / 2;
			pos_src = y * bytesperline_src + x * channels_src;

			src_gray = (float)datasrc[pos_src];

			u, ro, sum_vi, sum_vi_ro = 0;

			for (yk = -offset; yk < offset; yk++)
			{
				for (xk = -offset; xk < offset; xk++)
				{
					if (((y + yk) >= 0 && (y + yk) < src->height) && ((x + xk) >= 0 && (x + xk) < src->width))
					{
						pos_src = (y + yk) * bytesperline_src + (x + xk) * channels_src;
						src_gray = (float)datasrc[pos_src];
						sum_vi += src_gray;
					}
				}
			}

			u = (sum_vi) / pow(kernel_width, 2);
			for (yk = -offset; yk < offset; yk++)
			{
				for (xk = -offset; xk < offset; xk++)
				{
					if (((y + yk) >= 0 && (y + yk) < src->height) && ((x + xk) >= 0 && (x + xk) < src->width))
					{
						pos_src = (y + yk) * bytesperline_src + (x + xk) * channels_src;
						src_gray = (float)datasrc[pos_src];
						sum_vi_ro += pow((src_gray - u), 2);
					}
				}
			}
			ro = sqrt((sum_vi_ro / pow(kernel_width, 2)));

			threshold = u + k * ro;
			pos_dst = y * bytesperline_dst + x * channels_dst;
			pos_src = y * bytesperline_src + x * channels_src;

			src_gray = (float)datasrc[pos_src];
			if (src_gray > threshold)
			{
				datadst[pos_dst] = 255;
			}
			else
			{

				datadst[pos_dst] = 0;
			}
		}
	}
	return 1;
}

int vc_gray_to_binary_global_mean(IVC *src, IVC *dst)
{
	unsigned char *datasrc = (unsigned char *)src->data;
	int bytesperline_src = src->width * src->channels;
	int channels_src = src->channels;

	unsigned char *datadst = (unsigned char *)dst->data;
	int bytesperline_dst = dst->width * dst->channels;
	int channels_dst = dst->channels;

	int width = src->width;
	int height = src->height;
	int x, y;
	long int pos_src, pos_dst;
	float src_gray, threshold, sum_src;

	// Verificação de erros
	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL))
		return 0;
	if ((src->width != dst->width) || (src->height != dst->height))
		return 0;
	if ((src->channels != 1) || (dst->channels != 1))
		return 0;
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			pos_src = y * bytesperline_src + x * channels_src;
			pos_dst = y * bytesperline_dst + x * channels_dst;
			src_gray = (float)datasrc[pos_src];
			sum_src += src_gray;
		}
	}

	threshold = sum_src / (src->width * src->height);

	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			pos_src = y * bytesperline_src + x * channels_src;
			pos_dst = y * bytesperline_dst + x * channels_dst;
			src_gray = (float)datasrc[pos_src];

			if (src_gray > threshold)
			{
				datadst[pos_dst] = 255;
			}
			else
			{

				datadst[pos_dst] = 0;
			}
		}
	}
}

int vc_binary_dilate(IVC *src, IVC *dst, int kernel_width)
{
	unsigned char *datasrc = (unsigned char *)src->data;
	int bytesperline_src = src->width * src->channels;
	int channels_src = src->channels;

	unsigned char *datadst = (unsigned char *)dst->data;
	int bytesperline_dst = dst->width * dst->channels;
	int channels_dst = dst->channels;

	int width = src->width;
	int height = src->height;
	int x, y, xk, yk;
	long int pos_src, pos_dst;
	float src_gray, vmin, vmax, offset, threshold;
	int hasWhite = 0;

	// Verificação de erros
	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL))
		return 0;
	if ((src->width != dst->width) || (src->height != dst->height))
		return 0;
	if ((src->channels != 1) || (dst->channels != 1))
		return 0;

	offset = (kernel_width - 1) / 2;

	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{

			pos_src = y * bytesperline_src + x * channels_src;
			hasWhite = 0;
			for (yk = -offset; yk < offset; yk++)
			{
				for (xk = -offset; xk < offset; xk++)
				{
					if (((y + yk) >= 0 && (y + yk) < src->height) && ((x + xk) >= 0 && (x + xk) < src->width))
					{
						pos_src = (y + yk) * bytesperline_src + (x + xk) * channels_src;
						src_gray = (float)datasrc[pos_src];
						if (src_gray > 0)
						{

							hasWhite = 1;
						}
					}
				}
			}

			pos_dst = y * bytesperline_dst + x * channels_dst;
			pos_src = y * bytesperline_src + x * channels_src;

			if (hasWhite == 1)
			{
				datadst[pos_dst] = 255;
			}
			else
			{
				datadst[pos_dst] = 0;
			}
		}
	}
	return 1;
}

int vc_binary_subtract(IVC *src, IVC *dst)
{
	// src -> skull
	// dst -> skull and brain
	unsigned char *datasrc = (unsigned char *)src->data;
	int bytesperline_src = src->width * src->channels;
	int channels_src = src->channels;

	unsigned char *datadst = (unsigned char *)dst->data;
	int bytesperline_dst = dst->width * dst->channels;
	int channels_dst = dst->channels;

	int width = src->width;
	int height = src->height;
	int x, y, xk, yk;
	long int pos_src, pos_dst;
	float src_val, dst_val;

	// Verificação de erros
	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL))
		return 0;
	if ((src->width != dst->width) || (src->height != dst->height))
		return 0;
	if ((src->channels != 1) || (dst->channels != 1))
		return 0;

	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			pos_dst = y * bytesperline_dst + x * channels_dst;
			pos_src = y * bytesperline_src + x * channels_src;
			src_val = (float)datasrc[pos_src];
			dst_val = (float)datadst[pos_dst];
			if (src_val == 255)
			{
				datadst[pos_dst] = 0;
			}
		}
	}
	return 1;
}

int vc_gray_erode(IVC *src, IVC *dst, int kernel_width)
{
	unsigned char *datasrc = (unsigned char *)src->data;
	int bytesperline_src = src->width * src->channels;
	int channels_src = src->channels;

	unsigned char *datadst = (unsigned char *)dst->data;
	int bytesperline_dst = dst->width * dst->channels;
	int channels_dst = dst->channels;

	int width = src->width;
	int height = src->height;
	int x, y, xk, yk;
	long int pos_src, pos_dst;
	float src_gray, offset, vmin;
	int hasBlack = 0;

	// Verificação de erros
	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL))
		return 0;
	if ((src->width != dst->width) || (src->height != dst->height))
		return 0;
	if ((src->channels != 1) || (dst->channels != 1))
		return 0;

	offset = (kernel_width - 1) / 2;

	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			pos_dst = y * bytesperline_dst + x * channels_dst;
			pos_src = y * bytesperline_src + x * channels_src;

			hasBlack = 0;
			vmin = (float)datasrc[pos_src];
			for (yk = -offset; yk < offset; yk++)
			{
				for (xk = -offset; xk < offset; xk++)
				{
					if (((y + yk) >= 0 && (y + yk) < src->height) && ((x + xk) >= 0 && (x + xk) < src->width))
					{
						pos_src = (y + yk) * bytesperline_src + (x + xk) * channels_src;
						src_gray = (float)datasrc[pos_src];

						if (src_gray < vmin)
						{
							vmin = src_gray;
						}
					}
				}
			}

			datadst[pos_dst] = vmin;
		}
	}
	return 1;
}

int vc_gray_dilate(IVC *src, IVC *dst, int kernel_width)
{
	unsigned char *datasrc = (unsigned char *)src->data;
	int bytesperline_src = src->width * src->channels;
	int channels_src = src->channels;

	unsigned char *datadst = (unsigned char *)dst->data;
	int bytesperline_dst = dst->width * dst->channels;
	int channels_dst = dst->channels;

	int width = src->width;
	int height = src->height;
	int x, y, xk, yk;
	long int pos_src, pos_dst;
	float src_gray, offset, vmax;
	int hasBlack = 0;

	// Verificação de erros
	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL))
		return 0;
	if ((src->width != dst->width) || (src->height != dst->height))
		return 0;
	if ((src->channels != 1) || (dst->channels != 1))
		return 0;

	offset = (kernel_width - 1) / 2;

	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			pos_dst = y * bytesperline_dst + x * channels_dst;
			pos_src = y * bytesperline_src + x * channels_src;

			hasBlack = 0;
			vmax = (float)datasrc[pos_src];
			for (yk = -offset; yk < offset; yk++)
			{
				for (xk = -offset; xk < offset; xk++)
				{
					if (((y + yk) >= 0 && (y + yk) < src->height) && ((x + xk) >= 0 && (x + xk) < src->width))
					{
						pos_src = (y + yk) * bytesperline_src + (x + xk) * channels_src;
						src_gray = (float)datasrc[pos_src];

						if (src_gray > vmax)
						{
							vmax = src_gray;
						}
					}
				}
			}

			datadst[pos_dst] = vmax;
		}
	}
	return 1;
}

int vc_binary_erode(IVC *src, IVC *dst, int kernel_width)
{
	unsigned char *datasrc = (unsigned char *)src->data;
	int bytesperline_src = src->width * src->channels;
	int channels_src = src->channels;

	unsigned char *datadst = (unsigned char *)dst->data;
	int bytesperline_dst = dst->width * dst->channels;
	int channels_dst = dst->channels;

	int width = src->width;
	int height = src->height;
	int x, y, xk, yk;
	long int pos_src, pos_dst;
	float src_gray, offset;
	int hasBlack = 0;

	// Verificação de erros
	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL))
		return 0;
	if ((src->width != dst->width) || (src->height != dst->height))
		return 0;
	if ((src->channels != 1) || (dst->channels != 1))
		return 0;

	offset = (kernel_width - 1) / 2;

	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			pos_dst = y * bytesperline_dst + x * channels_dst;
			pos_src = y * bytesperline_src + x * channels_src;

			hasBlack = 0;
			for (yk = -offset; yk < offset; yk++)
			{
				for (xk = -offset; xk < offset; xk++)
				{
					if (((y + yk) >= 0 && (y + yk) < src->height) && ((x + xk) >= 0 && (x + xk) < src->width))
					{
						pos_src = (y + yk) * bytesperline_src + (x + xk) * channels_src;
						src_gray = (float)datasrc[pos_src];

						if (src_gray == 0)
						{

							hasBlack = 1;
						}
					}
				}
			}

			if (hasBlack == 1)
			{
				datadst[pos_dst] = 0;
			}
			else
			{
				datadst[pos_dst] = 255;
			}
		}
	}
	return 1;
}

int vc_binary_open(IVC *src, IVC *dst, int kernel_erode, int kernel_dilate)
{
	IVC *tmp_image = vc_image_new(src->width, src->height, 1, 255);
	vc_binary_erode(src, tmp_image, kernel_erode);
	vc_binary_dilate(tmp_image, dst, kernel_dilate);
	vc_image_free(tmp_image);
	return 1;
}

int vc_binary_close(IVC *src, IVC *dst, int kernel_erode, int kernel_dilate)
{
	IVC *tmp_image = vc_image_new(src->width, src->height, 1, 255);
	vc_binary_dilate(src, tmp_image, kernel_dilate);
	vc_binary_erode(tmp_image, dst, kernel_erode);
	vc_image_free(tmp_image);
	return 1;
}
int vc_gray_open(IVC *src, IVC *dst, int kernel_erode, int kernel_dilate)
{
	IVC *tmp_image = vc_image_new(src->width, src->height, 1, 255);
	vc_gray_erode(src, tmp_image, kernel_erode);
	vc_gray_dilate(tmp_image, dst, kernel_dilate);
	vc_image_free(tmp_image);
	return 1;
}

int vc_gray_close(IVC *src, IVC *dst, int kernel_erode, int kernel_dilate)
{
	IVC *tmp_image = vc_image_new(src->width, src->height, 1, 255);
	vc_gray_dilate(src, tmp_image, kernel_erode);
	vc_gray_erode(tmp_image, dst, kernel_dilate);
	vc_image_free(tmp_image);
	return 1;
}

int vc_rgb_to_gray(IVC *src, IVC *dst)
{
	unsigned char *datasrc = (unsigned char *)src->data;
	int bytesperline_src = src->width * src->channels;
	int channels_src = src->channels;
	unsigned char *datadst = (unsigned char *)dst->data;
	int bytesperline_dst = dst->width * dst->channels;
	int channels_dst = dst->channels;
	int width = src->width;
	int height = src->height;
	int x, y;
	long int pos_src, pos_dst;
	float rf, gf, bf;
	// Verificação de erros
	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL))
		return 0;
	if ((src->width != dst->width) || (src->height != dst->height))
		return 0;
	if ((src->channels != 3) || (dst->channels != 1))
		return 0;
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			pos_src = y * bytesperline_src + x * channels_src;
			pos_dst = y * bytesperline_dst + x * channels_dst;
			rf = (float)datasrc[pos_src];
			gf = (float)datasrc[pos_src + 1];
			bf = (float)datasrc[pos_src + 2];
			datadst[pos_dst] = (unsigned char)((rf * 0.299) + (gf * 0.587) + (bf * 0.114));
		}
	}
	return 1;
}

int vc_bgr_to_rgb(IVC *src)
{
	unsigned char *datasrc = (unsigned char *)src->data;
	int bytesperline_src = src->width * src->channels;
	int channels_src = src->channels;
	int width = src->width;
	int height = src->height;
	int x, y;
	long int pos;
	float rf, gf, bf;
	// Verificação de erros
	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL))
		return 0;
	if ((src->channels != 3))
		return 0;
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			pos = y * bytesperline_src + x * channels_src;
			float temp = datasrc[pos + 2];
			datasrc[pos + 2] = datasrc[pos];
			datasrc[pos] = temp;
		}
	}
	return 1;
}

// Gerar negativo de imagem RGB
int vc_rgb_negative(IVC *srcdst)
{
	unsigned char *data = (unsigned char *)srcdst->data;
	int width = srcdst->width;
	int height = srcdst->height;
	int bytesperline = srcdst->width * srcdst->channels;
	int channels = srcdst->channels;
	int x, y;
	long int pos;

	// verificação de erros
	if ((srcdst->width <= 0) || (srcdst->height <= 0) || (srcdst->data == NULL))
		return 0;
	if (channels != 3)
		return 0;

	// inverter imagem RGB
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			pos = y * bytesperline + x * channels;
			data[pos] = 255 - data[pos];
			data[pos + 1] = 255 - data[pos + 1];
			data[pos + 2] = 255 - data[pos + 2];
		}
	}

	return 1;
}

float min_array(float min_array[], int size)
{
	float min = 255;
	for (int i = 0; i < size; i++)
	{
		if (min_array[i] < min && min_array[i] != 0)
		{
			min = min_array[i];
		}
	}
	return min;
}

int vc_binary_label(IVC *src, IVC *dst)
{
	unsigned char *datasrc = (unsigned char *)src->data;
	int bytesperline_src = src->width * src->channels;
	int channels_src = src->channels;

	unsigned char *datadst = (unsigned char *)dst->data;
	int bytesperline_dst = dst->width * dst->channels;
	int channels_dst = dst->channels;

	int width = src->width;
	int height = src->height;
	int x, y;
	float dst_val;
	float label = 1;
	float neighbors[4];
	long int pos_dst, pos_A, pos_B, pos_C, pos_D;
	memcpy(dst->data, src->data, src->width * src->height * sizeof(unsigned char));
	// Verificação de erros
	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL))
		return 0;
	if ((src->width != dst->width) || (src->height != dst->height))
		return 0;
	if ((src->channels != 1) || (dst->channels != 1))
		return 0;
	for (y = 1; y < height; y++)
	{
		for (x = 1; x < width; x++)
		{
			pos_A = (y - 1) * bytesperline_dst + (x - 1) * channels_dst;
			pos_B = (y - 1) * bytesperline_dst + x * channels_dst;
			pos_C = (y - 1) * bytesperline_dst + (x + 1) * channels_dst;
			pos_D = y * bytesperline_dst + (x - 1) * channels_dst;

			pos_dst = y * bytesperline_dst + x * channels_dst;

			dst_val = (float)datadst[pos_dst];

			neighbors[0] = (float)datadst[pos_A];
			neighbors[1] = (float)datadst[pos_B];
			neighbors[2] = (float)datadst[pos_C];
			neighbors[3] = (float)datadst[pos_D];

			if (dst_val == 255)
			{
				if (neighbors[0] == 0 && neighbors[1] == 0 && neighbors[2] == 0 && neighbors[3] == 0)
				{
					datadst[pos_dst] = label;
					label++;
				}
				else
				{
					// printf("%f", min_array(neighbors, 4));
					datadst[pos_dst] = min_array(neighbors, 4);
				}
			}
			else
			{
				datadst[pos_dst] = 0;
			}
		}
	}
	return 1;
}

// Etiquetagem de blobs
// src		: Imagem bin�ria de entrada
// dst		: Imagem grayscale (ir� conter as etiquetas)
// nlabels	: Endere�o de mem�ria de uma vari�vel, onde ser� armazenado o n�mero de etiquetas encontradas.
// OVC*		: Retorna um array de estruturas de blobs (objectos), com respectivas etiquetas. � necess�rio libertar posteriormente esta mem�ria.
OVC *vc_binary_blob_labelling(IVC *src, IVC *dst, int *nlabels)
{
	unsigned char *datasrc = (unsigned char *)src->data;
	unsigned char *datadst = (unsigned char *)dst->data;
	int width = src->width;
	int height = src->height;
	int bytesperline = src->bytesperline;
	int channels = src->channels;
	int x, y, a, b;
	long int i, size;
	long int posX, posA, posB, posC, posD;
	int labeltable[256] = {0};
	int labelarea[256] = {0};
	int label = 1; // Etiqueta inicial.
	int num, tmplabel;
	OVC *blobs; // Apontador para array de blobs (objectos) que ser� retornado desta fun��o.

	// Verifica��o de erros
	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL))
		return 0;
	if ((src->width != dst->width) || (src->height != dst->height) || (src->channels != dst->channels))
		return NULL;
	if (channels != 1)
		return NULL;

	// Copia dados da imagem bin�ria para imagem grayscale
	memcpy(datadst, datasrc, bytesperline * height);

	// Todos os pix�is de plano de fundo devem obrigat�riamente ter valor 0
	// Todos os pix�is de primeiro plano devem obrigat�riamente ter valor 255
	// Ser�o atribu�das etiquetas no intervalo [1,254]
	// Este algoritmo est� assim limitado a 254 labels
	for (i = 0, size = bytesperline * height; i < size; i++)
	{
		if (datadst[i] != 0)
			datadst[i] = 255;
	}

	// Limpa os rebordos da imagem bin�ria
	for (y = 0; y < height; y++)
	{
		datadst[y * bytesperline + 0 * channels] = 0;
		datadst[y * bytesperline + (width - 1) * channels] = 0;
	}
	for (x = 0; x < width; x++)
	{
		datadst[0 * bytesperline + x * channels] = 0;
		datadst[(height - 1) * bytesperline + x * channels] = 0;
	}

	// Efectua a etiquetagem
	for (y = 1; y < height - 1; y++)
	{
		for (x = 1; x < width - 1; x++)
		{
			// Kernel:
			// A B C
			// D X

			posA = (y - 1) * bytesperline + (x - 1) * channels; // A
			posB = (y - 1) * bytesperline + x * channels;		// B
			posC = (y - 1) * bytesperline + (x + 1) * channels; // C
			posD = y * bytesperline + (x - 1) * channels;		// D
			posX = y * bytesperline + x * channels;				// X

			// Se o pixel foi marcado
			if (datadst[posX] != 0)
			{
				if ((datadst[posA] == 0) && (datadst[posB] == 0) && (datadst[posC] == 0) && (datadst[posD] == 0))
				{
					datadst[posX] = label;
					labeltable[label] = label;
					label++;
				}
				else
				{
					num = 255;

					// Se A est� marcado
					if (datadst[posA] != 0)
						num = labeltable[datadst[posA]];
					// Se B est� marcado, e � menor que a etiqueta "num"
					if ((datadst[posB] != 0) && (labeltable[datadst[posB]] < num))
						num = labeltable[datadst[posB]];
					// Se C est� marcado, e � menor que a etiqueta "num"
					if ((datadst[posC] != 0) && (labeltable[datadst[posC]] < num))
						num = labeltable[datadst[posC]];
					// Se D est� marcado, e � menor que a etiqueta "num"
					if ((datadst[posD] != 0) && (labeltable[datadst[posD]] < num))
						num = labeltable[datadst[posD]];

					// Atribui a etiqueta ao pixel
					datadst[posX] = num;
					labeltable[num] = num;

					// Actualiza a tabela de etiquetas
					if (datadst[posA] != 0)
					{
						if (labeltable[datadst[posA]] != num)
						{
							for (tmplabel = labeltable[datadst[posA]], a = 1; a < label; a++)
							{
								if (labeltable[a] == tmplabel)
								{
									labeltable[a] = num;
								}
							}
						}
					}
					if (datadst[posB] != 0)
					{
						if (labeltable[datadst[posB]] != num)
						{
							for (tmplabel = labeltable[datadst[posB]], a = 1; a < label; a++)
							{
								if (labeltable[a] == tmplabel)
								{
									labeltable[a] = num;
								}
							}
						}
					}
					if (datadst[posC] != 0)
					{
						if (labeltable[datadst[posC]] != num)
						{
							for (tmplabel = labeltable[datadst[posC]], a = 1; a < label; a++)
							{
								if (labeltable[a] == tmplabel)
								{
									labeltable[a] = num;
								}
							}
						}
					}
					if (datadst[posD] != 0)
					{
						if (labeltable[datadst[posD]] != num)
						{
							for (tmplabel = labeltable[datadst[posC]], a = 1; a < label; a++)
							{
								if (labeltable[a] == tmplabel)
								{
									labeltable[a] = num;
								}
							}
						}
					}
				}
			}
		}
	}

	// Volta a etiquetar a imagem
	for (y = 1; y < height - 1; y++)
	{
		for (x = 1; x < width - 1; x++)
		{
			posX = y * bytesperline + x * channels; // X

			if (datadst[posX] != 0)
			{
				datadst[posX] = labeltable[datadst[posX]];
			}
		}
	}

	// printf("\nMax Label = %d\n", label);

	// Contagem do n�mero de blobs
	// Passo 1: Eliminar, da tabela, etiquetas repetidas
	for (a = 1; a < label - 1; a++)
	{
		for (b = a + 1; b < label; b++)
		{
			if (labeltable[a] == labeltable[b])
				labeltable[b] = 0;
		}
	}
	// Passo 2: Conta etiquetas e organiza a tabela de etiquetas, para que n�o hajam valores vazios (zero) entre etiquetas
	*nlabels = 0;
	for (a = 1; a < label; a++)
	{
		if (labeltable[a] != 0)
		{
			labeltable[*nlabels] = labeltable[a]; // Organiza tabela de etiquetas
			(*nlabels)++;						  // Conta etiquetas
		}
	}

	// Se n�o h� blobs
	if (*nlabels == 0)
		return NULL;

	// Cria lista de blobs (objectos) e preenche a etiqueta
	blobs = (OVC *)calloc((*nlabels), sizeof(OVC));
	if (blobs != NULL)
	{
		for (a = 0; a < (*nlabels); a++)
			blobs[a].label = labeltable[a];
	}
	else
		return NULL;

	return blobs;
}

int vc_binary_blob_info(IVC *src, OVC *blobs, int nblobs)
{
	unsigned char *data = (unsigned char *)src->data;
	int width = src->width;
	int height = src->height;
	int bytesperline = src->bytesperline;
	int channels = src->channels;
	int x, y, i;
	long int pos;
	int xmin, ymin, xmax, ymax;
	long int sumx, sumy;

	// Verifica��o de erros
	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL))
		return 0;
	if (channels != 1)
		return 0;

	// Conta �rea de cada blob
	for (i = 0; i < nblobs; i++)
	{
		xmin = width - 1;
		ymin = height - 1;
		xmax = 0;
		ymax = 0;

		sumx = 0;
		sumy = 0;

		blobs[i].area = 0;

		for (y = 1; y < height - 1; y++)
		{
			for (x = 1; x < width - 1; x++)
			{
				pos = y * bytesperline + x * channels;

				if (data[pos] == blobs[i].label)
				{
					// �rea
					blobs[i].area++;

					// Centro de Gravidade
					sumx += x;
					sumy += y;

					// Bounding Box
					if (xmin > x)
						xmin = x;
					if (ymin > y)
						ymin = y;
					if (xmax < x)
						xmax = x;
					if (ymax < y)
						ymax = y;

					// Per�metro
					// Se pelo menos um dos quatro vizinhos n�o pertence ao mesmo label, ent�o � um pixel de contorno
					if ((data[pos - 1] != blobs[i].label) || (data[pos + 1] != blobs[i].label) || (data[pos - bytesperline] != blobs[i].label) || (data[pos + bytesperline] != blobs[i].label))
					{
						blobs[i].perimeter++;
					}
				}
			}
		}

		// Bounding Box
		blobs[i].x = xmin;
		blobs[i].y = ymin;
		blobs[i].width = (xmax - xmin) + 1;
		blobs[i].height = (ymax - ymin) + 1;

		// Centro de Gravidade
		// blobs[i].xc = (xmax - xmin) / 2;
		// blobs[i].yc = (ymax - ymin) / 2;
		blobs[i].xc = sumx / MAX(blobs[i].area, 1);
		blobs[i].yc = sumy / MAX(blobs[i].area, 1);
	}

	return 1;
}

// int vc_rgb_to_binary(IVC *src, IVC *dst, int threshold)
// {
// 	IVC *gray_image;
// 	vc_rgb_to_gray(src, gray_image);
// 	vc_gray_to_binary(gray_image, dst, threshold);
// 	return 1;
// }

char *netpbm_get_token(FILE *file, char *tok, int len)
{
	char *t;
	int c;

	for (;;)
	{
		while (isspace(c = getc(file)))
			;
		if (c != '#')
			break;
		do
			c = getc(file);
		while ((c != '\n') && (c != EOF));
		if (c == EOF)
			break;
	}

	t = tok;

	if (c != EOF)
	{
		do
		{
			*t++ = c;
			c = getc(file);
		} while ((!isspace(c)) && (c != '#') && (c != EOF) && (t - tok < len - 1));

		if (c == '#')
			ungetc(c, file);
	}

	*t = 0;

	return tok;
}

long int unsigned_char_to_bit(unsigned char *datauchar, unsigned char *databit, int width, int height)
{
	int x, y;
	int countbits;
	long int pos, counttotalbytes;
	unsigned char *p = databit;

	*p = 0;
	countbits = 1;
	counttotalbytes = 0;

	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			pos = width * y + x;

			if (countbits <= 8)
			{
				// Numa imagem PBM:
				// 1 = Preto
				// 0 = Branco
				//*p |= (datauchar[pos] != 0) << (8 - countbits);

				// Na nossa imagem:
				// 1 = Branco
				// 0 = Preto
				*p |= (datauchar[pos] == 0) << (8 - countbits);

				countbits++;
			}
			if ((countbits > 8) || (x == width - 1))
			{
				p++;
				*p = 0;
				countbits = 1;
				counttotalbytes++;
			}
		}
	}

	return counttotalbytes;
}

void bit_to_unsigned_char(unsigned char *databit, unsigned char *datauchar, int width, int height)
{
	int x, y;
	int countbits;
	long int pos;
	unsigned char *p = databit;

	countbits = 1;

	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			pos = width * y + x;

			if (countbits <= 8)
			{
				// Numa imagem PBM:
				// 1 = Preto
				// 0 = Branco
				// datauchar[pos] = (*p & (1 << (8 - countbits))) ? 1 : 0;

				// Na nossa imagem:
				// 1 = Branco
				// 0 = Preto
				datauchar[pos] = (*p & (1 << (8 - countbits))) ? 0 : 1;

				countbits++;
			}
			if ((countbits > 8) || (x == width - 1))
			{
				p++;
				countbits = 1;
			}
		}
	}
}

int vc_gray_histogram_show(IVC *src, IVC *dst)
{
	unsigned char *datasrc = (unsigned char *)src->data;
	unsigned char *datadst = (unsigned char *)dst->data;
	int width = src->width;
	int height = src->height;
	int bytesperline = src->bytesperline;
	int channels = src->channels;
	int x, y, i;
	long int pos;
	int ni[256] = {0};
	float pdf[256];
	int n = width * height;
	float pdfmax = 0;
	float pdfnorm[256];

	// Verificação de erros
	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL))
		return 0;
	if (channels != 1)
		return 0;

	for (int i = 0; i < width * height; ni[datasrc[i++]]++)
		;

	for (int i = 0; i < 256; i++)
	{
		pdf[i] = (float)ni[i] / (float)n;
	}

	for (int i = 0; i < 256; i++)
	{
		if (pdf[i] > pdfmax)
		{
			pdfmax = pdf[i];
		}
	}

	for (int i = 0; i < 256; i++)
	{
		pdfnorm[i] = pdf[i] / pdfmax;
	}

	for (int i = 0; i < 256 * 256; i++)
		datadst[i] = 0;

	for (int x = 0; x < 256; x++)
	{
		for (y = (256 - 1); y >= (256 - 1) - pdfnorm[x] * 255; y--)
		{
			datadst[y * 256 + x] = 255;
		}
	}
}

int vc_gray_histogram_equalization(IVC *src, IVC *dst)
{
	unsigned char *datasrc = (unsigned char *)src->data;
	unsigned char *datadst = (unsigned char *)dst->data;
	int width = src->width;
	int height = src->height;
	int bytesperline = src->bytesperline;
	int channels = src->channels;
	float pdf[256];
	float cdf[256];
	long int pos;
	int ni[256] = {0};
	int n = width * height;
	float cdfmin = -1;
	int brilho, value;

	// calculo do pdf
	for (int i = 0; i < width * height; ni[datasrc[i++]]++)
		;

	for (int i = 0; i < 256; i++)
	{
		pdf[i] = (float)ni[i] / (float)n;
	}

	// calculo do cdf e cdfmin
	for (int i = 0; i < 256; i++)
	{
		if (i == 0)
		{
			cdf[i] = pdf[i];
		}
		else
		{
			cdf[i] = cdf[i - 1] + pdf[i];
		}

		if (cdf[i] != 0 && cdfmin < 0)
		{
			cdfmin = cdf[i];
		}
	}
	// percorrer imagem
	for (int y = 1; y < height - 1; y++)
	{
		for (int x = 1; x < width - 1; x++)
		{
			pos = y * bytesperline + x * channels;
			brilho = datasrc[pos];

			value = (cdf[brilho] - cdfmin) / (1 - cdfmin) * (256 - 1);
			datadst[pos] = value;
		}
	}
}

int vc_gray_edge_prewitt(IVC *src, IVC *dst, float th)
{
	unsigned char *datasrc = (unsigned char *)src->data;
	unsigned char *datadst = (unsigned char *)dst->data;
	int width = src->width;
	int height = src->height;
	int bytesperline = src->bytesperline;
	int channels = src->channels;
	long int pos;
	int brilho;
	int sum;
	int mx, my;

	for (int y = 1; y < height - 1; y++)
	{
		for (int x = 1; x < width - 1; x++)
		{
			pos = y * bytesperline + x * channels;
			brilho = datasrc[pos];
		}
	}
}

int vc_gray_lowpass_mean_filter(IVC *src, IVC *dst, int kernelsize)
{
	unsigned char *datasrc = (unsigned char *)src->data;
	unsigned char *datadst = (unsigned char *)dst->data;
	int width = src->width;
	int height = src->height;
	int bytesperline = src->bytesperline;
	int channels = src->channels;
	long int pos, posX;
	int sum;
	float brilho;
	int offset = (kernelsize - 1) / 2;

	for (int y = 1; y < height - 1; y++)
	{
		for (int x = 1; x < width - 1; x++)
		{
			sum = 0;
			posX = y * bytesperline + x * channels;
			for (int yk = -offset; yk <= offset; yk++)
			{
				for (int xk = -offset; xk <= offset; xk++)
				{
					if (((y + yk) >= 0 && (y + yk) < src->height) && ((x + xk) >= 0 && (x + xk) < src->width))
					{
						pos = (y + yk) * bytesperline + (x + xk) * channels;
						sum += datasrc[pos];
					}
				}
			}

			datadst[posX] = sum / (kernelsize * kernelsize);
		}
	}
}

// order array acending
int *sort_array(int *array, int size)
{
	int i, j, temp;
	for (i = 0; i < size; i++)
	{
		for (j = 0; j < size - 1; j++)
		{
			if (array[j] > array[j + 1])
			{
				temp = array[j];
				array[j] = array[j + 1];
				array[j + 1] = temp;
			}
		}
	}
	return array;
}
//
// int vc_gray_lowpass_median_filter(IVC* src, IVC* dst, int kernelsize)
//{
//	unsigned char* datasrc = (unsigned char*)src->data;
//	unsigned char* datadst = (unsigned char*)dst->data;
//	int width = src->width;
//	int height = src->height;
//	int bytesperline = src->bytesperline;
//	int channels = src->channels;
//	long int pos, posX;
//	int sum;
//
//	int values[kernelsize * kernelsize];
//	int valuesOrdered[kernelsize * kernelsize];
//	int count;
//	float brilho;
//	int offset = (kernelsize - 1) / 2;
//
//	for (int y = 1; y < height - 1; y++)
//	{
//		for (int x = 1; x < width - 1; x++)
//		{
//			sum = 0;
//			count = 0;
//			posX = y * bytesperline + x * channels;
//			for (int yk = -offset; yk <= offset; yk++)
//			{
//				for (int xk = -offset; xk <= offset; xk++)
//				{
//					if (((y + yk) >= 0 && (y + yk) < src->height) && ((x + xk) >= 0 && (x + xk) < src->width))
//					{
//						pos = (y + yk) * bytesperline + (x + xk) * channels;
//						values[count] = datasrc[pos];
//						count++;
//					}
//				}
//			}
//			int* valuesOrdered = sort_array(values, kernelsize * kernelsize);
//			datadst[posX] = valuesOrdered[(kernelsize * kernelsize) / 2];
//		}
//	}
//}

int vc_gray_lowpass_gaussian_filter(IVC *src, IVC *dst)
{
	unsigned char *datasrc = (unsigned char *)src->data;
	unsigned char *datadst = (unsigned char *)dst->data;
	int width = src->width;
	int height = src->height;
	int bytesperline = src->bytesperline;
	int channels = src->channels;
	long int posX, pos;
	int value, sum;
	int matriz[5][5] = {
		{1, 4, 7, 4, 1},
		{4, 16, 26, 16, 4},
		{7, 26, 41, 26, 7},
		{4, 16, 26, 16, 4},
		{1, 4, 7, 4, 1}};

	int offset = 2;

	for (int y = 1; y < height - 1; y++)
	{
		for (int x = 1; x < width - 1; x++)
		{
			sum = 0;
			posX = y * bytesperline + x * channels;
			for (int yk = -offset; yk <= offset; yk++)
			{
				for (int xk = -offset; xk <= offset; xk++)
				{
					if (((y + yk) >= 0 && (y + yk) < src->height) && ((x + xk) >= 0 && (x + xk) < src->width))
					{
						pos = (y + yk) * bytesperline + (x + xk) * channels;
						sum += datasrc[pos] * matriz[(2 + yk)][(2 + xk)];
					}
				}
			}
			value = sum / 273;
			datadst[posX] = (int)value;
		}
	}
}

int vc_gray_highpass_filter(IVC *src, IVC *dst)
{
	unsigned char *datasrc = (unsigned char *)src->data;
	unsigned char *datadst = (unsigned char *)dst->data;
	int width = src->width;
	int height = src->height;
	int bytesperline = src->bytesperline;
	int channels = src->channels;
	long int posX, pos;
	int value, sum;
	int matriz[3][3] = {
		{-1, -1, -1},
		{-1, 8, -1},
		{-1, -1, -1}};

	int offset = 1;

	for (int y = 1; y < height - 1; y++)
	{
		for (int x = 1; x < width - 1; x++)
		{
			sum = 0;
			posX = y * bytesperline + x * channels;
			for (int yk = -offset; yk <= offset; yk++)
			{
				for (int xk = -offset; xk <= offset; xk++)
				{
					if (((y + yk) >= 0 && (y + yk) < src->height) && ((x + xk) >= 0 && (x + xk) < src->width))
					{
						pos = (y + yk) * bytesperline + (x + xk) * channels;
						sum += datasrc[pos] * matriz[(1 + yk)][(1 + xk)];
					}
				}
			}
			value = abs(sum) / 9;
			datadst[posX] = (int)value;
		}
	}
}

int vc_gray_highpass_filter_enhance(IVC *src, IVC *dst, int gain)
{
	unsigned char *datasrc = (unsigned char *)src->data;
	unsigned char *datadst = (unsigned char *)dst->data;
	int width = src->width;
	int height = src->height;
	int bytesperline = src->bytesperline;
	int channels = src->channels;
	long int posX, pos;
	int value, sum;
	int matriz[3][3] = {
		{-1, -1, -1},
		{-1, 8, -1},
		{-1, -1, -1}};

	int offset = 1;

	for (int y = 1; y < height - 1; y++)
	{
		for (int x = 1; x < width - 1; x++)
		{
			sum = 0;
			posX = y * bytesperline + x * channels;
			for (int yk = -offset; yk <= offset; yk++)
			{
				for (int xk = -offset; xk <= offset; xk++)
				{
					if (((y + yk) >= 0 && (y + yk) < src->height) && ((x + xk) >= 0 && (x + xk) < src->width))
					{
						pos = (y + yk) * bytesperline + (x + xk) * channels;
						sum += datasrc[pos] * matriz[(1 + yk)][(1 + xk)];
					}
				}
			}
			value = ((int)(sum / 9)) * gain;
			if ((datasrc[posX] + value) < 0)
			{
				datadst[posX] = 0;
			}
			else if ((datasrc[posX] + value) > 255)
			{
				datadst[posX] = 255;
			}
			else
			{
				datadst[posX] = datasrc[posX] + value;
			}
		}
	}
}

IVC *vc_read_image(char *filename)
{
	FILE *file = NULL;
	IVC *image = NULL;
	unsigned char *tmp;
	char tok[20];
	long int size, sizeofbinarydata;
	int width, height, channels;
	int levels = 255;
	int v;

	// Abre o ficheiro
	if ((file = fopen(filename, "rb")) != NULL)
	{
		// Efectua a leitura do header
		netpbm_get_token(file, tok, sizeof(tok));

		if (strcmp(tok, "P4") == 0)
		{
			channels = 1;
			levels = 1;
		} // Se PBM (Binary [0,1])
		else if (strcmp(tok, "P5") == 0)
			channels = 1; // Se PGM (Gray [0,MAX(level,255)])
		else if (strcmp(tok, "P6") == 0)
			channels = 3; // Se PPM (RGB [0,MAX(level,255)])
		else
		{
#ifdef VC_DEBUG
			printf("ERROR -> vc_read_image():\n\tFile is not a valid PBM, PGM or PPM file.\n\tBad magic number!\n");
#endif

			fclose(file);
			return NULL;
		}

		if (levels == 1) // PBM
		{
			if (sscanf(netpbm_get_token(file, tok, sizeof(tok)), "%d", &width) != 1 ||
				sscanf(netpbm_get_token(file, tok, sizeof(tok)), "%d", &height) != 1)
			{
#ifdef VC_DEBUG
				printf("ERROR -> vc_read_image():\n\tFile is not a valid PBM file.\n\tBad size!\n");
#endif

				fclose(file);
				return NULL;
			}

			// Aloca mem�ria para imagem
			image = vc_image_new(width, height, channels, levels);
			if (image == NULL)
				return NULL;

			sizeofbinarydata = (image->width / 8 + ((image->width % 8) ? 1 : 0)) * image->height;
			tmp = (unsigned char *)malloc(sizeofbinarydata);
			if (tmp == NULL)
				return 0;

#ifdef VC_DEBUG
			printf("\nchannels=%d w=%d h=%d levels=%d\n", image->channels, image->width, image->height, levels);
#endif

			if ((v = fread(tmp, sizeof(unsigned char), sizeofbinarydata, file)) != sizeofbinarydata)
			{
#ifdef VC_DEBUG
				printf("ERROR -> vc_read_image():\n\tPremature EOF on file.\n");
#endif

				vc_image_free(image);
				fclose(file);
				free(tmp);
				return NULL;
			}

			bit_to_unsigned_char(tmp, image->data, image->width, image->height);

			free(tmp);
		}
		else // PGM ou PPM
		{
			if (sscanf(netpbm_get_token(file, tok, sizeof(tok)), "%d", &width) != 1 ||
				sscanf(netpbm_get_token(file, tok, sizeof(tok)), "%d", &height) != 1 ||
				sscanf(netpbm_get_token(file, tok, sizeof(tok)), "%d", &levels) != 1 || levels <= 0 || levels > 255)
			{
#ifdef VC_DEBUG
				printf("ERROR -> vc_read_image():\n\tFile is not a valid PGM or PPM file.\n\tBad size!\n");
#endif

				fclose(file);
				return NULL;
			}

			// Aloca mem�ria para imagem
			image = vc_image_new(width, height, channels, levels);
			if (image == NULL)
				return NULL;

#ifdef VC_DEBUG
			printf("\nchannels=%d w=%d h=%d levels=%d\n", image->channels, image->width, image->height, levels);
#endif

			size = image->width * image->height * image->channels;

			if ((v = fread(image->data, sizeof(unsigned char), size, file)) != size)
			{
#ifdef VC_DEBUG
				printf("ERROR -> vc_read_image():\n\tPremature EOF on file.\n");
#endif

				vc_image_free(image);
				fclose(file);
				return NULL;
			}
		}

		fclose(file);
	}
	else
	{
#ifdef VC_DEBUG
		printf("ERROR -> vc_read_image():\n\tFile not found.\n");
#endif
	}

	return image;
}

int vc_write_image(char *filename, IVC *image)
{
	FILE *file = NULL;
	unsigned char *tmp;
	long int totalbytes, sizeofbinarydata;

	if (image == NULL)
		return 0;

	if ((file = fopen(filename, "wb")) != NULL)
	{
		if (image->levels == 1)
		{
			sizeofbinarydata = (image->width / 8 + ((image->width % 8) ? 1 : 0)) * image->height + 1;
			tmp = (unsigned char *)malloc(sizeofbinarydata);
			if (tmp == NULL)
				return 0;

			fprintf(file, "%s %d %d\n", "P4", image->width, image->height);

			totalbytes = unsigned_char_to_bit(image->data, tmp, image->width, image->height);
			printf("Total = %ld\n", totalbytes);
			if (fwrite(tmp, sizeof(unsigned char), totalbytes, file) != totalbytes)
			{
#ifdef VC_DEBUG
				fprintf(stderr, "ERROR -> vc_read_image():\n\tError writing PBM, PGM or PPM file.\n");
#endif

				fclose(file);
				free(tmp);
				return 0;
			}

			free(tmp);
		}
		else
		{
			fprintf(file, "%s %d %d 255\n", (image->channels == 1) ? "P5" : "P6", image->width, image->height);

			if (fwrite(image->data, image->bytesperline, image->height, file) != image->height)
			{
#ifdef VC_DEBUG
				fprintf(stderr, "ERROR -> vc_read_image():\n\tError writing PBM, PGM or PPM file.\n");
#endif

				fclose(file);
				return 0;
			}
		}

		fclose(file);

		return 1;
	}

	return 0;
}
