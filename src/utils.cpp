#include "utils.hpp"
#include "Vec3.hpp"
#include "Normal3.hpp"

// PCG

PCG::PCG(uint64_t initState, uint64_t initSeq) {
    state = 0;
    inc = (initSeq << 1u) | 1u;
    random();
    state = (state + initState);
    random();
}

uint32_t PCG::randomUint32() {
    uint64_t oldState = state;
    state = static_cast<uint64_t>(oldState * 6364136223846793005ULL + inc);
    uint32_t xorshifted = static_cast<uint32_t>(((oldState >> 18u) ^ oldState) >> 27u);
    uint32_t rot = static_cast<uint32_t>(oldState >> 59u);
    return (((xorshifted >> rot) | (xorshifted << ((-rot) & 31))));
}

// uses a simple rejection method, maybe faster than other methods in 3D (not tested)
Vec3 PCG::randomVersor() {
    while(true){
        float x = random(-1.0f, 1.0f), y = random(-1.0f, 1.0f), z = random(-1.0f, 1.0f);
        float norm2 = x * x + y * y + z * z;
        if (norm2 < 1.0f && norm2 != 0.0f) {
            return Vec3(x, y, z) / std::sqrt(norm2);
        }
    }
}

// cosine-weighted distribution around the z (local) axis
Vec3 PCG::sampleHemisphere(Vec3 n) {
    Vec3 e1, e2;
    createONB(n, e1, e2);
    float cos2 = random();
    float cosTheta = std::sqrt(cos2), sinTheta = std::sqrt(1.0f - cos2);
    float phi = 2.0f * PI * random();

    return e1 * std::cos(phi) * cosTheta + e2 * std::sin(phi) * cosTheta + n * sinTheta;
}

Vec3 PCG::sampleHemisphere(Normal3 n) {
    Vec3 nVec = n.toVec();
    return sampleHemisphere(nVec);
}



// other stuff

Vec3 reflect(const Vec3& v, const Normal3& n) {
    Vec3 nVec = n.toVec();
    return v - nVec * 2.0f * dot(nVec, v);
}

Vec3 refract(const Vec3& v, const Normal3& n, float refractionIndexRatio) {
    Vec3 nVec = -n.toVec(); // - because v enters the surface while n exits

    float cos = dot(nVec, v);
    float sin2 = 1 - cos * cos;

    // reflection if n1/n2 * sin(theta1) = sin(theta2) > 1
    if (refractionIndexRatio * refractionIndexRatio * sin2 > 1.0f) return reflect(v, n);

    // refraction, from https://physics.stackexchange.com/questions/435512/snells-law-in-vector-form
    return nVec * std::sqrt(1.0f - refractionIndexRatio * refractionIndexRatio * sin2) + (v - cos * nVec) * refractionIndexRatio;
}



void validateFloatVariable(std::string& s, std::unordered_map<std::string, float>& floatVariables) {
    std::string key;
    float value;

    size_t position = s.find(':');
    if (position == std::string::npos)
        throw std::invalid_argument("ERROR: \"" + s + "\" does not define a float variable\n"
                                    "the correct syntax is --float=name:value");

    key = s.substr(0, position);

    auto stringVal = s.erase(0, position + 1);
    try { value = std::stof(stringVal); }
    catch (std::out_of_range& e) { throw std::out_of_range(stringVal + " is out of float range"); }
    catch (std::invalid_argument& e) { throw std::invalid_argument(stringVal + " is not a valid number"); }

    floatVariables[key] = value;
}