//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//           INSTITUTO POLITÉCNICO DO CÁVADO E DO AVE
//                          2011/2012
//             ENGENHARIA DE SISTEMAS INFORMÁTICOS
//                    VISÃO POR COMPUTADOR
//
//             [  DUARTE DUQUE - dduque@ipca.pt  ]
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#define VC_DEBUG

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                   ESTRUTURA DE UMA IMAGEM
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

typedef struct
{
	unsigned char *data;
	int width, height;
	int channels;	  // Binário/Cinzentos=1; RGB=3
	int levels;		  // Binário=1; Cinzentos [1,255]; RGB [1,255]
	int bytesperline; // width * channels
} IVC;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                   ESTRUTURA DE UM BLOB (OBJECTO)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

typedef struct
{
	int x, y, width, height; // Caixa Delimitadora (Bounding Box)
	int area;				 // �rea
	int xc, yc;				 // Centro-de-massa
	int perimeter;			 // Per�metro
	int label;				 // Etiqueta
} OVC;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                    PROTÓTIPOS DE FUNÇÕES
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#define MAX(a, b) (a > b ? a : b)
#define MIN(a, b) (a < b ? a : b)

// FUNÇÕES: ALOCAR E LIBERTAR UMA IMAGEM
IVC *vc_image_new(int width, int height, int channels, int levels);
IVC *vc_image_free(IVC *image);

// FUNÇÕES: LEITURA E ESCRITA DE IMAGENS (PBM, PGM E PPM)
IVC *vc_read_image(char *filename);
int vc_write_image(char *filename, IVC *image);

// FUNÇÕES: Inversão de cores
// int vc_rgb_to_binary(IVC *src, IVC *dst);
int vc_gray_negative(IVC *srcdst);
int vc_rgb_negative(IVC *srcdst);

int vc_rgb_get_red_gray(IVC *srcdst);
int vc_rgb_get_green_gray(IVC *srcdst);
int vc_rgb_get_blue_gray(IVC *srcdst);

int vc_rgb_to_gray(IVC *src, IVC *dst);

int vc_binary_subtract(IVC *src, IVC *dst);

// HSV
int vc_rgb_to_hsv(IVC *src, IVC *dst);
int vc_scale_gray_to_rgb(IVC *src, IVC *dst);

// segmentation
int vc_gray_to_binary(IVC *src, IVC *dst, int threshold);
int vc_hsv_segmentation(IVC *src, IVC *dst, int hmin, int hmax, int smin, int smax, int vmin, int vmax);
int vc_gray_to_binary_global_mean(IVC *src, IVC *dst);
int vc_gray_to_binary_midpoint(IVC *src, IVC *dst, int kernel_width);
int vc_gray_to_binary_bersen(IVC *src, IVC *dst, int kernel_width, int cmin);
int vc_gray_to_binary_niblack(IVC *src, IVC *dst, int kernel_width, float k);

// operadores morfologicos
int vc_binary_dilate(IVC *src, IVC *dst, int kernel);
int vc_binary_erode(IVC *src, IVC *dst, int kernel);

int vc_gray_dilate(IVC *src, IVC *dst, int kernel);
int vc_gray_erode(IVC *src, IVC *dst, int kernel);

int vc_binary_open(IVC *src, IVC *dst, int kernel_erode, int kernel_dilate);
int vc_binary_close(IVC *src, IVC *dst, int kernel_erode, int kernel_dilate);
int vc_gray_open(IVC *src, IVC *dst, int kernel_erode, int kernel_dilate);
int vc_gray_close(IVC *src, IVC *dst, int kernel_erode, int kernel_dilate);

int vc_binary_label(IVC *src, IVC *dst);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                    PROT�TIPOS DE FUN��ES
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

OVC *vc_binary_blob_labelling(IVC *src, IVC *dst, int *nlabels);
int vc_binary_blob_info(IVC *src, OVC *blobs, int nblobs);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//												Histograma
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int vc_gray_histogram_show(IVC *src, IVC *dst);
int vc_gray_histogram_equalization(IVC *src, IVC *dst);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//												Contornos
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int vc_gray_edge_prewitt(IVC *src, IVC *dst, float th);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//												Filtros
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int vc_gray_lowpass_mean_filter(IVC *src, IVC *dst, int kernelsize);
int vc_gray_lowpass_median_filter(IVC *src, IVC *dst, int kernelsize);
int vc_gray_lowpass_gaussian_filter(IVC *src, IVC *dst);
int vc_gray_highpass_filter(IVC *src, IVC *dst);
int vc_gray_highpass_filter_enhance(IVC *src, IVC *dst, int gain);

int vc_bgr_to_rgb(IVC *src);
double checkHVariance(IVC *src, IVC *segmented, int xi, int yi, int width, int height, int area);