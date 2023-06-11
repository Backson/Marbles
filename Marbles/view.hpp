#pragma once

class Model;

class View {
public:
    View(double x, double y, double w) : _x(x), _y(y), _w(w) { }

    void draw(const Model &) const;

private:
    double _x;
    double _y;
    double _w;
};
