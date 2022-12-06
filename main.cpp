#include <iostream>
#include <cmath>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/videoio/videoio.hpp>

using namespace std;
using namespace cv;

Mat imagen, test, transformacion;

int maximoNiveles = 255, pixel = 0, niveles = 1, opciones = 0;

VideoCapture video(0);

void captura(int event, int x, int y, int flags, void *param)
{

    Mat *d = reinterpret_cast<Mat *>(param);
    Mat clone = d->clone();

    switch (event)
    {
    case EVENT_LBUTTONDOWN:
        imwrite("./captura.png", clone);
        break;

    default:
        break;
    }
}

void deslizante(int niveles, void *)
{
}

Mat videoCambio(int valor, Mat prueba1)
{
    vector<int> array;

    if (valor == 0)
    {
        for (int i = 0; i < prueba1.cols; i++)
        {
            for (int j = 0; j < prueba1.rows; j++)
            {
                prueba1.at<uchar>(j, i) = 0;
            }
        }

        return prueba1;
    }
    else
    {
        double intervalo = round(255 / valor);

        for (int i = 0; i < valor; i++)
        {
            array.push_back(intervalo * (i + 1));
        }

        for (int i = 0; i < prueba1.cols; i++)
        {
            for (int j = 0; j < prueba1.rows; j++)
            {
                pixel = prueba1.at<uchar>(j, i);
                for (int valorlimite : array)
                {
                    if (pixel <= valorlimite && pixel > (valorlimite - intervalo))
                    {

                        prueba1.at<uchar>(j, i) = valorlimite - intervalo;
                        break;
                    }
                }
            }
        }
        return prueba1;
    }
}

int main(int, char **)
{

    if (video.isOpened())
    {
        Mat frameAnterior, frameAnteriorV1, frameAnteriorV2, frameAnteriorV3;
        Mat frameHSV, frameLab, frameYcrCb, frameHSVV1, frameLabV1, frameYcrCbV1;
        Mat gris, resta;

        namedWindow("Escala", WINDOW_AUTOSIZE);
        namedWindow("Imagen_Entrada", WINDOW_AUTOSIZE);

        createTrackbar("deslizante", "Escala", &niveles, maximoNiveles, deslizante);
        createTrackbar("opciones", "Imagen_Entrada", &opciones, 4, deslizante);
        deslizante(opciones, 0);
        deslizante(niveles, 0);

        while (true)
        {
            video >> imagen;
            if (imagen.empty())
            {
                break;
            }
            // Cambiar tamanio de imagen
            resize(imagen, imagen, Size(), 0.25, 0.25);
            cvtColor(imagen, test, COLOR_BGR2GRAY);
            cvtColor(imagen, gris, COLOR_BGR2GRAY);
            cvtColor(imagen, frameHSV, COLOR_BGR2HSV);
            cvtColor(imagen, frameLab, COLOR_BGR2Lab);
            cvtColor(imagen, frameYcrCb, COLOR_BGR2YCrCb);

            if (frameAnterior.empty() && frameAnteriorV1.empty() && frameAnteriorV2.empty() && frameAnteriorV3.empty())
            {
                frameAnterior = gris.clone();
                frameAnteriorV1 = frameHSV.clone();
                frameAnteriorV2 = frameLab.clone();
                frameAnteriorV3 = frameYcrCb.clone();
            }

            absdiff(gris, frameAnterior, resta);
            absdiff(frameHSV, frameAnteriorV1, frameHSVV1);
            absdiff(frameLab, frameAnteriorV2, frameLabV1);
            absdiff(frameYcrCb, frameAnteriorV3, frameYcrCbV1);

            frameAnterior = gris.clone();
            frameAnteriorV1 = frameHSV.clone();
            frameAnteriorV2 = frameLab.clone();
            frameAnteriorV3 = frameYcrCb.clone();

            int niveles1 = getTrackbarPos("deslizante", "Escala");
            int niveles2 = getTrackbarPos("opciones", "Imagen_Entrada");
            transformacion = videoCambio(niveles1, test);

            imshow("Escala", transformacion);
            switch (niveles2) // donde opción es la variable a comparar
            {
            case 0:
                imshow("Imagen_Entrada", imagen);
                setMouseCallback("Imagen_Entrada", captura, reinterpret_cast<void *>(&imagen));

                break;
            case 1:
                imshow("Imagen_Entrada", frameHSVV1);
                setMouseCallback("Imagen_Entrada", captura, reinterpret_cast<void *>(&frameHSVV1));
                break;
            case 3:
                imshow("Imagen_Entrada", frameYcrCbV1);
                setMouseCallback("Imagen_Entrada", captura, reinterpret_cast<void *>(&frameYcrCbV1));
                break;
            default:
                imshow("Imagen_Entrada", resta);
                setMouseCallback("Imagen_Entrada", captura, reinterpret_cast<void *>(&resta));
            }
            

            if (waitKey(23) == 27)
            {
                break;
            }
        }
        destroyAllWindows();
        video.release();
        return 0;
    }
}