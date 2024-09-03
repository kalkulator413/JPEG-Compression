#include <cassert>
#include <cstdlib>
#include <cstring>

template<typename T, size_t Alignment=16>
class Matrix
{
public:
    size_t nRows;
    size_t nCols;
    Matrix(size_t R, size_t C);
    Matrix(const Matrix& other);
    Matrix& operator=(const Matrix& other);
    ~Matrix();
    void set(size_t r, size_t c, T elem);
    T& operator()(size_t r, size_t c = 0);
    T& operator[](size_t n);
private:
    T* data;
};

template<typename T, size_t Alignment>
inline Matrix<T, Alignment>::Matrix(size_t R, size_t C) : nRows(R), nCols(C)
{
    if constexpr (Alignment == 0)
        data = static_cast<T*>(std::malloc(nRows * nCols * sizeof(T)));
    else
        data = static_cast<T*>(std::aligned_alloc(Alignment, nRows * nCols * sizeof(T)));
}

template<typename T, size_t Alignment>
inline Matrix<T, Alignment>::~Matrix()
{
    std::free(data);
}


template<typename T, size_t Alignment>
Matrix<T, Alignment>::Matrix(const Matrix& other) : nRows(other.nRows), nCols(other.nCols)
{
    size_t totalSize = nRows * nCols * sizeof(T);
    if constexpr (Alignment == 0)
        data = static_cast<T*>(std::malloc(totalSize));
    else
        data = static_cast<T*>(std::aligned_alloc(Alignment, totalSize));

    std::memcpy(data, other.data, totalSize);
}

template<typename T, size_t Alignment>
Matrix<T, Alignment>& Matrix<T, Alignment>::operator=(const Matrix& other)
{
    if (this == &other)
        return *this;

    std::free(data);

    nRows = other.nRows;
    nCols = other.nCols;
    size_t totalSize = nRows * nCols * sizeof(T);
    if constexpr (Alignment == 0)
        data = static_cast<T*>(std::malloc(totalSize));
    else
        data = static_cast<T*>(std::aligned_alloc(Alignment, totalSize));

    std::memcpy(data, other.data, totalSize);

    return *this;
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
