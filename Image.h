#ifndef FINALWORK_IMAGE_H
#define FINALWORK_IMAGE_H

#include <vector>
#include <string>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <stdlib.h>
#include <time.h>
#include <cmath>
#include <opencv2/opencv.hpp>

template<typename PixelType>
class Image
{
    using DataType = std::vector<std::vector<PixelType>>;

public:
    Image(){};
    void Read(const std::string& fileName);
    void GetHistogram(int []);
    void LocalBinaryPattern(int [], int radius);
    void GetNeighbors(int i, int j, int radius, std::vector<int> &arr);
    void GetSubHistogram(int from_i, int to_i, int from_j, int to_j, std::vector<int> &c_histogram);

private:

    DataType Data; // matriz de pixels
    DataType ProcessedData;

    std::size_t Rows{};
    std::size_t Columns{};

};

template<typename  PixelType>
void Image<PixelType>::Read(const std::string& fileName)
{
    cv::Mat image = cv::imread(fileName, cv::IMREAD_COLOR);

    if (!image.data)
    {
        std::cerr<<"No image data\n";
        return;
    }

    Rows = image.rows;
    Columns = image.cols;

    Data = DataType(Rows, std::vector<PixelType>(Columns, PixelType{}));
    ProcessedData = DataType(Rows, std::vector<PixelType>(Columns, PixelType{}));

    uchar red, green, blue;
    for (unsigned r=0; r < Rows; ++r)
    {
        cv::Vec3b * row = image.ptr<cv::Vec3b>(r);
        for (unsigned c=0; c < Columns; ++c)
        {
            red =   row[c][2];
            green = row[c][1];
            blue =  row[c][0];

            if constexpr (std::is_fundamental<PixelType>::value) //uchar, short, float (gray)
            {
                Data[r][c] = static_cast<PixelType>((red + green + blue)/3);
                ProcessedData[r][c] = static_cast<PixelType>((red + green + blue)/3);
            }
            else //RGB LAB, channels...
            {
                //memoria
                Data[r][c][0] = static_cast<typename PixelType::ValueType>(red);
                Data[r][c][1] = static_cast<typename PixelType::ValueType>(green);
                Data[r][c][2] = static_cast<typename PixelType::ValueType>(blue);
            }
        }
    }

    //std::cout << "Pixels " <<Rows * Columns / 256.0 << std::endl;
}

template<typename  PixelType>
void Image<PixelType>::GetHistogram(int histogram[]) {
    for (unsigned i = 0; i < Rows; ++i){
        for (unsigned j = 0; j < Columns; ++j){
            histogram[Data[i][j]]++;
        }
    }
}

template<typename  PixelType>
void Image<PixelType>::LocalBinaryPattern(int histogram[], int radius) {
    std::vector<int> neighbors;
    std::vector<int> c_histogram;

    for (unsigned i = 0; i < Rows; ++i){
        for (unsigned j = 0; j < Columns; ++j){

            auto Center = Data[i][j];
            GetNeighbors(i, j, radius, neighbors);
            long long sum = 0;

            for (unsigned p = 0; p < neighbors.size(); ++p){
                auto s = (neighbors[p] - Center) < 0 ? 0 : 1;
                sum += s * pow(2, p);
            }
            ProcessedData[i][j] = sum;

        }
    }

    float sub_row_length = Rows / 5.f;
    float sub_col_length = Columns / 5.f;
    sub_row_length = round(sub_row_length);
    sub_col_length = round(sub_col_length);

    // 1) i = [0 < sub_row_length] j = [0 < sub_col_length]
    GetSubHistogram(0, sub_row_length, 0, sub_col_length, c_histogram);

    // 2) i = [0 < sub_row_length] j = [sub_col_length < sub_col_length * 2]
    GetSubHistogram(0, sub_row_length, sub_col_length, sub_col_length * 2, c_histogram);

    // 3) i = [0 < sub_row_length] j = [sub_col_length * 2 < sub_col_length * 3]
    GetSubHistogram(0, sub_row_length, sub_col_length * 2, sub_col_length * 3, c_histogram);

    // 4) i = [0 < sub_row_length] j = [sub_col_length * 3 < sub_col_length * 4]
    GetSubHistogram(0, sub_row_length, sub_col_length * 3, sub_col_length * 4, c_histogram);

    // 5) i = [0 < sub_row_length] j = [sub_col_length * 4 < Cols]
    GetSubHistogram(0, sub_row_length, sub_col_length * 4, Columns, c_histogram);

    // 6) i = [sub_row_length < sub_row_length * 2] j = [0 < sub_col_length]
    GetSubHistogram(sub_row_length, sub_row_length * 2, 0, sub_col_length, c_histogram);

    // 7) i = [sub_row_length < sub_row_length * 2] j = [sub_col_length < sub_col_length * 2]
    GetSubHistogram(sub_row_length, sub_row_length * 2, sub_col_length, sub_col_length * 2, c_histogram);

    // 8) i = [sub_row_length < sub_row_length * 2] j = [sub_col_length * 2 < sub_col_length * 3]
    GetSubHistogram(sub_row_length, sub_row_length * 2, sub_col_length * 2, sub_col_length * 3, c_histogram);

    // 9) i = [sub_row_length < sub_row_length * 2] j = [sub_col_length * 3 < sub_col_length * 4]
    GetSubHistogram(sub_row_length, sub_row_length * 2, sub_col_length * 3, sub_col_length * 4, c_histogram);

    // 10) i = [sub_row_length < sub_row_length * 2] j = [sub_col_length * 4 < Cols]
    GetSubHistogram(sub_row_length, sub_row_length * 2, sub_col_length * 4, Columns, c_histogram);

    // 11) i = [sub_row_length * 2 < sub_row_length * 3] j = [0 < sub_col_length]
    GetSubHistogram(sub_row_length * 2, sub_row_length * 3, 0, sub_col_length, c_histogram);

    // 12) i = [sub_row_length * 2 < sub_row_length * 3] j = [sub_col_length < sub_col_length * 2]
    GetSubHistogram(sub_row_length * 2, sub_row_length * 3, sub_col_length, sub_col_length * 2, c_histogram);

    // 13) i = [sub_row_length * 2 < sub_row_length * 3] j = [sub_col_length * 2 < sub_col_length * 3]
    GetSubHistogram(sub_row_length * 2, sub_row_length * 3, sub_col_length * 2, sub_col_length * 3, c_histogram);

    // 14) i = [sub_row_length * 2 < sub_row_length * 3] j = [sub_col_length * 3 < sub_col_length * 4]
    GetSubHistogram(sub_row_length * 2, sub_row_length * 3, sub_col_length * 3, sub_col_length * 4, c_histogram);

    // 15) i = [sub_row_length * 2 < sub_row_length * 3] j = [sub_col_length * 4 < Cols]
    GetSubHistogram(sub_row_length * 2, sub_row_length * 3, sub_col_length * 4, Columns, c_histogram);

    // 16) i = [sub_row_length * 3 < sub_row_length * 4] j = [0 < sub_col_length]
    GetSubHistogram(sub_row_length * 3, sub_row_length * 4, 0, sub_col_length, c_histogram);

    // 17) i = [sub_row_length * 3 < sub_row_length * 4] j = [sub_col_length < sub_col_length * 2]
    GetSubHistogram(sub_row_length * 3, sub_row_length * 4, sub_col_length, sub_col_length * 2, c_histogram);

    // 18) i = [sub_row_length * 3 < sub_row_length * 4] j = [sub_col_length * 2 < sub_col_length * 3]
    GetSubHistogram(sub_row_length * 3, sub_row_length * 4, sub_col_length * 2, sub_col_length * 3, c_histogram);

    // 19) i = [sub_row_length * 3 < sub_row_length * 4] j = [sub_col_length * 3 < sub_col_length * 4]
    GetSubHistogram(sub_row_length * 3, sub_row_length * 4, sub_col_length * 3, sub_col_length * 4, c_histogram);

    // 20) i = [sub_row_length * 3 < sub_row_length * 4] j = [sub_col_length * 4 < Cols]
    GetSubHistogram(sub_row_length * 3, sub_row_length * 4, sub_col_length * 4, Columns, c_histogram);

    // 21) i = [sub_row_length * 4 < Rows] j = [0 < sub_col_length]
    GetSubHistogram(sub_row_length * 4, Rows, 0, sub_col_length, c_histogram);

    // 22) i = [sub_row_length * 4 < Rows] j = [sub_col_length < sub_col_length * 2]
    GetSubHistogram(sub_row_length * 4, Rows, sub_col_length, sub_col_length * 2, c_histogram);

    // 23) i = [sub_row_length * 4 < Rows] j = [sub_col_length * 2 < sub_col_length * 3]
    GetSubHistogram(sub_row_length * 4, Rows, sub_col_length * 2, sub_col_length * 3, c_histogram);

    // 24) i = [sub_row_length * 4 < Rows] j = [sub_col_length * 3 < sub_col_length * 4]
    GetSubHistogram(sub_row_length * 4, Rows, sub_col_length * 3, sub_col_length * 4, c_histogram);

    // 25) i = [sub_row_length * 4 < Rows] j = [sub_col_length * 4 < Cols]
    GetSubHistogram(sub_row_length * 4, Rows, sub_col_length * 4, Columns, c_histogram);


    for (int i = 0; i < c_histogram.size(); ++i) histogram[i] = c_histogram[i];

}

template<typename  PixelType>
void Image<PixelType>::GetNeighbors(int i, int j, int radius, std::vector<int> &arr) {
    arr.clear();
    int points = 8 * radius;
    float angle = 0.f;
    int x, y;

    while(points--)
    {
        auto px = i + (radius * cos(angle));
        auto py = j + (radius * sin(angle));

        x = round(px);
        y = round(py);

        if (x < 0 || x >= Rows) x = i;
        if (y < 0 || x >= Columns) y = j;

        arr.push_back(Data[x][y]);

        angle += 2.f * 3.1415 / points;
    }
}

template<typename  PixelType>
void Image<PixelType>::GetSubHistogram(int from_i, int to_i, int from_j, int to_j, std::vector<int> &c_histogram) {
    int arr[256] = {0};

    for (int i = from_i; i < to_i; ++i){
        for (int j = from_j; j < to_j; ++j){
            arr[ProcessedData[i][j]]++;
        }
    }

    //concatenate the histogram
    for (int i = 0; i < 256; ++i){
        c_histogram.push_back(arr[i]);
    }
}

#endif //FINALWORK_IMAGE_H
