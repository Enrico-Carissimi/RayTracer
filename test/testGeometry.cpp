#include <iostream>
#include "Vec3.hpp"
#include "Point3.hpp"
#include "Normal3.hpp"
#include "Transformation.hpp"
#include "utils.hpp"



void testVec3operations() {
 
    Vec3 a = Vec3(1.0, 2.0, 3.0);
    Vec3 b = Vec3(4.0, 6.0, 8.0);

    sassert(a.isClose(a));
    sassert(!a.isClose(b));

    sassert((-a).isClose(Vec3(-1.0, -2.0, -3.0)));
    sassert((a + b).isClose(Vec3(5.0, 8.0, 11.0)));
    sassert((b - a).isClose(Vec3(3.0, 4.0, 5.0)));
    sassert((a * 2).isClose(Vec3(2.0, 4.0, 6.0)));
    sassert((2 * a).isClose(Vec3(2.0, 4.0, 6.0)));
    sassert(areClose(dot(a, b), 40.0f));
    sassert(cross(a, b).isClose(Vec3(-2.0, 4.0, -2.0))); 
    sassert(cross(b, a).isClose(Vec3(2.0, -4.0, 2.0))); 
    sassert(areClose(a.norm2(), 14.0f));
    sassert(areClose(a.norm() * a.norm(), 14.0f));

    a += b;
    sassert(a.isClose(Vec3(5.0, 8.0, 11.0)));

}

void testPoint3operations() {

    Point3 p1(1.0, 2.0, 3.0);
    Vec3 v(4.0, 6.0, 8.0);
    Point3 p2(4.0, 6.0, 8.0);

    sassert(p1.isClose(p1));
    sassert(!p1.isClose(p2));

    sassert((p1 * 2).isClose(Point3(2.0, 4.0, 6.0)));
    sassert((p1 + v).isClose(Point3(5.0, 8.0, 11.0)));  
    sassert((p2 - p1).isClose(Vec3(3.0, 4.0, 5.0)));  
    sassert((p1 - v).isClose(Point3(-3.0, -4.0, -5.0)));  

}

void testTrasformation() {

    float mat[16] = {
        1.0f, 2.0f, 3.0f, 4.0f,
        5.0f, 6.0f, 7.0f, 8.0f,
        9.0f, 9.0f, 8.0f, 7.0f,
        6.0f, 5.0f, 4.0f, 1.0f
    };

    float inv[16] = {
        -3.75f,  2.75f, -1.0f,   0.0f,
        4.375f, -3.875f, 2.0f, -0.5f,
        0.5f,   0.5f,  -1.0f,   1.0f,
        -1.375f, 0.875f, 0.0f,  -0.5f
    };

    Transformation m1(mat, inv);
    sassert(m1.isConsistent());

    Transformation m2(mat, inv);
    sassert(areCloseMatrix(m1.matrix, m2.matrix));
    sassert(areCloseMatrix(m1.inverseMatrix, m2.inverseMatrix));           

    Transformation m3(mat, inv);
    m3.matrix[10] += 1.0f; 
    sassert(!m1.isClose(m3));

    Transformation m4(mat, inv);
    m4.inverseMatrix[10] += 1.0f;
    sassert(!m1.isClose(m4));

}

void testTransformationMultiplication() {
    float m1_matrix[16] = {
        1.0, 2.0, 3.0, 4.0,
        5.0, 6.0, 7.0, 8.0,
        9.0, 9.0, 8.0, 7.0,
        6.0, 5.0, 4.0, 1.0
    };
    
    float m1_invm[16] = {
        -3.75, 2.75, -1, 0,
        4.375, -3.875, 2.0, -0.5,
        0.5, 0.5, -1.0, 1.0,
        -1.375, 0.875, 0.0, -0.5
    };
    
    Transformation m1(m1_matrix, m1_invm);
    sassert(m1.isConsistent());

    float m2_matrix[16] = {
        3.0, 5.0, 2.0, 4.0,
        4.0, 1.0, 0.0, 5.0,
        6.0, 3.0, 2.0, 0.0,
        1.0, 4.0, 2.0, 1.0
    };
    
    float m2_invm[16] = {
        0.4, -0.2, 0.2, -0.6,
        2.9, -1.7, 0.2, -3.1,
        -5.55, 3.15, -0.4, 6.45,
        -0.9, 0.7, -0.2, 1.1
    };
    
    Transformation m2(m2_matrix, m2_invm);
    sassert(m2.isConsistent());

    float expected_matrix[16] = {
        33.0, 32.0, 16.0, 18.0,
        89.0, 84.0, 40.0, 58.0,
        118.0, 106.0, 48.0, 88.0,
        63.0, 51.0, 22.0, 50.0
    };

    float expected_invm[16] = {
        -1.45, 1.45, -1.0, 0.6,
        -13.95, 11.95, -6.5, 2.6,
        25.525, -22.025, 12.25, -5.2,
        4.825, -4.325, 2.5, -1.1
    };

    Transformation expected(expected_matrix, expected_invm);
    sassert(expected.isConsistent()); 

    sassert(areCloseMatrix(expected.matrix, (m1 * m2).matrix));
    sassert(areCloseMatrix(expected.inverseMatrix, (m1 * m2).inverseMatrix));
}

void testVecPointMultiplication() {

    float m_matrix[16] = {
        1.0, 2.0, 3.0, 4.0,
        5.0, 6.0, 7.0, 8.0,
        9.0, 9.0, 8.0, 7.0,
        0.0, 0.0, 0.0, 1.0
    };
        
    float m_invm[16] = {
        -3.75, 2.75, -1, 0,
        5.75, -4.75, 2.0, 1.0,
        -2.25, 2.25, -1.0, -2.0,
        0.0, 0.0, 0.0, 1.0
    };
        
    Transformation m(m_matrix, m_invm);
    sassert(m.isConsistent());
    
    Vec3 expected_v(14.0, 38.0, 51.0);
    sassert(expected_v.isClose(m * Vec3(1.0, 2.0, 3.0)));
    
    Point3 expected_p(18.0, 46.0, 58.0);
    sassert(expected_p.isClose(m * Point3(1.0, 2.0, 3.0)));
    
    Normal3 expected_n(-8.75, 7.75, -3.0);
    sassert(expected_n.isClose(m * Normal3(3.0, 2.0, 4.0)));
}

void testTransformationInverse() {

    float m_matrix[16] = {
        1.0, 2.0, 3.0, 4.0,
        5.0, 6.0, 7.0, 8.0,
        9.0, 9.0, 8.0, 7.0,
        6.0, 5.0, 4.0, 1.0
    };
    
    float m_invm[16] = {
        -3.75, 2.75, -1, 0,
        4.375, -3.875, 2.0, -0.5,
        0.5, 0.5, -1.0, 1.0,
        -1.375, 0.875, 0.0, -0.5
    };
    
    Transformation m1(m_matrix, m_invm);
    sassert(m1.isConsistent());

    Transformation m2 = m1.inverse();
    sassert(m2.isConsistent());

    Transformation prod = m1 * m2;
    sassert(prod.isConsistent());
    sassert(prod.isClose(Transformation()));
}

void testTranslation() {

    Transformation tr1 = translation(Vec3(1.0, 2.0, 3.0));
    sassert(tr1.isConsistent());

    Transformation tr2 = translation(Vec3(4.0, 6.0, 8.0));
    sassert(tr2.isConsistent());

    Transformation prod = tr1 * tr2;
    sassert(prod.isConsistent());

    Transformation expected = translation(Vec3(5.0, 8.0, 11.0));
    sassert(prod.isClose(expected));
}

void testRotation() {

    const Vec3 vecX(1.0, 0.0, 0.0);
    const Vec3 vecY(0.0, 1.0, 0.0);
    const Vec3 vecZ(0.0, 0.0, 1.0);

    sassert(rotation(0.1, Axis::X).isConsistent()); 
    sassert(rotation(0.1, Axis::Y).isConsistent());  
    sassert(rotation(0.1, Axis::Z).isConsistent());  
    
    // Rotation tests of some specific rotations
    sassert((rotation(90, Axis::X) * vecY).isClose(vecZ));  // 90° rotation around the X-axis
    sassert((rotation(90, Axis::Y) * vecZ).isClose(vecX));  // 90° rotation around the Y-axis
    sassert((rotation(90, Axis::Z) * vecX).isClose(vecY));  // 90° rotation around the Z-axis
    
}

void testScaling() {
    
    Transformation tr1 = scaling(Vec3(2.0, 5.0, 10.0));
    sassert(tr1.isConsistent());

    Transformation tr2 = scaling(Vec3(3.0, 2.0, 4.0));
    sassert(tr2.isConsistent());

    Transformation expected = scaling(Vec3(6.0, 10.0, 40.0));
    
    sassert(expected.isClose(tr1 * tr2));
}

float epsilon = 1e-3;

void testONB() {

    PCG pcg;

    for (int i = 0; i < 100; ++i) {
        Vec3 normal = Vec3(pcg.random(), pcg.random(), pcg.random()).normalize();

        Vec3 e1, e2, e3;
        e3 = normal;
        createONB(e3, e1, e2);

        sassert(areClose(e3.x, normal.x));
        sassert(areClose(e3.y, normal.y));
        sassert(areClose(e3.z, normal.z));

        sassert(areClose(e1.norm(), 1.0f, epsilon));
        sassert(areClose(e2.norm(), 1.0f, epsilon));
        sassert(areClose(e3.norm(), 1.0f, epsilon));

        sassert(areClose(dot(e1, e2), 0.0f, epsilon));
        sassert(areClose(dot(e2, e3), 0.0f, epsilon));
        sassert(areClose(dot(e3, e1), 0.0f, epsilon));
    }
}

void testReflection() {
    Normal3 n(2. / 3., 2. / 3., 1. / 3.);
    sassert(areClose(n.norm2(), 1.));

    Vec3 k(0., 0., 1.);
    sassert(reflect(k, n).isClose(Vec3(-4. / 9., -4. / 9., 7. / 9.)));

    PCG pcg;
    for (int i = 0; i < 10; i++) {
        sassert(areClose(reflect(pcg.randomVersor(), n).norm2(), 1.));
    }
}

void testRefraction() {
    Normal3 n(0., 0., 1.);
    Vec3 v = Vec3(1., 1., -1).normalize();

    sassert(refract(v, n, 1.).isClose(v)); // n1/n2 = 1
    sassert(refract(v, n, 100.).isClose(reflect(v, n))); // no refraction

    v = Vec3(1., 0., -1.).normalize();
    sassert(refract(v, n, 1. / std::sqrt(2.)).isClose(Vec3(0.5, 0., -0.5 * std::sqrt(3.)))); // from 45 to 30 deg

    PCG pcg;
    for (int i = 0; i < 10; i++) {
        sassert(areClose(refract(pcg.randomVersor(), n, 0.8).norm2(), 1.));
    }
}



int main() {
    testVec3operations();
    testPoint3operations();

    testTrasformation();
    testTransformationMultiplication();
    testVecPointMultiplication();
    testTransformationInverse();
    testTranslation();
    testRotation();
    testScaling();

    testONB();

    testReflection();
    testRefraction();
    
    std::cout << "All tests passed!\n";
    return 0;
}
