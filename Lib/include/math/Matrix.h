#include <cassert>
#include <cstdlib>

template<typename T, size_t Alignment=16>
class Matrix
{
public:
    size_t nRows;
    size_t nCols;
    Matrix(size_t R, size_t C);
    ~Matrix();
    void set(size_t r, size_t c, T elem);
    T& operator()(size_t r, size_t c = 0);
    T& operator[](size_t n);
    void setOnes();
    void setZeros();
private:
    T* data;
};

template<typename T, size_t Alignment>
inline Matrix<T, Alignment>::Matrix(size_t R, size_t C) : nRows(R), nCols(C)
{
    data = static_cast<T*>(std::aligned_alloc(Alignment, nRows * nCols * 8 * sizeof(T)));
}

template<typename T, size_t Alignment>
inline Matrix<T, Alignment>::~Matrix()
{
    std::free(data);
}

template<typename T, size_t Alignment>
inline void Matrix<T, Alignment>::set(size_t r, size_t c, T elem)
{
    assert(r < nRows && c < nCols);
    data[nCols * r + c] = elem;
}

template<typename T, size_t Alignment>
inline T& Matrix<T, Alignment>::operator()(size_t r, size_t c)
{
    assert(r < nRows && c < nCols);
    return data[nCols * r + c];
}

template<typename T, size_t Alignment>
inline T& Matrix<T, Alignment>::operator[](size_t n)
{
    assert(n < nRows * nCols);
    return data[n];
}

template<typename T, size_t Alignment>
inline void Matrix<T, Alignment>::setOnes()
{
    size_t size = nRows * nCols;
    for (size_t i = 0; i < size; ++i)
    {
        data[i] = static_cast<T>(1);
    }
}

template<typename T, size_t Alignment>
inline void Matrix<T, Alignment>::setZeros()
{
    size_t size = nRows * nCols;
    for (size_t i = 0; i < size; ++i)
    {
        data[i] = static_cast<T>(0);
    }
}