all:
	g++ main.cpp -o vision.bin -I/usr/include/opencv4 -L/usr/lib/x86_64-linux-gnu/ -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_imgcodecs -lopencv_video -lopencv_videoio

run:
	./vision.bin