#include <iostream>
#include <fstream>
#include <vector>
#include <random>

struct point
{
    double x, y;
};

class AffineTransformation         // AffineTransformation: f(x) = M*x + v
{
    private:
        //matrix 2x2
        double M[2][2];

        //vetor
        double v[2];

    public:
        AffineTransformation(double m11, double m12, double m21, double m22, double v1, double v2)
        {
            //matrix 2x2
            M[0][0] = m11;
            M[0][1] = m12;
            M[1][0] = m21;
            M[1][1] = m22;

            //vecctor
            v[0] = v1;
            v[1] = v2;
        }

        point operator()(const point& p) const
        {
            return
            {
                M[0][0] * p.x + M[0][1] * p.y + v[0],
                M[1][0] * p.x + M[1][1] * p.y + v[1]
            };
        }
};

class TransformationSet
{
    private:
        std::vector<AffineTransformation> transformations;
        std::mt19937 gen;
        std::uniform_int_distribution<> dist;

    public:
        TransformationSet():
            gen(std::random_device{}())
            {

            }

        void addTransformation(const AffineTransformation& transformation)
        {
            transformations.push_back(transformation);

            dist = std::uniform_int_distribution<>(0, (transformations.size() - 1));
        }

        point applyRandomTransformation(const point& p)
        {
            int index = dist(gen);

            return
                transformations[index](p);
        }
};

int main()
{
    std::ifstream input("input.txt");
    std::ofstream output("output.txt");

    int n;
    double m11, m12, m21, m22, v1, v2;
    point startPoint;
    TransformationSet transformationSet;

/*
    n - amount of points
    (x,y) - start point
    3 Affine transformations
*/
    input >> n >> startPoint.x >> startPoint.y;
    while (input >> m11 >> m12 >> m21 >> m22 >> v1 >> v2)
    {
        transformationSet.addTransformation(AffineTransformation(m11, m12, m21, m22, v1, v2));
    }

    point currentPoint = startPoint;

    for (int i = 0; i < n; i++)
    {
        output << currentPoint.x << ' ' << currentPoint.y << '\n';
        currentPoint = transformationSet.applyRandomTransformation(currentPoint);
    }

    return 0;
}
