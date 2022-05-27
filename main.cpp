#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>

extern "C"
{
#include "vc.h"
}

using namespace cv;
int main(void)
{
	// Vídeo
	char videofile[20] = "video.avi";
	cv::VideoCapture capture;
	struct
	{
		int width, height;
		int ntotalframes;
		int fps;
		int nframe;
	} video;
	// Outros
	std::string str;
	int key = 0;

	/* Leitura de vídeo de um ficheiro */
	/* NOTA IMPORTANTE:
	O ficheiro video.avi deverá estar localizado no mesmo directório que o ficheiro de código fonte.
	*/
	capture.open(videofile);

	/* Em alternativa, abrir captura de vídeo pela Webcam #0 */
	// capture.open(0, cv::CAP_DSHOW); // Pode-se utilizar apenas capture.open(0);

	/* Verifica se foi possível abrir o ficheiro de vídeo */
	if (!capture.isOpened())
	{
		std::cerr << "Erro ao abrir o ficheiro de vídeo!\n";
		return 1;
	}

	/* Número total de frames no vídeo */
	video.ntotalframes = (int)capture.get(cv::CAP_PROP_FRAME_COUNT);
	/* Frame rate do vídeo */
	video.fps = (int)capture.get(cv::CAP_PROP_FPS);
	/* Resolução do vídeo */
	video.width = (int)capture.get(cv::CAP_PROP_FRAME_WIDTH);
	video.height = (int)capture.get(cv::CAP_PROP_FRAME_HEIGHT);

	/* Cria uma janela para exibir o vídeo */
	cv::namedWindow("VC - VIDEO", cv::WINDOW_AUTOSIZE);

	cv::Mat frame;
	int i = 0;
	int nLaranjas;

	OVC *blobs;

	while (key != 'q')
	{
		/* Leitura de uma frame do vídeo */
		capture.read(frame);

		/* Verifica se conseguiu ler a frame */
		if (frame.empty())
			break;

		/* Número da frame a processar */
		video.nframe = (int)capture.get(cv::CAP_PROP_POS_FRAMES);

		/* Exemplo de inserção texto na frame */
		str = std::string("RESOLUCAO: ").append(std::to_string(video.width)).append("x").append(std::to_string(video.height));
		cv::putText(frame, str, cv::Point(20, 25), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 0), 2);
		cv::putText(frame, str, cv::Point(20, 25), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 1);
		str = std::string("TOTAL DE FRAMES: ").append(std::to_string(video.ntotalframes));
		cv::putText(frame, str, cv::Point(20, 50), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 0), 2);
		cv::putText(frame, str, cv::Point(20, 50), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 1);
		str = std::string("FRAME RATE: ").append(std::to_string(video.fps));
		cv::putText(frame, str, cv::Point(20, 75), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 0), 2);
		cv::putText(frame, str, cv::Point(20, 75), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 1);
		str = std::string("N. DA FRAME: ").append(std::to_string(video.nframe));
		cv::putText(frame, str, cv::Point(20, 100), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 0), 2);
		cv::putText(frame, str, cv::Point(20, 100), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 1);

		// Faça o seu código aqui...

		// Cria uma nova imagem IVC
		IVC *image = vc_image_new(video.width, video.height, 3, 255);
		IVC *image_rgb = vc_image_new(video.width, video.height, 3, 255);
		IVC *image_hsv = vc_image_new(video.width, video.height, 3, 255);
		IVC *segmentated = vc_image_new(video.width, video.height, 1, 255);
		IVC *eroded = vc_image_new(video.width, video.height, 1, 255);
		IVC *dilated = vc_image_new(video.width, video.height, 1, 255);
		IVC *binary = vc_image_new(video.width, video.height, 1, 255);

		IVC *labled_image = vc_image_new(video.width, video.height, 1, 255);

		// Copia dados de imagem da estrutura cv::Mat para uma estrutura IVC
		memcpy(image->data, frame.data, video.width * video.height * 3);
		// Executa uma função da nossa biblioteca vc
		vc_bgr_to_rgb(image);
		vc_rgb_to_hsv(image, image_hsv);

		vc_hsv_segmentation(image_hsv, segmentated, 10, 25, 40, 100, 40, 100);

		vc_gray_to_binary(segmentated, binary, 200);

		vc_binary_erode(binary, eroded, 5);

		if (i == 530 || i == 260 || i == 160 || i == 430 || i == 353)
		{
			char buffer[100];
			char buffer1[100];
			char buffer2[100];
			char buffer3[100];
			snprintf(buffer, 100, "./imgs/originals/%i.ppm", i);
			vc_write_image(buffer, image);
			snprintf(buffer1, 100, "./imgs/hsv/%i.ppm", i);
			vc_write_image(buffer1, image_hsv);
			snprintf(buffer2, 100, "./imgs/%i.ppm", i);
			snprintf(buffer3, 100, "./imgs/%i_erode.ppm", i);
			vc_write_image(buffer2, segmentated);
			vc_write_image(buffer3, eroded);
		}

		blobs = vc_binary_blob_labelling(eroded, labled_image, &nLaranjas);

		vc_binary_blob_info(labled_image, blobs, nLaranjas);

		// for (int i = 0; i < nLaranjas; i++)
		// {
		// 	printf("---------------------------------------------\n");
		// 	printf("Box Width: %d\n", blobs[i].width);
		// 	printf("Box height: %d\n", blobs[i].height);
		// 	printf("area: %d\n", blobs[i].area);
		// 	printf("xc: %d\n", blobs[i].xc);
		// 	printf("x: %d\n", blobs[i].x);
		// 	printf("yc: %d\n", blobs[i].yc);
		// 	printf("y: %d\n", blobs[i].y);
		// 	printf("Perimeter: %d\n", blobs[i].perimeter);
		// 	printf("Lable: %d\n", blobs[i].label);
		// 	printf("---------------------------------------------\n");
		// }"Box Width: %d\n,Box height: %d\narea: %d\nxc: %d\nx: %d\nyc: %d\ny: %d\nPerimeter: %d\nLable: %d\n"

		vc_bgr_to_rgb(image);

		// Copia dados de imagem da estrutura IVC para uma estrutura cv::Mat
		memcpy(frame.data, image->data, video.width * video.height * 3);

		for (int i = 0; i < nLaranjas; i++)
		{
			if (blobs[i].area > 250)
			{
				cv::Rect rect(blobs[i].x, blobs[i].y, blobs[i].width, blobs[i].height);
				rectangle(frame, rect, (255, 255, 255), 2, LINE_8);
				char buffer[100];
				snprintf(buffer, 100, "Box Width: %d\n,Box height: %d\narea: %d\nxc: %d\nx: %d\nyc: %d\ny: %d\nPerimeter: %d\nLabel: %d\n",
								 blobs[i].width, blobs[i].height, blobs[i].area, blobs[i].xc, blobs[i].x, blobs[i].yc, blobs[i].y, blobs[i].perimeter, blobs[i].label);

				putText(frame, buffer, Point(blobs[i].x, blobs[i].y - 10), 0, 0.7, (0, 255, 0));
			}
		}

		// Liberta a memória da imagem IVC que havia sido criada
		vc_image_free(image);

		// +++++++++++++++++++++++++

		/* Exibe a frame */
		cv::imshow("VC - VIDEO", frame);

		/* Sai da aplicação, se o utilizador premir a tecla 'q' */
		key = cv::waitKey(1);
		i++;
	}
	printf("%d\n", nLaranjas);

	for (int i = 0; i < nLaranjas; i++)
	{
		printf("---------------------------------------------\n");
		printf("Box Width: %d\n", blobs[i].width);
		printf("Box height: %d\n", blobs[i].height);
		printf("area: %d\n", blobs[i].area);
		printf("xc: %d\n", blobs[i].xc);
		printf("x: %d\n", blobs[i].x);
		printf("yc: %d\n", blobs[i].yc);
		printf("y: %d\n", blobs[i].y);
		printf("Perimeter: %d\n", blobs[i].perimeter);
		printf("Lable: %d\n", blobs[i].label);
		printf("---------------------------------------------\n");
	}
	/* Fecha a janela */
	cv::destroyWindow("VC - VIDEO");

	/* Fecha o ficheiro de vídeo */
	capture.release();

	return 0;
}