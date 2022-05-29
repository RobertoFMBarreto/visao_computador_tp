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

		// inverter os canais de bgr para rgb
		vc_bgr_to_rgb(image);

		// converter rgb em hsv para ser possivel segmentar
		vc_rgb_to_hsv(image, image_hsv);

		// vc_hsv_segmentation(image_hsv, segmentated, 10, 30, 80, 100, 27, 80);
		// aplicar a segmentação em hsv com os valores detetados no gimp
		// -> hmin 10
		// -> hmax 30
		// -> smin 75
		// -> smax 100
		// -> vmin 25
		// -> vmax 80
		vc_hsv_segmentation(image_hsv, segmentated, 10, 30, 75, 100, 25, 80);

		// aplicar o open pois existe algum ruido na imagem e convém remover com o erode
		// mas também não se pode perder muito do tamanho da laranja neste processo
		// sendo necessario aplicar um kernel igual para realizar o dilate

		vc_binary_open(segmentated, open, 5, 5);

		// realizar o labelling sobre a imagem segmentada
		blobs = vc_binary_blob_labelling(open, labled_image, &nBlobs);

		// obter as informações sobre os blobs descobertos anteriormente
		vc_binary_blob_info(labled_image, blobs, nBlobs);

		// converter em bgr novamente
		vc_bgr_to_rgb(image);

		// Copia dados de imagem da estrutura IVC para uma estrutura cv::Mat
		memcpy(frame.data, image->data, video.width * video.height * 3);

		// iterar pelos blobs encontrados
		for (int i = 0; i < nBlobs; i++)
		{
			// valor que guarda o numero da laranja identificada para cada blob
			int numeroLaranja;
			// verificar se o blob tem tamanho suficente para ser identificado como uma laranja
			if (blobs[i].height >= 280 && blobs[i].width >= 280)
			{
				// calcular o calibre da laranja tendo em conta o seu tamanho convertido em mm
				int calibre = getCalibre((int)(blobs[i].width * 53) / 280);
				// se ainda não tiverem sido detetadas laranjas então vamos inserir a primeira
				if (nLaranjas == 0)
				{
					// incrementar o contador de laranjas
					nLaranjas++;
					// utilizar o contador de laranjas como key para o dicionario
					// guardar a informação do blob da laranja
					laranjas[nLaranjas] = blobs[i];
					// alterar o numero da laranja pois descobrimos de qual laranja se trata
					numeroLaranja = nLaranjas;
				}
				else
				{
					// iterar pelo dicionario de laranjas para ver se o blob atual poderá ser alguma das laranjas já existentes
					// iniciar sempre a achar que a laranja não existe
					bool existsLaranja = false;
					for (auto &t : laranjas)
					{
						// numero dado à laranja
						auto key = t.first;
						// blob guardado
						auto laranja = t.second;

						// Verificar se:
						// 	Em Y:
						// 		-> Se o blob não está acima da laranja ou seja se este termina antes desta laranja começar
						// 		-> Se o blob não está abaixo da laranja ou seja se este não começa depois do termino da laranja
						// 	Em X:
						//		-> Se o blob não está à esquerda ou seja se este não termina antes do da laranja
						//		-> Se o blob não está à direita ou seja se este não começa depois do termino da laranja
						if (!((blobs[i].y + blobs[i].height) < laranja.y || blobs[i].y > (laranja.y + laranja.height)) &&
								!(blobs[i].x > (laranja.x + laranja.width) || (blobs[i].x + blobs[i].width) < laranja.x))
						{
							// caso nem esteja a direita nem a esquerda nem por cima nem por baixo então quer dizer qeu se trata da mesma laranja
							// mas lijeiramente deslocada
							// logo temos de guardar a nova posição desta laranja
							laranjas[key] = blobs[i];
							// alterar o numero da laranja pois já detetamos a qual laranja este blob pertence
							numeroLaranja = key;
							// marcar que a laranja existe para esta não ser criada
							existsLaranja = true;
						}
					}
					// caso não se tenha descoberto a laranja vamos então adicionar esta laranja às laranjas conhecidas
					if (!existsLaranja)
					{
						// incrementar o contador de laranjas
						nLaranjas++;
						// utilizar o contador de laranjas como key para o dicionario
						// guardar a informação do blob da laranja
						laranjas[nLaranjas] = blobs[i];
						// alterar o numero da laranja pois descobrimos de qual laranja se trata
						numeroLaranja = nLaranjas;
					}
				}

				// Rect é uma classe que guarda informações sobre um retangulo
				// sendo estas:
				// -> canto superior esquerdo (xInicial, yInicial)
				// -> largura e altura (width, height)
				// utilizando assim esta para guardar informações sobre o blob
				cv::Rect rect(blobs[i].x, blobs[i].y, blobs[i].width, blobs[i].height);

				// Desenhar a bounding box de acordo com o retangulo criado anteriormente com o Rect
				//  desenhar no próprim frame com uma linha azul e espesura de 2
				rectangle(frame, rect, (255, 255, 255), 2);

				char buffer[100];
				char buffer1[100];
				char buffer2[100];
				snprintf(buffer, 100, "Laranja: %d   Calibre: %d", numeroLaranja, calibre);
				snprintf(buffer1, 100, "Diametro: %d", blobs[i].width);
				snprintf(buffer2, 100, "NLaranjas: %d", nLaranjas);

				// se o texto a escrever ficar fora dos limites da imagem para cima
				// então este vai ser movido para baixo até haver espaço
				if ((blobs[i].y - 70) < 0)
				{
					// putText permite escrever texto em uma imagem
					// putText necessita de:
					// 	-> imagem onde escrever, neste caso o frame
					// 	-> string a escrever
					//	-> Objeto Point que indica um ponto dando as suas coordenadas
					// 		- Neste caso foi dado o inicio da imagem em x para ficar alinhado à esquerda
					//    - Foi dado também o inicio da imagem em y + a sua altura para se chegar o fim da imagem em y
					//    - Foi também acrescentado valores para espaçar as diferentes strings
					// -> Foi dado um tamanho de fonte de 0.7, uma cor azul e uma espessura de 2
					putText(frame, buffer, Point(blobs[i].x, blobs[i].y + blobs[i].height + 80), 0, 0.7, (255, 255, 255), 2);
					putText(frame, buffer1, Point(blobs[i].x, blobs[i].y + blobs[i].height + 50), 0, 0.7, (255, 255, 255), 2);
					putText(frame, buffer2, Point(blobs[i].x, blobs[i].y + blobs[i].height + 20), 0, 0.7, (255, 255, 255), 2);
				}
				else
				{
					// Assim como o putText acima apenas modificando que agora queremos escrever acima da imagem e não abaixo
					putText(frame, buffer, Point(blobs[i].x, blobs[i].y - 70), 0, 0.7, (255, 255, 255), 2);
					putText(frame, buffer1, Point(blobs[i].x, blobs[i].y - 40), 0, 0.7, (255, 255, 255), 2);
					putText(frame, buffer2, Point(blobs[i].x, blobs[i].y - 10), 0, 0.7, (255, 255, 255), 2);
				}
			}
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