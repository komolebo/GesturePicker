#include <sstream>
#include "Functions.hpp"


float distanceP2P(Point a, Point b) {
    return sqrt(fabs(pow(a.x - b.x, 2) + pow(a.y - b.y, 2)));
}

float getAngle(Point s, Point f, Point e){
    float l1 = distanceP2P(f,s);
    float l2 = distanceP2P(f,e);
    float dot=(s.x-f.x)*(e.x-f.x) + (s.y-f.y)*(e.y-f.y);
    float angle = acos(dot/(l1*l2));
    return (float) (angle * 180 / PI);
}

void dumpMessage(Mat src, string text) {
    putText(src, text, Point(src.cols / 2, src.rows / 10), FONT_HERSHEY_PLAIN, 2.f, Scalar(255, 0, 0), 2);
}


void normalizeColors(Sensors c_lower, Sensors c_upper, Sensors avg_colors) {
    // copy all boundaries read from track bar to all of the different boundaries
    for (int i = 1; i < N_COLOR_SENSORS; i++) {
        for (int j = 0; j < 3; j++) {
            c_lower[i][j] = c_lower[0][j];
            c_upper[i][j] = c_upper[0][j];
        }
    }

    // adopt boundaries for color levels read from user's palm
    for (int i = 0; i < N_COLOR_SENSORS; i++) {
        if ((avg_colors[i][0] - c_lower[i][0]) < 0)
            c_lower[i][0] = avg_colors[i][0];
        if ((avg_colors[i][1] - c_lower[i][1]) < 0)
            c_lower[i][1] = avg_colors[i][1];
        if ((avg_colors[i][2] - c_lower[i][2]) < 0)
            c_lower[i][2] = avg_colors[i][2];
        if ((avg_colors[i][0] + c_upper[i][0]) > 255)
            c_upper[i][0] = 255 - avg_colors[i][0];
        if ((avg_colors[i][1] + c_upper[i][1]) > 255)
            c_upper[i][1] = 255 - avg_colors[i][1];
        if ((avg_colors[i][2] + c_upper[i][2]) > 255)
            c_upper[i][2] = 255 - avg_colors[i][2];
    }
}

int getMedian(vector<int> &val) {
    size_t n = val.size();

    sort(val.begin(), val.end());

    return n % 2 ? val[n / 2] : val[n / 2 - 1];
}

void getAverageColor(const ColSensor &sensor, int *avg) {
    Mat r;
    vector<int> r_vec, g_vec, b_vec;

    sensor.detect_area.copyTo(r);

    // generate vectors
    for (int i = sensor.BORDER_THICKNESS; i < r.rows - sensor.BORDER_THICKNESS; i++)
        for (int j = sensor.BORDER_THICKNESS; j < r.cols - sensor.BORDER_THICKNESS; j++) {
            int index = r.cols * i + j;
            r_vec.push_back(r.data[r.channels() * index + 0]);
            g_vec.push_back(r.data[r.channels() * index + 1]);
            b_vec.push_back(r.data[r.channels() * index + 2]);
        }

    avg[0] = getMedian(r_vec);
    avg[1] = getMedian(g_vec);
    avg[2] = getMedian(b_vec);
}

void filterByPalmColor(FramesHolder *m, Sensors c_lower, Sensors c_upper, Sensors avg_colors) {
    Scalar lowerBound, upperBound;
    for (int i = 0; i < N_COLOR_SENSORS; i++) {
        normalizeColors(c_lower, c_upper, avg_colors);
        lowerBound = Scalar(avg_colors[i][0] - c_lower[i][0], avg_colors[i][1] - c_lower[i][1],
                            avg_colors[i][2] - c_lower[i][2]);
        upperBound = Scalar(avg_colors[i][0] + c_upper[i][0], avg_colors[i][1] + c_upper[i][1],
                            avg_colors[i][2] + c_upper[i][2]);
        m->bwList.push_back(Mat(m->srcBlur.rows, m->srcBlur.cols, CV_8U));
        inRange(m->srcBlur, lowerBound, upperBound, m->bwList[i]);
    }
    m->bwList[0].copyTo(m->bw);
    for (int i = 1; i < N_COLOR_SENSORS; i++)
        m->bw += m->bwList[i];

    medianBlur(m->bw, m->bw, 7);
}

int findBiggestContour(const vector<Contour> &contours){
    int index_max = -1, max_size = 0;

    for (int i = 0; i < contours.size(); i++)
        if(contours[i].size() > max_size){
            max_size = (int) contours[i].size();
            index_max = i;
        }

    return index_max;
}

void makeContours(FramesHolder *m, HandGesture* hg){
    Mat mat_bw;
    pyrUp(m->bw,m->bw);
    m->bw.copyTo(mat_bw);

    vector<Contour> contours = vector<Contour>();
    findContours(mat_bw, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

    int biggest_id = findBiggestContour(contours);

    hg->initVectors();
    if (biggest_id != -1) {
        hg->analyzeContour(contours[biggest_id]);
        hg->analyzeBRect();
    }
}

Figure convertPattern2Figure(const Contour &pattern) {
    Figure figure = Figure();

    for (int i = 0; i < pattern.size() - 1; i++)
        figure.push_back(pattern[i + 1] - pattern[i]);

    return figure;
}

void orderContourFromOrigin(Contour &c) {
    int index = -1;
    float min_dist = INT_MAX;

    // Search most left-downed point from contour
    for (int i = 0; i < c.size(); i++) {
        float dist = distanceP2P(c[i], Point(0, 2000));

        if (dist < min_dist) {
            min_dist = dist;
            index = i;
        }
    }

    // Change contour order starting from index
    Contour new_contour;
    for (int i = index; i < c.size(); i++)
        new_contour.push_back(c[i]);
    for (int i = 0; i < index; i++)
        new_contour.push_back(c[i]);

    c.swap(new_contour);
}

void readFile(const char *name, vector<Contour> &patterns) {
    ifstream f(name);

    if (!f) return;

    string line;
    Contour pattern = Contour();

    int tmp_x, tmp_y;
    while (getline(f, line)) {
        istringstream iss(line);

        pattern.clear();

        while (iss.rdbuf()->in_avail()) {
            iss >> tmp_x;
            iss >> tmp_y;

            pattern.push_back(Point(tmp_x, tmp_y));
        }

        patterns.push_back(pattern);
    }

    f.close();
}

void appendFile(const char *name, const Contour &pattern) {
    ofstream f(name, std::ios_base::app);

    if (!f || pattern.empty()) return;

    // Make contour points start from most left-down position
    Contour new_contour = pattern;
    orderContourFromOrigin(new_contour);

    for (const Point &p: new_contour)
        f << p.x << ' ' << p.y << "  ";
    f << endl;

    f.close();
}

float compareFigures(const Figure &fig1, const Figure &fig2) {
    Figure *n_fig1 = normalize(fig1);
    Figure *n_fig2 = normalize(fig2);

    long n = max(n_fig1->size(), n_fig2->size());

    // Make them same-sized
    Figure *smaller = (n_fig1->size() == n ? n_fig2 : n_fig1);
    for (int i = 0; i < n - smaller->size(); i++)
        smaller->push_back(Point(0, 0));

    // Count difference
    float dif_x = 0, dif_y = 0;
    for (int i = 0; i < n; i++) {
        dif_x += abs((*n_fig1)[i].x - (*n_fig2)[i].x);
        dif_y += abs((*n_fig1)[i].y - (*n_fig2)[i].y);
    }

    float k = (dif_x + dif_y) / 2;

    // Normalize result to [0..1]
    k /= 2 * n;

    delete n_fig1;
    delete n_fig2;

    return k;
}

Figure *normalize(const Figure &fig) {
    Figure *new_fig = new Figure();

    float max_dx = 0;
    float max_dy = 0;

    for (const CvPoint2D32f &p: fig) {
        if (max_dx < abs(p.x))
            max_dx = abs(p.x);
        if (max_dy < abs(p.y))
            max_dy = abs(p.y);
    }

    for (const CvPoint2D32f &p: fig)
        new_fig->push_back(CvPoint2D32f(p.x / max_dx, p.y / max_dy));

    return new_fig;
}

void placePicture2Origin(Contour &picture) {
    int min_x = INT_MAX, min_y = INT_MAX;

    for (Point &p: picture) {
        if (p.x < min_x)
            min_x = p.x;
        if (p.y < min_y)
            min_y = p.y;
    }

    for (Point &p: picture) {
        p.x -= min_x;
        p.y -= min_y;
    }
}