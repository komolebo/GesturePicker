#include "Starter.h"
#include "Functions.hpp"

std::map<int, int> frame_pat_counter;

void Starter::waitForPalmCover(FramesHolder *m) {
    camera >> m->src;

	if(m->src.empty()) {
		cout << "Data is empty" << endl;
		camera.release();
		exit(-1);
	}

    flip(m->src, m->src, 1);

	const int l = SENSOR_RECT_LEN;
    
	col_detectors.push_back(ColSensor(Point(m->src.cols / 3, m->src.rows / 6), Point(m->src.cols / 3 + l, m->src.rows / 6 + l), &m->src));
	col_detectors.push_back(ColSensor(Point(m->src.cols / 4, m->src.rows / 2), Point(m->src.cols / 4 + l, m->src.rows / 2 + l), &m->src));
	col_detectors.push_back(ColSensor(Point(m->src.cols / 3, m->src.rows / 1.5), Point(m->src.cols / 3 + l, m->src.rows / 1.5 + l), &m->src));
	col_detectors.push_back(ColSensor(Point(m->src.cols / 2, m->src.rows / 2), Point(m->src.cols / 2 + l, m->src.rows / 2 + l), &m->src));
	col_detectors.push_back(ColSensor(Point(m->src.cols / 2.5, m->src.rows / 2.5), Point(m->src.cols / 2.5 + l, m->src.rows / 2.5 + l), &m->src));
	col_detectors.push_back(ColSensor(Point(m->src.cols / 2, m->src.rows / 1.5), Point(m->src.cols / 2 + l, m->src.rows / 1.5 + l), &m->src));
	col_detectors.push_back(ColSensor(Point(m->src.cols / 2.5, m->src.rows / 1.8), Point(m->src.cols / 2.5 + l, m->src.rows / 1.8 + l), &m->src));
	
	m->src.release();
	
	for (int i = 0; i < FRAMES_WAIT_FOR_PALM; i++){
        camera >> m->src;
        flip(m->src, m->src, 1);

        for (int j = 0; j < N_COLOR_SENSORS; j++)
            col_detectors[j].drawRectangle(m->src);

        dumpMessage(m->src, "Cover rectangles with palm");

		if (!m->src.empty())
			imshow(MAIN_WINDOW_NAME, m->src);

		m->src.release();
        if (cv::waitKey(30) >= 0) break;
    }
}

void Starter::readPalmColor() {
    camera = cv::VideoCapture(0);

	if (!camera.isOpened()) {
		cout << "Webcam unavailable!" << endl;
		exit(-1);
	}

    namedWindow(MAIN_WINDOW_NAME, CV_WINDOW_KEEPRATIO);

    waitForPalmCover(&m);

    for (int i = 0; i < FRAMES_READ_AVER_COL; i++) {
        camera >> m.src;
        flip(m.src, m.src, 1);

        cvtColor(m.src, m.src, ORIGCOL2COL);

        for (int j = 0; j < N_COLOR_SENSORS; j++) {
            getAverageColor(col_detectors[j], avg_colors[j]);
            col_detectors[j].drawRectangle(m.src);
        }
        cvtColor(m.src, m.src, COL2ORIGCOL);

        dumpMessage(m.src, "Finding palm's average color");
        imshow(MAIN_WINDOW_NAME, m.src);
        if (cv::waitKey(30) >= 0) break;

		m.src.release();
	}
	m.src.release();

    destroyWindow(MAIN_WINDOW_NAME);
    camera.release();
	col_detectors.clear();
}

void Starter::showWindows(FramesHolder m) {
    pyrDown(m.bw, m.bw);
    pyrDown(m.bw, m.bw);

    Rect rec(Point(3 * m.src.cols / 4, 0), m.bw.size());
    Mat result;

    merge(vector<Mat>(3, m.bw), result);
    result.copyTo(m.src(rec));
    imshow(MAIN_WINDOW_NAME, m.src);
}

void Starter::initTrackbars() {
    for (int i = 0; i < N_COLOR_SENSORS; i++) {
        c_lower[i][0] = 12;
        c_lower[i][1] = 30;
		c_lower[i][2] = 80;
		c_upper[i][0] = 7;
        c_upper[i][1] = 40;
        c_upper[i][2] = 80;
    }
    createTrackbar("lower1", TRACKBAR_WINDOW_NAME, &c_lower[0][0], 255);
    createTrackbar("lower2", TRACKBAR_WINDOW_NAME, &c_lower[0][1], 255);
    createTrackbar("lower3", TRACKBAR_WINDOW_NAME, &c_lower[0][2], 255);
    createTrackbar("upper1", TRACKBAR_WINDOW_NAME, &c_upper[0][0], 255);
    createTrackbar("upper2", TRACKBAR_WINDOW_NAME, &c_upper[0][1], 255);
    createTrackbar("upper3", TRACKBAR_WINDOW_NAME, &c_upper[0][2], 255);
}

void outputLikeness(Mat &m, double likeness, int id) {
    Scalar c = (likeness > 0.1 ? Scalar(255 * (1 - likeness), 0, 0) : Scalar(125, 125, 125));
    string str = "#" + to_string(id) + ": " + to_string(likeness);
    putText(m, str, Point(50, 80), FONT_HERSHEY_PLAIN, 3.0f, c, 3);

    for (int i = 0; i < IpcMessages.size(); i++)
        putText(m, IpcMessages[i], Point(10, (20*i+20)), FONT_HERSHEY_PLAIN, 2.0f, Scalar(255, 0, 0), 3);
}

void checkLikeness() {
    for (auto k: frame_pat_counter)
        if (k.second >= SUCCESS_FRAMES_PER_SEC) {
            IpcMessages.push_back("#" + to_string(k.first));
			cout << to_string(k.first) << endl;
		}

    for (auto &k: frame_pat_counter)
        k.second = 0;
}

double countLikeness(const Figure &fig1, int &id) {
    double best_likeness = 1;

    int i = 0;
    for (const Figure &fig: figures) {
        double likeness = compareFigures(fig1, fig);

        if (likeness < best_likeness) {
            best_likeness = likeness;
            id = i;
        }
        i++;
    }

    if (best_likeness < SUCCESS_LIKENESS)
        frame_pat_counter[id]++;

    //cout << "like: " << best_likeness << " #" << id << endl;
    return best_likeness;
}


void outputContours(const vector<Contour> &contours, const char *window) {
    Mat m(500, 600, CV_8UC3);
    m.setTo(Scalar(0, 0, 0));

    int col_index = 1;
    for (const Contour &c: contours) {
        int v = (int) (col_index++ * 255 / contours.size());
        Scalar col = Scalar(v, v, v);

        for (int i = 0; i < c.size() - 1; i++)
            line(m, c[i], c[i + 1], col, 2);

        if (c.size() > 1)
            line(m, c[0], c[c.size() - 1], col, 2);
    }

    imshow(window, m);
    m.release();
}


void Starter::recognize(){
    camera = cv::VideoCapture(0);

    // read patterns
    readFile(patterns);
    for (const Contour &p: patterns)
        figures.push_back(convertPattern2Figure(p));

    // refresh counters
    for (int i = 0; i < patterns.size(); i++)
        frame_pat_counter[i] = 0;
    IpcMessages = vector<string>();

    namedWindow(MAIN_WINDOW_NAME, CV_WINDOW_FULLSCREEN);
    namedWindow(TEST_WINDOW, CV_WINDOW_AUTOSIZE);
    namedWindow(TRACKBAR_WINDOW_NAME, CV_WINDOW_AUTOSIZE);
	moveWindow(TEST_WINDOW, 800, 300);
    initTrackbars();

    double likeness = 1;
    int frame_num = 0, pattern_id = -1;
    while (cv::waitKey(20) != 113) {
        //cout << key << endl;

        camera >> m.src;

        // Make blurred copy
        flip(m.src, m.src, 1);
        pyrDown(m.src, m.srcBlur);
        blur(m.srcBlur, m.srcBlur, Size(3, 3));
        cvtColor(m.srcBlur, m.srcBlur, ORIGCOL2COL);

        // Get only colors in range between c_lower and c_upper
        filterByPalmColor(&m, c_lower, c_upper, avg_colors);

        cvtColor(m.srcBlur, m.srcBlur, COL2ORIGCOL);
        makeContours(&m, &hg);

        if (hg.isHand()) {
            hg.formPalmContour();
            hg.draw(&m);
        }

        if (frame_num % 2 == 0)
            likeness = countLikeness(hg.palmFigure, pattern_id);
        if (frame_num % 25 == 0)
            checkLikeness();
		
		dumpMessage(m.src, "Recognizing mode");

//        outputContours(patterns, "patterns");
        outputLikeness(m.src, likeness, pattern_id);
        outputContours(vector<Contour>(1, hg.palmContour), TEST_WINDOW);
		/*cvtColor(m.src, m.src, ORIGCOL2COL);
		for (int i = 0; i < N_COLOR_SENSORS; i++) {
		rectangle(m.src, Point(10, 50 + 50*i), Point(50, 50*i), Scalar(avg_colors[i][0], avg_colors[i][1], avg_colors[i][2]), 2, 8);
		}*/
        showWindows(m);

        m.src.release();
        m.bw.release();
        m.bwList.clear();
        frame_num++;
    }

    destroyAllWindows();
    camera.release();
}

void Starter::addPattern() {
	readFile(patterns);
    for (const Contour &p: patterns)
        figures.push_back(convertPattern2Figure(p));

    camera = cv::VideoCapture(0);

    namedWindow(MAIN_WINDOW_NAME, CV_WINDOW_FULLSCREEN);
    namedWindow(TEST_WINDOW, CV_WINDOW_AUTOSIZE);
	namedWindow(TRACKBAR_WINDOW_NAME, WINDOW_NORMAL);
	moveWindow(TEST_WINDOW, 800, 0);

    initTrackbars();

    int key;
    while ((key = cv::waitKey(20)) != 113) {
		if (key == 32 && state == WORKING)          // fix frame
            state = PAUSE;
        else if (key == 32 && state == PAUSE) {     // unfix frame
            state = WORKING;
        }
		else if (key == 115) {                      // s - save pattern; ENTER
			imwrite("AppData/GesturePicker/Patterns/pattern" + to_string(patterns.size()) + ".jpg", m.bw);
			cout << patterns.size() << endl;

            appendFile(hg.palmContour);
			
			patterns.clear();
			figures.clear();
			readFile(patterns);
			for (const Contour &p: patterns)
				figures.push_back(convertPattern2Figure(p));			
		}

        if (state == PAUSE) continue;
        //cout << key << endl;

        camera >> m.src;

        // Make blurred copy
        flip(m.src, m.src, 1);
        pyrDown(m.src, m.srcBlur);
        blur(m.srcBlur, m.srcBlur, Size(3, 3));
        cvtColor(m.srcBlur, m.srcBlur, ORIGCOL2COL);

        // Get only colors in range between c_lower and c_upper
		filterByPalmColor(&m, c_lower, c_upper, avg_colors);

        cvtColor(m.srcBlur, m.srcBlur, COL2ORIGCOL);
		makeContours(&m, &hg);

        if (hg.isHand()) {
            hg.formPalmContour();
            hg.draw(&m);

            outputContours(vector<Contour>(1, hg.palmContour), TEST_WINDOW);
        }

		dumpMessage(m.src, "Adding mode");
        showWindows(m);

        m.src.release();
        //m.bw.release();
		m.srcBlur.release();
        m.bwList.clear();
    }

    camera.release();
    destroyAllWindows();
}
