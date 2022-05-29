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

int getCalibre(int diametro)
{
	//! como fazer para calibre 0?
	if (diametro >= 100)
	{
		return 0;
	}
	else if (diametro >= 87 && diametro < 100)
	{
		return 1;
	}
	else if (diametro >= 84 && diametro < 96)
	{
		return 2;
	}
	else if (diametro >= 81 && diametro < 92)
	{
		return 3;
	}
	else if (diametro >= 77 && diametro < 88)
	{
		return 4;
	}
	else if (diametro >= 73 && diametro < 84)
	{
		return 5;
	}
	else if (diametro >= 70 && diametro < 80)
	{
		return 6;
	}
	else if (diametro >= 67 && diametro < 76)
	{
		return 7;
	}
	else if (diametro >= 64 && diametro < 73)
	{
		return 8;
	}
	else if (diametro >= 62 && diametro < 70)
	{
		return 9;
	}
	else if (diametro >= 60 && diametro < 68)
	{
		return 10;
	}
	else if (diametro >= 58 && diametro < 66)
	{
		return 11;
	}
	else if (diametro >= 56 && diametro < 63)
	{
		return 12;
	}
	else if (diametro >= 53 && diametro < 60)
	{
		return 13;
	}
	return -1;
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
	int nLaranjas = 0;
	std::map<int, OVC> laranjas;
	// std::map<int, OVC> laranjas;
	while (key != 'q')
	{
		int nBlobs;
		OVC *blobs;
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
		IVC *open = vc_image_new(video.width, video.height, 1, 255);

		IVC *labled_image = vc_image_new(video.width, video.height, 1, 255);

		// Copia dados de imagem da estrutura cv::Mat para uma estrutura IVC
		memcpy(image->data, frame.data, video.width * video.height * 3);
		// Executa uma função da nossa biblioteca vc
		vc_bgr_to_rgb(image);
		vc_rgb_to_hsv(image, image_hsv);

		// vc_hsv_segmentation(image_hsv, segmentated, 10, 30, 80, 100, 27, 80);
		vc_hsv_segmentation(image_hsv, segmentated, 10, 30, 75, 100, 25, 80);

		vc_binary_open(segmentated, open, 5, 5);
		// vc_binary_erode(segmentated, eroded, 7);
		blobs = vc_binary_blob_labelling(open, labled_image, &nBlobs);

		vc_binary_blob_info(labled_image, blobs, nBlobs);

		if (i == 530 || i == 260 || i == 160 || i == 430 || i == 353)
		{
			char buffer[100];
			char buffer1[100];
			snprintf(buffer, 100, "./imgs/%i_seg.ppm", i);
			snprintf(buffer1, 100, "./imgs/%i_op.ppm", i);
			vc_write_image(buffer, segmentated);
			vc_write_image(buffer1, open);
		}

		vc_bgr_to_rgb(image);

		// Copia dados de imagem da estrutura IVC para uma estrutura cv::Mat
		memcpy(frame.data, image->data, video.width * video.height * 3);

		for (int i = 0; i < nBlobs; i++)
		{
			int numeroLaranja;
			if (blobs[i].height >= 280 && blobs[i].width >= 280)
			{
				int calibre = getCalibre((int)(blobs[i].width * 53) / 280);
				if (nLaranjas == 0)
				{
					nLaranjas++;
					laranjas[nLaranjas] = blobs[i];
					numeroLaranja = nLaranjas;
				}
				else
				{
					// implementar codigo de detertar se dentro de bounding box de alguma laranja
					bool existsLaranja = false;
					for (auto &t : laranjas)
					{

						auto key = t.first;
						auto laranja = t.second;
						int blobXEnd = blobs[i].x + blobs[i].width;
						int laranjaXEnd = laranja.x + laranja.width;

						if (!((blobs[i].y + blobs[i].height) < laranja.y || blobs[i].y > (laranja.y + laranja.height)) &&
								!(blobs[i].x > (laranja.x + laranja.width) || (blobs[i].x + blobs[i].width) < laranja.x))
						{

							laranjas[key] = blobs[i];
							numeroLaranja = key;
							existsLaranja = true;
						}
					}
					if (!existsLaranja)
					{
						printf("NOVA LARANJA!!\n");
						nLaranjas++;
						laranjas[nLaranjas] = blobs[i];
						numeroLaranja = nLaranjas;
					}
				}

				// for (auto &t : laranjas)
				// {
				// 	auto key = t.first;
				// 	auto value = t.second;
				// 	std::cout
				// 			<< t.first << " "
				// 			<< t.second.x << " "
				// 			<< t.second.width << "\n";
				// }

				cv::Rect rect(blobs[i].x, blobs[i].y, blobs[i].width, blobs[i].height);
				rectangle(frame, rect, (255, 255, 255), 2);
				char buffer[100];
				char buffer1[100];
				char buffer2[100];
				snprintf(buffer, 100, "Laranja: %d   Calibre: %d", numeroLaranja, calibre);
				snprintf(buffer1, 100, "Diametro: %d", blobs[i].width);
				snprintf(buffer2, 100, "NLaranjas: %d", nLaranjas);
				if ((blobs[i].y - 70) < 0)
				{
					putText(frame, buffer, Point(blobs[i].x, blobs[i].y + blobs[i].height + 80), 0, 0.7, (255, 255, 255), 2);
					putText(frame, buffer1, Point(blobs[i].x, blobs[i].y + blobs[i].height + 50), 0, 0.7, (255, 255, 255), 2);
					putText(frame, buffer2, Point(blobs[i].x, blobs[i].y + blobs[i].height + 20), 0, 0.7, (255, 255, 255), 2);
				}
				else
				{

					putText(frame, buffer, Point(blobs[i].x, blobs[i].y - 70), 0, 0.7, (255, 255, 255), 2);
					putText(frame, buffer1, Point(blobs[i].x, blobs[i].y - 40), 0, 0.7, (255, 255, 255), 2);
					putText(frame, buffer2, Point(blobs[i].x, blobs[i].y - 10), 0, 0.7, (255, 255, 255), 2);
				}
			}
		}

		if (i == 530 || i == 260 || (i <= 160 && i >= 140) || i == 430 || i == 353)
		{
			char buffer[100];
			memcpy(image->data, frame.data, video.width * video.height * 3);
			vc_bgr_to_rgb(image);
			snprintf(buffer, 100, "./imgs/%i.ppm", i);
			vc_write_image(buffer, image);
		}

		// Liberta a memória da imagem IVC que havia sido criada
		vc_image_free(image);
		vc_image_free(image_rgb);
		vc_image_free(image_hsv);
		vc_image_free(segmentated);
		vc_image_free(eroded);
		vc_image_free(dilated);
		vc_image_free(binary);
		vc_image_free(open);
		vc_image_free(labled_image);

		// +++++++++++++++++++++++++

		/* Exibe a frame */
		cv::imshow("VC - VIDEO", frame);

		/* Sai da aplicação, se o utilizador premir a tecla 'q' */
		key = cv::waitKey(1);
		i++;
	}

	/* Fecha a janela */
	cv::destroyWindow("VC - VIDEO");

	/* Fecha o ficheiro de vídeo */
	capture.release();

	return 0;
}