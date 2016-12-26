#include "handGesture.hpp"
#include "Functions.hpp"

using namespace cv;
using namespace std;

void HandGesture::initVectors() {
    hullI       = vector<int>  (contour.size());
    hullP       = Contour      (contour.size());
    defects     = vector<Vec4i>(contour.size());
    palmContour = Contour      (contour.size());
    palmFigure  = Figure       (contour.size());
}

void HandGesture::analyzeBRect() {
    double h = bRect.height;
    double w = bRect.width;

    mayBeHand = true;

    if (h == 0 || w == 0 || h / w > 4 || w / h > 4) mayBeHand = false;
    if (fingerTips.size() > 5 || bRect.x < 20) mayBeHand = false;
}

void HandGesture::eliminateDefects() {
    int tolerance = bRect.height / 5;
    float angleTol = 95;
    vector<Vec4i> newDefects;
    int startidx, endidx, faridx;
    for (vector<Vec4i>::iterator d = defects.begin(); d != defects.end(); d++) {
        Vec4i v = (*d);

        startidx = v[0];
        endidx = v[1];
        faridx = v[2];

        Point ptStart(contour[startidx]);
        Point ptEnd(contour[endidx]);
        Point ptFar(contour[faridx]);
        if (distanceP2P(ptStart, ptFar) > tolerance && distanceP2P(ptEnd, ptFar) > tolerance &&
            getAngle(ptStart, ptFar, ptEnd) < angleTol) {
            if (ptEnd.y > (bRect.y + bRect.height - bRect.height / 4)) {}
            else if (ptStart.y > (bRect.y + bRect.height - bRect.height / 4)) {}
            else {
                newDefects.push_back(v);
            }
        }
    }
    defects.swap(newDefects);
    removeRedundantEndPoints(defects);
}

// remove endpoint of convexity defects if they are at the same fingertip
void HandGesture::removeRedundantEndPoints(const vector<Vec4i> &newDefects) {
    float tolerance = bRect.width / 6;
    int startidx, endidx;
    int startidx2, endidx2;
    for (int i = 0; i < newDefects.size(); i++) {
        for (int j = i; j < newDefects.size(); j++) {
            startidx = newDefects[i][0];
            startidx2 = newDefects[j][0];
            endidx = newDefects[i][1];
            endidx2 = newDefects[j][1];

            Point ptStart(contour[startidx]);
            Point ptStart2(contour[startidx2]);
            Point ptEnd(contour[endidx]);
            Point ptEnd2(contour[endidx2]);

            if (distanceP2P(ptStart, ptEnd2) < tolerance) {
                contour[startidx] = ptEnd2;
                break;
            }
            if (distanceP2P(ptEnd, ptStart2) < tolerance) {
                contour[startidx2] = ptEnd;
            }
        }
    }
}


void HandGesture::formPalmContour() {
    if (!mayBeHand || contour.size() == 0 || hullP.empty())
        return;

    palmContour.clear();
    palmContour = hullP;

    // Put defect points
    for (Vec4i def_point: defects) {
        Point ptStart(contour[def_point[0]]);
        Point ptFar(contour[def_point[2]]);

        // find closest dot to start defect point
        vector<Point>::iterator closest_dot;
        float min_dist = INT_MAX;
        for (auto it = palmContour.begin(); it != palmContour.end(); it++) {
            float dist = distanceP2P(*it, ptStart);

            if (dist < min_dist) {
                min_dist = dist;
                closest_dot = it;
            }
        }

        palmContour.insert(closest_dot, ptFar);
    }

    placePicture2Origin(palmContour);
    orderContourFromOrigin(palmContour);

    palmFigure = convertPattern2Figure(palmContour);
}

void HandGesture::analyzeContour(Contour c) {
    contour = c;
    bRect = boundingRect(Mat(contour));

    //approxPolyDP( Mat(hg->contours[hg->cIdx]), hg->contours[hg->cIdx], 11, true );

    convexHull(Mat(contour), hullP, false, true);
    convexHull(Mat(contour), hullI, false, false);
    approxPolyDP(Mat(hullP), hullP, 18, true);

    // if contour exists
    if (contour.size() > 3) {
        convexityDefects(contour, hullI, defects);
        eliminateDefects();
    }
}

void HandGesture::draw(FramesHolder *m) {
    vector<Contour> a = vector<Contour>(1, hullP);
    drawContours(m->src, a, 0,cv::Scalar(200,0,0),2, 8, vector<Vec4i>(), 0, Point());

    // Draw bounding rectangle
    rectangle(m->src, bRect.tl(), bRect.br(),Scalar(0,0,200));

    for (auto &d: defects) {
        int startidx=d[0];
        int endidx=d[1];
        int faridx=d[2];

        Point ptStart(contour[startidx] );
        Point ptEnd(contour[endidx] );
        Point ptFar(contour[faridx] );

        line( m->src, ptStart, ptFar, Scalar(0,255,0), 1 );
        line( m->src, ptEnd, ptFar, Scalar(0,255,0), 1 );
        circle( m->src, ptFar,   4, Scalar(0,255,0), 2 );
        circle( m->src, ptEnd,   4, Scalar(0,0,255), 2 );
        circle( m->src, ptStart,   4, Scalar(255,0,0), 2 );
    }
}
