#include "Decoder.h"
#include <algorithm>
#include <cmath>

static constexpr double EPS = 1e-9;

bool comparePoints(const Point3D& a, const Point3D& b) {
    if (std::abs(a.getZ() - b.getZ()) > EPS) return a.getZ() < b.getZ();
    if (std::abs(a.getY() - b.getY()) > EPS) return a.getY() < b.getY();
    return a.getX() < b.getX();
}

static bool pointsEqual(const Point3D& a, const Point3D& b) {
    return std::abs(a.getX() - b.getX()) < EPS &&
        std::abs(a.getY() - b.getY()) < EPS &&
        std::abs(a.getZ() - b.getZ()) < EPS;
}

void Decoder::decode(Container& container, std::vector<Object>& sequence) {
    std::vector<Point3D> candidatePoints;
    candidatePoints.push_back(Point3D(0, 0, 0));

    double cLen = container.getLengthContainer();
    double cWid = container.getWidthContainer();
    double cHei = container.getHeightContainer();

    for (Object& obj : sequence) {
        int selectedPointIndex = -1;
        bool placed = false;

        std::sort(candidatePoints.begin(), candidatePoints.end(), comparePoints);

        int preferredOri = obj.getOrientation();
        std::vector<int> allowed = obj.getAllowedOrientationsList();
        std::vector<int> tryOrder;

        if (std::find(allowed.begin(), allowed.end(), preferredOri) != allowed.end()) {
            tryOrder.push_back(preferredOri);
        }
        for (int ori : allowed) {
            if (ori != preferredOri) tryOrder.push_back(ori);
        }

        for (size_t i = 0; i < candidatePoints.size(); ++i) {
            for (int ori : tryOrder) {
                obj.setOrientation(ori);
                obj.setPosition(candidatePoints[i]);

                if (container.canPlaceObject(obj)) {
                    selectedPointIndex = static_cast<int>(i);
                    placed = true;
                    break;
                }
            }
            if (placed) break;
        }

        if (placed) {
            container.placeObject(obj);

            double px = candidatePoints[selectedPointIndex].getX();
            double py = candidatePoints[selectedPointIndex].getY();
            double pz = candidatePoints[selectedPointIndex].getZ();
            double el = obj.getEffectiveLength();
            double ew = obj.getEffectiveWidth();
            double eh = obj.getEffectiveHeight();

            candidatePoints.erase(candidatePoints.begin() + selectedPointIndex);

            auto projectDown = [&](Point3D pt) {
                double bestZ = 0;
                for (const Object& placedObj : container.getPlacedObjects()) {
                    double px2 = placedObj.getPosition().getX();
                    double py2 = placedObj.getPosition().getY();
                    double pz2 = placedObj.getPosition().getZ();
                    if (pt.getX() < px2 + placedObj.getEffectiveLength() - EPS && pt.getX() + EPS > px2 &&
                        pt.getY() < py2 + placedObj.getEffectiveWidth() - EPS && pt.getY() + EPS > py2) {
                        if (pz2 + placedObj.getEffectiveHeight() <= pt.getZ() + EPS) {
                            bestZ = std::max(bestZ, pz2 + placedObj.getEffectiveHeight());
                        }
                    }
                }
                return Point3D(pt.getX(), pt.getY(), bestZ);
                };

            Point3D p1 = projectDown(Point3D(px + el, py, pz));
            Point3D p2 = projectDown(Point3D(px, py + ew, pz));
            Point3D p3 = projectDown(Point3D(px, py, pz + eh));

            for (const Point3D& newPt : { p1, p2, p3 }) {
                if (newPt.getX() >= cLen - EPS || newPt.getY() >= cWid - EPS || newPt.getZ() >= cHei - EPS) continue;

                bool exists = false;
                for (const Point3D& existing : candidatePoints) {
                    if (pointsEqual(existing, newPt)) {
                        exists = true; break;
                    }
                }
                if (!exists) candidatePoints.push_back(newPt);
            }
        }
    }
}